#include <errno.h>
#include <reent.h>
#include <stdint.h>
#include <stdbool.h>
#include <sys/stat.h>

#ifdef CONFIG_CONSOLE_WO
#include "bflb_wo.h"
#else
#include "bflb_uart.h"
#endif

/** @addtogroup ttyin_config
-----------------------------------------------------------------------------
* @{
----------------------------------------------------------------------------*/
/*!< the following configuration is only valid for tty input */

/*!< block mode or nonblock mode */
#ifndef CONFIG_TTYIN_NONBLOCK
#define CONFIG_TTYIN_NONBLOCK 0
#endif

/*!< use raw data */
#ifndef CONFIG_TTYIN_RAWMODE
#define CONFIG_TTYIN_RAWMODE 0
#endif

/*!< map crnl -> nl */
#ifndef CONFIG_TTYIN_CRNL2NL
#define CONFIG_TTYIN_CRNL2NL 1
#endif

/*!< map crnl -> cr */
#ifndef CONFIG_TTYIN_CRNL2CR
#define CONFIG_TTYIN_CRNL2CR 0
#endif

/*!< map nlcr -> nl */
#ifndef CONFIG_TTYIN_NLCR2NL
#define CONFIG_TTYIN_NLCR2NL 1
#endif

/*!< map nlcr -> cr */
#ifndef CONFIG_TTYIN_NLCR2CR
#define CONFIG_TTYIN_NLCR2CR 0
#endif

/*!< map cr -> nl */
#ifndef CONFIG_TTYIN_CR2NL
#define CONFIG_TTYIN_CR2NL 1
#endif

/*!< map nl -> cr */
#ifndef CONFIG_TTYIN_NL2CR
#define CONFIG_TTYIN_NL2CR 0
#endif
/*---------------------------------------------------------------------------
* @}            ttyin_config
----------------------------------------------------------------------------*/

/** @addtogroup ttyin_fifo
-----------------------------------------------------------------------------
* @{
----------------------------------------------------------------------------*/
struct _ttyin_fifo {
    volatile uint32_t in;
    volatile uint32_t out;
    volatile uint32_t mask;
    volatile void *data;
};

/*****************************************************************************
* @brief        macro to define and init a fifo object
* 
* @param        name        name of the fifo
* @param        size        the number of elements in the fifo, 
*                           this must be a power of 2 !!!
*****************************************************************************/
#define TTYIN_FIFO_DEFINE(name, size)                                       \
    struct {                                                                \
        struct _ttyin_fifo fifo;                                            \
        uint8_t buf[(((size) < 2) || ((size) & ((size)-1))) ? -1 : (size)]; \
    }(name) = {                                                             \
        .fifo.in = 0,                                                       \
        .fifo.out = 0,                                                      \
        .fifo.mask = (size)-1,                                              \
        .fifo.data = (name).buf                                             \
    }

/*****************************************************************************
* @brief        get fifo used space size
* 
* @param[in]    fifo        fifo object
* 
* @retval uint32_t          used space size
*****************************************************************************/
static inline uint32_t _ttyin_fifo_used(struct _ttyin_fifo *fifo)
{
    return fifo->in - fifo->out;
}

/*****************************************************************************
* @brief        get fifo unused space size
* 
* @param[in]    fifo        fifo object
* 
* @retval uint32_t          unused space size
*****************************************************************************/
static inline uint32_t _ttyin_fifo_free(struct _ttyin_fifo *fifo)
{
    return (fifo->mask + 1) - (fifo->in - fifo->out);
}

/*****************************************************************************
* @brief        returns true if the fifo is full
* 
* @param[in]    fifo        fifo object
* 
* @retval bool              
*****************************************************************************/
static inline bool _ttyin_fifo_is_full(struct _ttyin_fifo *fifo)
{
    return _ttyin_fifo_used(fifo) > fifo->mask;
}

/*****************************************************************************
* @brief        returns true if the fifo is empty
* 
* @param[in]    name        fifo object
* 
* @retval bool              
*****************************************************************************/
static inline bool _ttyin_fifo_is_empty(struct _ttyin_fifo *fifo)
{
    return fifo->in == fifo->out;
}

