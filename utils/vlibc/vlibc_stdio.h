#ifndef _VLIBC_STDIO_H
#define _VLIBC_STDIO_H

#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdint.h>

#ifdef CONFIG_VLIBC_FATFS
#include "ff.h"
#endif

/** @addtogroup Types
 * @{
 */

struct __vlibc_io {
    char *bg; /*!< buffer begin pointer */
    char *wp; /*!< buffer write pointer */
    char *rp; /*!< buffer read pointer */
    char *ed; /*!< buffer end pointer */

    uint32_t dev; /*!< io device */
    uint8_t flag; /*!< io flag */
    uint8_t vbuf; /*!< buffer mode */
    uint8_t abuf; /*!< buffer auto */
    uint8_t err;  /*!< error */
};

typedef struct {
    uint32_t magic;

    union {
        struct __vlibc_io *io;
#ifdef CONFIG_VLIBC_FATFS
        FIL *file;
#else
        int *file;
#endif
    };
} vlibc_file_t;

/*!< file type */
#define VLIBC_FILE vlibc_file_t

/**
 * @}
 */

/** @addtogroup stdio extra
 * @{
 */
extern uint32_t __vlibc_io_init(const char *name, uint8_t mode);
extern uint32_t __vlibc_io_deinit(struct __vlibc_io *io);
extern size_t __vlibc_io_mem2dev(struct __vlibc_io *io, const void *ptr, size_t size);
extern size_t __vlibc_io_dev2mem(struct __vlibc_io *io, void *ptr, size_t size);
/**
 * @}
 */

/** @addtogroup Marcos
 * @{
 */

#define _VLIBC_MAGIC_MASK      ((unsigned int)(0xffff0000))
#define _VLIBC_IO_MAGIC_CODE   ((unsigned int)(0x10de0000))
#define _VLIBC_FILE_MAGIC_CODE ((unsigned int)(0xf11e0000))

#define _VLIBC_IO_WRITE ((unsigned char)(0x01))
#define _VLIBC_IO_READ  ((unsigned char)(0x02))

/*!< io buffer size */
#define VLIBC_BUFSIZ 256

/*!< file stack buffer size */
#define VLIBC_FBUFSIZ 256

/*!< max open file count at the same time */
#define VLIBC_FOPEN_MAX 20

/*!< max file name length */
#define VLIBC_FILENAME_MAX 256

/*!< max io name length */
#define _VLIBC_IONAME_MAX 32

/*!< max tmpnam file name length */
#define VLIBC_L_tmpnam VLIBC_FILENAME_MAX

/*!< max tmpnam rand name */
#define VLIBC_TMP_MAX 0

/*!< stand io */
extern vlibc_file_t *__vlibc_stdio_fileptrs[3];
#define vlibc_stdin  (__vlibc_stdio_fileptrs[0])
#define vlibc_stdout (__vlibc_stdio_fileptrs[1])
#define vlibc_stderr (__vlibc_stdio_fileptrs[2])

/**
 * @}
 */

/** @addtogroup stdio functions
 * @{
 */

extern void vlibc_clearerr(VLIBC_FILE *);
extern int vlibc_feof(VLIBC_FILE *);
extern int vlibc_ferror(VLIBC_FILE *);

extern VLIBC_FILE *vlibc_fopen(const char *, const char *);
extern VLIBC_FILE *vlibc_freopen(const char *, const char *, VLIBC_FILE *);
extern int vlibc_fclose(VLIBC_FILE *);

extern size_t vlibc_fread(void *, size_t, size_t, VLIBC_FILE *);
extern size_t vlibc_fwrite(const void *, size_t, size_t, VLIBC_FILE *);

extern int vlibc_fflush(VLIBC_FILE *);
extern int vlibc_fseek(VLIBC_FILE *, long, int);
extern long vlibc_ftell(VLIBC_FILE *);

extern int vlibc_remove(const char *);
extern int vlibc_rename(const char *, const char *);
extern void vlibc_rewind(VLIBC_FILE *);

extern void vlibc_setbuf(VLIBC_FILE *, char *);
extern int vlibc_setvbuf(VLIBC_FILE *, char *, int, size_t);

extern VLIBC_FILE *vlibc_tmpfile(void);
extern char *vlibc_tmpnam(char *);

extern int vlibc_fprintf(VLIBC_FILE *, const char *, ...);
extern int vlibc_printf(const char *, ...);
extern int vlibc_sprintf(char *, const char *, ...);
extern int vlibc_snprintf(char *, size_t, const char *, ...);

extern int vlibc_vfprintf(VLIBC_FILE *, const char *, va_list);
extern int vlibc_vprintf(const char *, va_list);
extern int vlibc_vsprintf(char *, const char *, va_list);
extern int vlibc_vsnprintf(char *, size_t, const char *, va_list);

extern int vlibc_fscanf(VLIBC_FILE *, const char *, ...);
extern int vlibc_scanf(const char *, ...);
extern int vlibc_sscanf(const char *, const char *, ...);

extern int vlibc_vfscanf(VLIBC_FILE *, const char *, va_list);
extern int vlibc_vscanf(const char *, va_list);
extern int vlibc_vsscanf(const char *, const char *, va_list);

extern int vlibc_fgetc(VLIBC_FILE *);
extern char *vlibc_fgets(char *, int, VLIBC_FILE *);

extern int vlibc_fputc(int, VLIBC_FILE *);
extern int vlibc_fputs(const char *, VLIBC_FILE *);

extern int vlibc_getc(VLIBC_FILE *);
extern int vlibc_getchar(void);
extern char *vlibc_gets(char *);

extern int vlibc_putc(int, VLIBC_FILE *);
extern int vlibc_putchar(int);
extern int vlibc_puts(const char *);

extern void vlibc_perror(const char *);

/**
 * @}
 */

#endif