/*****************************************************************************
* @brief        put byte into the fifo
* 
* @param[in]    name        fifo object
* @param[in]    ch          data
* 
*****************************************************************************/
static inline void _ttyin_fifo_put(struct _ttyin_fifo *fifo, uint8_t ch)
{
#if defined(CONFIG_TTYIN_CR2NL) && CONFIG_TTYIN_CR2NL
    ch = (ch == '\r') ? '\n' : ch;
#elif defined(CONFIG_TTYIN_NL2CR) && CONFIG_TTYIN_NL2CR
    ch = (ch == '\n') ? '\r' : ch;
#endif

    if (!_ttyin_fifo_is_full(fifo)) {
        ((volatile uint8_t *)(fifo->data))[fifo->in & fifo->mask] = ch;
        fifo->in++;
    }
}

/*****************************************************************************
* @brief        get byte from the fifo
* 
* @param[in]    fifo        fifo object
* 
* @retval int               data
*****************************************************************************/
static inline int _ttyin_fifo_get(struct _ttyin_fifo *fifo)
{
    register int ch = -1;

#if defined(CONFIG_TTYIN_NONBLOCK) && CONFIG_TTYIN_NONBLOCK
    if (!_ttyin_fifo_is_empty(fifo))
#else
    while (_ttyin_fifo_is_empty(fifo)) {}
#endif
    {
        ch = ((volatile uint8_t *)(fifo->data))[fifo->out & fifo->mask];
        fifo->out++;
    }

    return ch;
}

#define ttyin_fifo_used(__fifo)      _ttyin_fifo_used(&((__fifo).fifo))
#define ttyin_fifo_free(__fifo)      _ttyin_fifo_free(&((__fifo).fifo))
#define ttyin_fifo_is_full(__fifo)   _ttyin_fifo_is_full(&((__fifo).fifo))
#define ttyin_fifo_is_empty(__fifo)  _ttyin_fifo_is_empty(&((__fifo).fifo))
#define ttyin_fifo_put(__fifo, __ch) _ttyin_fifo_put(&((__fifo).fifo), (__ch))
#define ttyin_fifo_get(__fifo)       _ttyin_fifo_get(&((__fifo).fifo))

/*---------------------------------------------------------------------------
* @}            ttyin_fifo
----------------------------------------------------------------------------*/
struct bflb_device_s *console = NULL;

static TTYIN_FIFO_DEFINE(stdin_fifo, 512);

#ifndef CONFIG_CONSOLE_WO
void console_receive_isr(int irq, void *arg)
{
    uint32_t intstatus = bflb_uart_get_intstatus(console);

    if (intstatus & UART_INTSTS_RX_FIFO) {
        while (bflb_uart_rxavailable(console)) {
            ttyin_fifo_put(stdin_fifo, bflb_uart_getchar(console));
        }
    }

    if (intstatus & UART_INTSTS_RTO) {
        while (bflb_uart_rxavailable(console)) {
            ttyin_fifo_put(stdin_fifo, bflb_uart_getchar(console));
        }
        bflb_uart_int_clear(console, UART_INTCLR_RTO);
    }
}
#endif

#ifdef CONFIG_CONSOLE_WO
void bflb_wo_set_console(struct bflb_device_s *dev)
{
    console = dev;
}
#else
void bflb_uart_set_console(struct bflb_device_s *dev)
{
    console = dev;
    // bflb_uart_rxint_mask(console, false);
    // bflb_irq_attach(console->irq_num, console_receive_isr, console);
    // bflb_irq_enable(console->irq_num);
}
#endif

/*****************************************************************************
* @brief        open
* 
* @param[in]    reent       pointer to reentrant struct
* @param[in]    path        file path string pointer
* @param[in]    flags       open mode in fcntl.h
* @param[in]    mode        permission mode
* 
* @retval int               >=0:fd -1:Error
*****************************************************************************/
int _open_tty_r(struct _reent *reent, const char *path, int flags, int mode)
{
    if (strncmp("/dev/null", path, 9) == 0) {
        return 0x3ff;
    } else if (strncmp("/dev/stdin", path, 10) == 0) {
        return 0;
    } else if (strncmp("/dev/stdout", path, 11) == 0) {
        return 1;
    } else if (strncmp("/dev/stderr", path, 11) == 0) {
        return 2;
    } else {
        reent->_errno = ENOENT;
        return -1;
    }
}

/*****************************************************************************
* @brief        close
* 
* @param[in]    reent       pointer to reentrant struct
* @param[in]    fd          file descriptors
* 
* @retval int               0:Success -1:Error
*****************************************************************************/
int _close_tty_r(struct _reent *reent, int fd)
{
    if (fd == 0x3ff) {
        return 0;
    } else if (fd == 0) {
        return 0;
    } else if (fd == 1) {
        return 0;
    } else if (fd == 2) {
        return 0;
    } else {
        reent->_errno = EBADF;
        return -1;
    }
}

/*****************************************************************************
* @brief        read
* 
* @param[in]    reent       pointer to reentrant struct
* @param[in]    fd          file descriptors
* @param[in]    ptr         pointer to buffer
* @param[in]    size        number of bytes read
* 
* @retval _ssize_t          actual number of bytes read
*****************************************************************************/
_ssize_t _read_tty_r(struct _reent *reent, int fd, void *ptr, size_t size)
{
    if (fd == 0x3ff) {
        return -1;
    } else if (fd == 0) {
        int ch;

        for (size_t i = 0; i < size; i++) {
            if ((ch = ttyin_fifo_get(stdin_fifo)) <= -1) {
                return i;
            } else {
                ((uint8_t *)ptr)[i] = ch;
#ifndef CONFIG_CONSOLE_WO
                bflb_uart_putchar(console, ch);
#endif
            }
        }

        return size;
    } else if ((fd == 1) || (fd == 2)) {
        reent->_errno = EACCES;
        return -1;
    } else {
        reent->_errno = EBADF;
        return -1;
    }
}

/*****************************************************************************
* @brief        write
* 
* @param[in]    reent       pointer to reentrant struct
* @param[in]    fd          file descriptors
* @param[in]    ptr         pointer to buffer
* @param[in]    size        number of bytes write
* 
* @retval _ssize_t          actual number of bytes write
*****************************************************************************/
_ssize_t _write_tty_r(struct _reent *reent, int fd, const void *ptr, size_t size)
{
    if (fd == 0x3ff) {
        return size;
    } else if (fd == 0) {
        reent->_errno = EACCES;
        return -1;
    } else if ((fd == 1) || (fd == 2)) {
        for (size_t i = 0; i < size; i++) {
#ifdef CONFIG_CONSOLE_WO
            bflb_wo_uart_putchar(console, ((uint8_t *)ptr)[i]);
#else
            bflb_uart_putchar(console, ((uint8_t *)ptr)[i]);
#endif
        }
        return size;
    } else {
        reent->_errno = EBADF;
        return -1;
    }
}

/*****************************************************************************
* @brief        fstat (not supported)
* 
* @param[in]    reent       pointer to reentrant struct
* @param[in]    fd          file descriptors
* @param[out]   st          file stat
* 
* @retval int               0:Success -1:Error
*****************************************************************************/
int _fstat_tty_r(struct _reent *reent, int fd, struct stat *st)
{
    if (fd == 0x3ff) {
        st->st_mode = 0666 | S_IFCHR;
        return 0;
    } else if (fd == 0) {
        st->st_mode = 0444 | S_IFCHR;
        return 0;
    } else if ((fd == 1) || (fd == 2)) {
        st->st_mode = 0222 | S_IFCHR;
        return 0;
    } else {
        reent->_errno = EBADF;
        return -1;
    }
}

/*****************************************************************************
* @brief        stat
* 
* @param[in]    reent       pointer to reentrant struct
* @param[in]    path        file path
* @param[out]   st          file stat
* 
* @retval int               0:Success -1:Error
*****************************************************************************/
int _stat_tty_r(struct _reent *reent, const char *path, struct stat *st)
{
    if (strncmp("/dev/null", path, 9) == 0) {
        return _fstat_tty_r(reent, 0x3ff, st);
    } else if (strncmp("/dev/stdin", path, 10) == 0) {
        return _fstat_tty_r(reent, 0, st);
    } else if (strncmp("/dev/stdout", path, 11) == 0) {
        return _fstat_tty_r(reent, 1, st);
    } else if (strncmp("/dev/stderr", path, 11) == 0) {
        return _fstat_tty_r(reent, 2, st);
    } else {
        reent->_errno = EBADF;
        return -1;
    }
}
