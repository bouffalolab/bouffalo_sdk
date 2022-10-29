#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include "vlibc_stdio.h"

#ifndef vlibc_malloc
#define vlibc_malloc malloc
#endif

#ifndef vlibc_free
#define vlibc_free free
#endif

#define _VLIBC_IO_HAVE_WRITE ((unsigned char)(0x04))
#define _VLIBC_IO_HAVE_READ  ((unsigned char)(0x08))

#define _VLIBC_ABUF_ENABLE  ((unsigned char)(0x01))
#define _VLIBC_ABUF_DISABLE ((unsigned char)(0x00))

#define _VLIBC_TYPEIS_IO   ((int)(0x01))
#define _VLIBC_TYPEIS_FILE ((int)(0x02))

#define vlibc_file(_stream) ((vlibc_file_t *)(_stream))

#ifdef CONFIG_VLIBC_FATFS
int FRESULT_to_errno[20] = {
    0,
    EIO,
    EPIPE,
    EIO,
    ENOENT,
    ENOENT,
    ENOEXEC,
    ENOSPC,
    EACCES,
    ENXIO,
    EROFS,
    ENXIO,
    ENXIO,
    EPERM,
    EPERM,
    EBUSY,
    EACCES,
    ENOMEM,
    EMFILE,
    EINVAL
};
#endif

#ifdef CONFIG_VLIBC_DEBUG
#define CHECK_FILE(_stream, __ret)       \
    do {                                 \
        if ((void *)(_stream) == NULL) { \
            errno = EINVAL;              \
            return __ret;                \
        }                                \
    } while (0)
#else
#define CHECK_FILE(_stream, __ret) \
    do {                           \
    } while (0)
#endif

#define IF_IO_DEV(_stream) \
    if (((vlibc_file(_stream)->magic) & _VLIBC_MAGIC_MASK) == _VLIBC_IO_MAGIC_CODE)

#define IF_FILE(_stream) \
    if (((vlibc_file(_stream)->magic) & _VLIBC_MAGIC_MASK) == _VLIBC_FILE_MAGIC_CODE)

vlibc_file_t *__vlibc_stdio_fileptrs[3] = { NULL, NULL, NULL };

/**
 *   @brief
 *   @param  stream
 */
void vlibc_clearerr(VLIBC_FILE *stream)
{
    CHECK_FILE(stream, /*!< no return */);
    CHECK_FILE(stream->file, /*!< no return */);

    IF_IO_DEV(stream)
    {
        stream->io->err = 0;
    }
    else IF_FILE(stream)
    {
#ifdef CONFIG_VLIBC_FATFS
        f_error(stream->file) = 0;
#endif
    }
    else
    {
    }
}

/**
 *   @brief
 *   @param  stream
 *   @return int
 */
int vlibc_feof(VLIBC_FILE *stream)
{
    CHECK_FILE(stream, EOF);
    CHECK_FILE(stream->file, EOF);

    IF_IO_DEV(stream)
    {
        return 0;
    }
    else IF_FILE(stream)
    {
#ifdef CONFIG_VLIBC_FATFS
        return f_eof(stream->file);
#else
        errno = EIO;
        return EOF;
#endif
    }
    else
    {
        errno = EBADF;
        return EOF;
    }
}

/**
 *   @brief         
 *   @param  stream                 
 *   @return int 
 */
int vlibc_ferror(VLIBC_FILE *stream)
{
    CHECK_FILE(stream, EOF);
    CHECK_FILE(stream->file, EOF);

    IF_IO_DEV(stream)
    {
        return stream->io->err;
    }
    else IF_FILE(stream)
    {
#ifdef CONFIG_VLIBC_FATFS
        int fresult;

        fresult = f_error(stream->file);

        return (fresult == FR_OK) ? 0 : EOF;
#else
        errno = EIO;
        return EOF;
#endif
    }
    else
    {
        errno = EBADF;
        return EOF;
    }
}

static int __get_mode_vlibc_fopen(const char *mode, unsigned char *iomode, unsigned char *openmode)
{
    /*!< get file and io open mode */
    switch (*mode) {
        case 'r':
#ifdef CONFIG_VLIBC_FATFS
            *openmode = FA_READ;
#endif
            *iomode = _VLIBC_IO_READ;
            break;
        case 'w':
#ifdef CONFIG_VLIBC_FATFS
            *openmode = FA_CREATE_ALWAYS | FA_WRITE;
#endif
            *iomode = _VLIBC_IO_WRITE;
            break;
        case 'a':
#ifdef CONFIG_VLIBC_FATFS
            *openmode = FA_OPEN_APPEND | FA_WRITE;
#endif
            *iomode = _VLIBC_IO_WRITE;
            break;
        case 'x':
#ifdef CONFIG_VLIBC_FATFS
            *openmode = FA_CREATE_NEW | FA_WRITE;
#endif
            *iomode = _VLIBC_IO_WRITE;
            break;
        default:
            errno = EINVAL;
            return EOF;
    }

    for (int i = 1; i < 7; ++i) {
        switch (*++mode) {
            case '\0':
                break;
            case '+':
#ifdef CONFIG_VLIBC_FATFS
                *openmode |= (FA_WRITE | FA_READ);
#endif
                *iomode |= (_VLIBC_IO_WRITE | _VLIBC_IO_READ);
                continue;
            case 'b':
                continue;
            default:
                continue;
        }
        break;
    }

    return 0;
}

static int __check_type_vlibc_fopen(const char *name, unsigned char iomode)
{
    /*!< check type */
    switch (*name) {
        case '>':
        case '<': {
            unsigned char count = 1;
            for (int i = 0; i < VLIBC_FILENAME_MAX; i++) {
                switch (*name++) {
                    case '\0':
                        if ((0 < count) && (count <= _VLIBC_IONAME_MAX)) {
                            break;
                        } else {
                            errno = ENOEXEC;
                            return EOF;
                        }

                    /*!< output io device */
                    case '<':
                        if (iomode & _VLIBC_IO_READ) {
                            errno = EINVAL;
                            return EOF;
                        }

                        if ((0 < count) && (count <= _VLIBC_IONAME_MAX)) {
                            count = 0;
                            /*!< have a output io device */
                            iomode |= _VLIBC_IO_HAVE_WRITE;
                            continue;
                        } else {
                            errno = ENOEXEC;
                            return EOF;
                        }

                    /*!<  input io device */
                    case '>':
                        if (iomode & _VLIBC_IO_WRITE) {
                            errno = EINVAL;
                            return EOF;
                        }

                        if (iomode & _VLIBC_IO_HAVE_READ) {
                            /*!< only can have one input io device */
                            errno = EINVAL;
                            return EOF;
                        }

                        if ((0 < count) && (count <= _VLIBC_IONAME_MAX)) {
                            count = 0;
                            /*!< have a input io device */
                            iomode |= _VLIBC_IO_HAVE_READ;
                            continue;
                        } else {
                            errno = ENOEXEC;
                            return EOF;
                        }

                    default:
                        count++;
                        continue;
                }
                break;
            }

            if (iomode & (_VLIBC_IO_HAVE_READ | _VLIBC_IO_HAVE_WRITE)) {
                return _VLIBC_TYPEIS_IO;
            }

            return _VLIBC_TYPEIS_FILE;
        }
        default:
            return _VLIBC_TYPEIS_FILE;
    }
}

static VLIBC_FILE *__io_vlibc_open(VLIBC_FILE *fnew, const char *name, unsigned char iomode)
{
    int result = 0;
    char namebuf[_VLIBC_IONAME_MAX + 2];
    char *nameend = NULL;
    char nofree = 0;

    if (fnew == NULL) {
        fnew = (VLIBC_FILE *)vlibc_malloc(sizeof(VLIBC_FILE));
        if (fnew == NULL) {
            errno = ENOMEM;
            return NULL;
        }
    } else {
        nofree = 1;
    }

    fnew->io = (void *)vlibc_malloc(sizeof(struct __vlibc_io));
    if (fnew == NULL) {
        errno = ENOMEM;
        if (nofree == 0) {
            vlibc_free(fnew);
        }
        return NULL;
    }

    fnew->io->bg = (void *)vlibc_malloc(VLIBC_BUFSIZ);
    if (fnew->io->bg == NULL) {
        vlibc_free(fnew->io);
        errno = ENOMEM;
        if (nofree == 0) {
            vlibc_free(fnew);
        }
        return NULL;
    }

    fnew->magic = _VLIBC_IO_MAGIC_CODE;

    fnew->io->dev = 0;
    fnew->io->flag = (iomode & (_VLIBC_IO_WRITE | _VLIBC_IO_READ));
    fnew->io->vbuf = _IOLBF;
    fnew->io->abuf = _VLIBC_ABUF_ENABLE;
    fnew->io->err = 0;

    fnew->io->wp = fnew->io->bg;
    fnew->io->rp = fnew->io->bg;
    fnew->io->ed = fnew->io->bg + VLIBC_BUFSIZ;

    /*!< init io device */
    for (int i = 0; i < VLIBC_FILENAME_MAX; i++) {
        switch (*name) {
            case '\0':
                break;

            case '<':
            case '>': {
                for (; i < VLIBC_FILENAME_MAX; i++) {
                    switch (*name++) {
                        case '\0':
                            name--;
                            break;
                        case '>':
                        case '<':
                            strncpy(namebuf, name, _VLIBC_IONAME_MAX + 1);
                            nameend = strchr(namebuf, '<');
                            if (nameend != NULL) {
                                *nameend = '\0';
                            } else {
                                nameend = strchr(namebuf, '>');
                                if (nameend != NULL) {
                                    *nameend = '\0';
                                }
                            }

                            result = __vlibc_io_init(namebuf, iomode & (_VLIBC_IO_READ | _VLIBC_IO_WRITE));
                            fnew->io->dev = result;
                            if (result == 0) {
                                vlibc_free(fnew->io->bg);
                                vlibc_free(fnew->io);
                                fnew->magic = 0;
                                if (nofree == 0) {
                                    vlibc_free(fnew);
                                }
                                errno = ENOENT;
                                return NULL;
                            }
                            break;

                        default:
                            continue;
                    }

                    break;
                }
                continue;
            }

            default:
                name++;
                continue;
        }
        break;
    }

    return fnew;
}

#ifdef CONFIG_VLIBC_FATFS
static VLIBC_FILE *__file_vlibc_open(VLIBC_FILE *fnew, const char *name, unsigned char openmode)
{
    int fresult;
    char nofree = 0;

    if (fnew == NULL) {
        fnew = (VLIBC_FILE *)vlibc_malloc(sizeof(VLIBC_FILE));
        if (fnew == NULL) {
            errno = ENOMEM;
            return NULL;
        }
    } else {
        nofree = 1;
    }

    fnew->file = (FIL *)vlibc_malloc(sizeof(FIL));
    if (fnew->file == NULL) {
        if (nofree == 0) {
            vlibc_free(fnew);
        }
        errno = ENOMEM;
        return NULL;
    }

    fnew->magic = _VLIBC_FILE_MAGIC_CODE;

    fresult = f_open(fnew->file, name, openmode);

    if (fresult != FR_OK) {
        if (fresult <= FR_INVALID_PARAMETER) {
            errno = FRESULT_to_errno[fresult];
        }

        vlibc_free(fnew->file);
        fnew->magic = 0;
        if (nofree == 0) {
            vlibc_free(fnew);
        }
        return NULL;
    }

    return fnew;
}
#else
static VLIBC_FILE *__file_vlibc_open(VLIBC_FILE *fnew, const char *name, unsigned char openmode)
{
    errno = EIO;
    return NULL;
}
#endif

static int __io_vlibc_close(VLIBC_FILE *stream)
{
    int result;

    if (stream->io->abuf == _VLIBC_ABUF_ENABLE) {
        if (stream->io->bg == NULL) {
            return EOF;
        }
    }

    result = __vlibc_io_deinit(stream->io);
    if (result != stream->io->dev) {
        errno = EINVAL;
        return EOF;
    }

    if (stream->io->abuf == _VLIBC_ABUF_ENABLE) {
        vlibc_free(stream->io->bg);
    }

    vlibc_free(stream->io);
    stream->magic = 0;
    stream->io = NULL;
    return 0;
}

#ifdef CONFIG_VLIBC_FATFS
static int __file_vlibc_close(VLIBC_FILE *stream)
{
    int fresult;

    fresult = f_close(stream->file);
    if (fresult != FR_OK) {
        if (fresult <= FR_INVALID_PARAMETER) {
            errno = FRESULT_to_errno[fresult];
        }
        return EOF;
    }

    vlibc_free(stream->file);
    stream->magic = 0;
    stream->io = NULL;
    return 0;
}
#else
static int __file_vlibc_close(VLIBC_FILE *stream)
{
    errno = EIO;
    return EOF;
}
#endif

/**
 *   @brief  
 *   @param  filename
 *   @param  mode
 *   @return VLIBC_FILE*
 */
VLIBC_FILE *vlibc_fopen(const char *filename, const char *mode)
{
    CHECK_FILE(filename, NULL);
    CHECK_FILE(mode, NULL);

    unsigned char iomode = 0;
    unsigned char openmode = 0;

    if (__get_mode_vlibc_fopen(mode, &iomode, &openmode)) {
        return NULL;
    }

    switch (__check_type_vlibc_fopen(filename, iomode)) {
        case EOF:
            return NULL;
        case _VLIBC_TYPEIS_IO:
            return __io_vlibc_open(NULL, filename, iomode);
        case _VLIBC_TYPEIS_FILE:
            return __file_vlibc_open(NULL, filename, openmode);
        default:
            return NULL;
    }
}

/**
 *   @brief  重定向输入输出流.
 *           fclose 之后的文件指针无法再 freopen.
 *           freopen 会关闭原先的流, 并按照 <filename> 和 <mode> 直接在原先的流上重建实体.
 *           freopen 会将两个流合并为一个流, 可以多次执行将多个流合并,
 *           只需要一次 fclose 就可以关闭所有合并的流
 *   @param  filename               
 *   @param  mode                   
 *   @param  stream                 
 *   @return VLIBC_FILE* 
 */
VLIBC_FILE *vlibc_freopen(const char *filename, const char *mode, VLIBC_FILE *stream)
{
    CHECK_FILE(filename, NULL);
    CHECK_FILE(mode, NULL);
    CHECK_FILE(stream, NULL);
    CHECK_FILE(stream->file, NULL);

    VLIBC_FILE *fnew = NULL;

    unsigned char iomode = 0;
    unsigned char openmode = 0;

    if (__get_mode_vlibc_fopen(mode, &iomode, &openmode)) {
        return NULL;
    }

    switch (__check_type_vlibc_fopen(filename, iomode)) {
        case EOF:
            return NULL;

        case _VLIBC_TYPEIS_IO:
            IF_IO_DEV(stream)
            {
                if (__io_vlibc_close(stream)) {
                    return NULL;
                }

                fnew = __io_vlibc_open(stream, filename, iomode);
                if (fnew == NULL) {
                    return NULL;
                }
                return fnew;
            }
            else IF_FILE(stream)
            {
#ifdef CONFIG_VLIBC_FATFS
                if (__file_vlibc_close(stream)) {
                    return NULL;
                }

                fnew = __io_vlibc_open(stream, filename, iomode);
                if (fnew == NULL) {
                    return NULL;
                }

                return fnew;
#else
                return NULL;
#endif
            }
            else
            {
                errno = EBADF;
                return NULL;
            }

        case _VLIBC_TYPEIS_FILE:
            IF_IO_DEV(stream)
            {
                if (__io_vlibc_close(stream)) {
                    return NULL;
                }

                fnew = __file_vlibc_open(stream, filename, openmode);
                if (fnew == NULL) {
                    return NULL;
                }

                return fnew;
            }
            else IF_FILE(stream)
            {
#ifdef CONFIG_VLIBC_FATFS
                if (__file_vlibc_close(stream)) {
                    return NULL;
                }

                fnew = __file_vlibc_open(stream, filename, openmode);
                if (fnew == NULL) {
                    return NULL;
                }

                return fnew;
#else
                return NULL;
#endif
            }
            else
            {
                errno = EBADF;
                return NULL;
            }
        default:
            return NULL;
    }
}

/**
 *   @brief         
 *   @param  stream                 
 *   @return int 
 */
int vlibc_fclose(VLIBC_FILE *stream)
{
    CHECK_FILE(stream, EOF);
    CHECK_FILE(stream->file, EOF);

    IF_IO_DEV(stream)
    {
        if (vlibc_fflush(stream)) {
            return EOF;
        }

        if (__io_vlibc_close(stream)) {
            return EOF;
        }

        vlibc_free(stream);

        return 0;
    }
    else IF_FILE(stream)
    {
        if (vlibc_fflush(stream)) {
            return EOF;
        }

        if (__file_vlibc_close(stream)) {
            return EOF;
        }

        vlibc_free(stream);

        return 0;
    }
    else
    {
        errno = EBADF;
        return EOF;
    }
}

/**
 *   @brief         
 *   @param  ptr                    
 *   @param  size                   
 *   @param  nmemb                  
 *   @param  stream                 
 *   @return size_t 
 */
size_t vlibc_fread(void *ptr, size_t size, size_t nmemb, VLIBC_FILE *stream)
{
    CHECK_FILE(ptr, 0);
    CHECK_FILE(size, 0);
    CHECK_FILE(nmemb, 0);
    CHECK_FILE(stream, 0);

    IF_IO_DEV(stream)
    {
        size_t bytes = size * nmemb;
        char *bg = stream->io->bg;
        char *wp = stream->io->wp;
        char *rp = stream->io->rp;
        char *ed = stream->io->ed;

        do {
            if (rp == wp) {
                break;
            }

            if (rp == ed) {
                rp = bg;
            }

            *(char *)ptr++ = *rp++;
        } while (--bytes);

        return size * nmemb - bytes;
    }
    else IF_FILE(stream)
    {
#ifdef CONFIG_VLIBC_FATFS
        int fresult;
        size_t bytes;
        fresult = f_read(stream->file, ptr, size * nmemb, &bytes);
        if (fresult != FR_OK) {
            if (fresult <= FR_INVALID_PARAMETER) {
                errno = FRESULT_to_errno[fresult];
            }
        }

        return bytes;
#else
        return 0;
#endif
    }
    else
    {
        errno = EBADF;
        return 0;
    }
}

/**
 *   @brief         
 *   @param  ptr                    
 *   @param  size                   
 *   @param  nmemb                  
 *   @param  stream                 
 *   @return size_t 
 */
size_t vlibc_fwrite(const void *ptr, size_t size, size_t nmemb, VLIBC_FILE *stream)
{
    CHECK_FILE(ptr, 0);
    CHECK_FILE(size, 0);
    CHECK_FILE(nmemb, 0);
    CHECK_FILE(stream, 0);

    IF_IO_DEV(stream)
    {
        switch (stream->io->vbuf) {
            case _IOFBF: {
                size_t ret;
                size_t bytes = size * nmemb;
                char *bg = stream->io->bg;
                char *wp = stream->io->wp;
                char *ed = stream->io->ed;

                do {
                    if (wp == ed) {
                        ret = __vlibc_io_mem2dev(stream->io, bg, ed - bg);
                        if (ret == 0) {
                            stream->io->wp = wp;
                            errno = EINVAL;
                            return size * nmemb - bytes;
                        }
                        wp = bg;
                    }

                    *wp++ = *(char *)ptr++;
                } while (--bytes);

                if (wp == ed) {
                    ret = __vlibc_io_mem2dev(stream->io, bg, ed - bg);
                    if (ret == 0) {
                        stream->io->wp = wp;
                        errno = EINVAL;
                        return size * nmemb - bytes;
                    }
                    wp = bg;
                }

                stream->io->wp = wp;
                return size * nmemb - bytes;
            }
            case _IOLBF: {
                size_t ret;
                size_t bytes = size * nmemb;
                char *bg = stream->io->bg;
                char *wp = stream->io->wp;
                char *ed = stream->io->ed;

                do {
                    if (wp == ed) {
                        ret = __vlibc_io_mem2dev(stream->io, bg, ed - bg);
                        if (ret == 0) {
                            stream->io->wp = wp;
                            errno = EINVAL;
                            return size * nmemb - bytes;
                        }
                        wp = bg;
                    }

                    *wp = *(char *)ptr++;

                    if (*wp == '\n') {
                        wp++;
                        --bytes;
                        ret = __vlibc_io_mem2dev(stream->io, bg, wp - bg);
                        if (ret == 0) {
                            stream->io->wp = wp;
                            errno = EINVAL;
                            return size * nmemb - bytes;
                        }

                        wp = bg;
                    } else {
                        wp++;
                        --bytes;
                    }
                } while (bytes);

                stream->io->wp = wp;

                return size * nmemb - bytes;
            }
            case _IONBF: {
                size_t bytes;
                bytes = __vlibc_io_mem2dev(stream->io, ptr, size * nmemb);
                if (bytes == 0) {
                    errno = EINVAL;
                }
                return bytes;
            }
            default:
                errno = EINVAL;
                return 0;
        }
    }
    else IF_FILE(stream)
    {
#ifdef CONFIG_VLIBC_FATFS
        int fresult;
        size_t bytes;
        fresult = f_write(stream->file, ptr, size * nmemb, &bytes);
        if (fresult != FR_OK) {
            if (fresult <= FR_INVALID_PARAMETER) {
                errno = FRESULT_to_errno[fresult];
            }
        }

        return bytes;
#else
        errno = EIO;
        return 0;
#endif
    }
    else
    {
        errno = EBADF;
        return 0;
    }
}

/**
 *   @brief         
 *   @param  stream                 
 *   @return int 
 */
int vlibc_fflush(VLIBC_FILE *stream)
{
    CHECK_FILE(stream, EOF);
    CHECK_FILE(stream->file, EOF);

    IF_IO_DEV(stream)
    {
        switch (stream->io->vbuf) {
            case _IOFBF:
            case _IOLBF: {
                size_t ret;
                ret = (size_t)(stream->io->wp - stream->io->bg);
                if (ret > 0) {
                    ret = __vlibc_io_mem2dev(stream->io, stream->io->bg, ret);
                    if (ret == 0) {
                        errno = EINVAL;
                        return EOF;
                    }
                    stream->io->wp = stream->io->bg;
                }
            }
                return 0;

            case _IONBF:
                return 0;

            default:
                errno = EINVAL;
                return EOF;
        }
    }
    else IF_FILE(stream)
    {
#ifdef CONFIG_VLIBC_FATFS
        int fresult;
        fresult = f_sync(stream->file);
        if (fresult != FR_OK) {
            if (fresult <= FR_INVALID_PARAMETER) {
                errno = FRESULT_to_errno[fresult];
            }
            return EOF;
        }

        return 0;
#else
        errno = EIO;
        return EOF;
#endif
    }
    else
    {
        errno = EBADF;
        return EOF;
    }
}

/**
 *   @brief         
 *   @param  stream                 
 *   @param  offset                 
 *   @param  whence                 
 *   @return int 
 */
int vlibc_fseek(VLIBC_FILE *stream, long offset, int whence)
{
    CHECK_FILE(stream, EOF);
    CHECK_FILE(stream->file, EOF);

    IF_IO_DEV(stream)
    {
        errno = ESPIPE;
        return EOF;
    }
    else IF_FILE(stream)
    {
#ifdef CONFIG_VLIBC_FATFS
        int fresult;
        long long temp;

        switch (whence) {
            case SEEK_SET:
                fresult = f_lseek(stream->file, offset);
                if (fresult != FR_OK) {
                    if (fresult <= FR_INVALID_PARAMETER) {
                        errno = FRESULT_to_errno[fresult];
                    }
                    return EOF;
                }
                return 0;

            case SEEK_CUR:
                temp = (long)stream->file->fptr;
                temp += offset;
                if (temp > 0xffffffff) {
                    errno = ESPIPE;
                    return EOF;
                }

                fresult = f_lseek(stream->file, temp);
                if (fresult != FR_OK) {
                    if (fresult <= FR_INVALID_PARAMETER) {
                        errno = FRESULT_to_errno[fresult];
                    }
                    return EOF;
                }
                return 0;

            case SEEK_END:
                temp = (long)stream->file->obj.objsize;
                temp -= offset;
                if (temp <= 0) {
                    temp = 0;
                }
                fresult = f_lseek(stream->file, temp);
                if (fresult != FR_OK) {
                    if (fresult <= FR_INVALID_PARAMETER) {
                        errno = FRESULT_to_errno[fresult];
                    }
                    return EOF;
                }
                return 0;
            default:
                errno = EINVAL;
                return EOF;
        }
#else
        errno = EIO;
        return EOF;
#endif
    }
    else
    {
        errno = EBADF;
        return EOF;
    }
}

/**
 *   @brief         
 *   @param  stream                 
 *   @return long 
 */
long vlibc_ftell(VLIBC_FILE *stream)
{
    CHECK_FILE(stream, EOF);
    CHECK_FILE(stream->file, EOF);

    IF_IO_DEV(stream)
    {
        errno = ESPIPE;
        return EOF;
    }
    else IF_FILE(stream)
    {
#ifdef CONFIG_VLIBC_FATFS
        return (long)f_tell(stream->file);
#else
        errno = EIO;
        return EOF;
#endif
    }
    else
    {
        errno = EBADF;
        return EOF;
    }
}

/**
 *   @brief         
 *   @param  filename               
 *   @return int 
 */
int vlibc_remove(const char *filename)
{
    CHECK_FILE(filename, EOF);

#ifdef CONFIG_VLIBC_FATFS
    int fresult;
    FILINFO finfo;

    fresult = f_stat(filename, &finfo);
    if (fresult != FR_OK) {
        if (fresult <= FR_INVALID_PARAMETER) {
            errno = FRESULT_to_errno[fresult];
            return EOF;
        }
    }

    fresult = f_unlink(filename);
    if (fresult != FR_OK) {
        if (fresult <= FR_INVALID_PARAMETER) {
            errno = FRESULT_to_errno[fresult];
            return EOF;
        }
    }

    return 0;
#else
    errno = ENOENT;
    return EOF;
#endif
}

/**
 *   @brief         
 *   @param  old_filename           
 *   @param  new_filename           
 *   @return int 
 */
int vlibc_rename(const char *old_filename, const char *new_filename)
{
    CHECK_FILE(old_filename, EOF);
    CHECK_FILE(new_filename, EOF);
#ifdef CONFIG_VLIBC_FATFS
    int fresult;

    fresult = f_rename(old_filename, new_filename);
    if (fresult != FR_OK) {
        if (fresult <= FR_INVALID_PARAMETER) {
            errno = FRESULT_to_errno[fresult];
            return EOF;
        }
    }

    return 0;
#else
    errno = ENOENT;
    return EOF;
#endif
}

/**
 *   @brief         
 *   @param  stream                 
 */
void vlibc_rewind(VLIBC_FILE *stream)
{
    CHECK_FILE(stream, /*!< no return */);
    CHECK_FILE(stream->file, /*!< no return */);

    IF_IO_DEV(stream)
    {
        errno = ESPIPE;
        return;
    }
    else IF_FILE(stream)
    {
#ifdef CONFIG_VLIBC_FATFS
        int fresult;
        fresult = f_lseek(stream->file, 0);
        if (fresult != FR_OK) {
            if (fresult <= FR_INVALID_PARAMETER) {
                errno = FRESULT_to_errno[fresult];
            }
        }
#else
        errno = EIO;
#endif
    }
    else
    {
        errno = EBADF;
        return;
    }
}

/**
 *   @brief         
 *   @param  stream                 
 *   @param  buffer                 
 */
void vlibc_setbuf(VLIBC_FILE *stream, char *buffer)
{
    CHECK_FILE(stream, /*!< no return */);

    IF_IO_DEV(stream)
    {
        if (stream->io->abuf == _VLIBC_ABUF_ENABLE) {
            if (stream->io->bg == NULL) {
                return;
            }

            vlibc_free(stream->io->bg);
            stream->io->abuf = _VLIBC_ABUF_DISABLE;
        }

        stream->io->bg = buffer;
        stream->io->wp = buffer;
        stream->io->rp = buffer;

        if (buffer == NULL) {
            stream->io->ed = buffer;
        } else {
            stream->io->ed = buffer + VLIBC_BUFSIZ;
        }
    }
    else IF_FILE(stream)
    {
#ifdef CONFIG_VLIBC_FATFS
        return;
#else
        errno = EIO;
        return;
#endif
    }
    else
    {
        errno = EBADF;
        return;
    }
}

/**
 *   @brief         
 *   @param  stream                 
 *   @param  buffer                 
 *   @param  mode                   
 *   @param  size                   
 *   @return int 
 */
int vlibc_setvbuf(VLIBC_FILE *stream, char *buffer, int mode, size_t size)
{
    CHECK_FILE(stream, EOF);

    IF_IO_DEV(stream)
    {
        switch (mode) {
            case _IOFBF:
            case _IOLBF:
                vlibc_setbuf(stream, buffer);
                if (buffer != NULL) {
                    stream->io->ed = buffer + size;
                }
                stream->io->vbuf = mode;
                break;
            case _IONBF:
                vlibc_setbuf(stream, NULL);
                stream->io->vbuf = mode;
                break;
            default:
                return EOF;
        }
        return 0;
    }
    else IF_FILE(stream)
    {
#ifdef CONFIG_VLIBC_FATFS
        return EOF;
#else
        errno = EIO;
        return EOF;
#endif
    }
    else
    {
        errno = EBADF;
        return EOF;
    }
}

VLIBC_FILE *vlibc_tmpfile(void)
{
#ifdef CONFIG_VLIBC_FATFS
    return NULL;
#else
    return NULL;
#endif
}

char *vlibc_tmpnam(char *str)
{
#ifdef CONFIG_VLIBC_FATFS
    return NULL;
#else
    return NULL;
#endif
}

int vlibc_fscanf(VLIBC_FILE *stream, const char *format, ...)
{
    CHECK_FILE(stream, EOF);
    CHECK_FILE(format, EOF);
#ifdef CONFIG_VLIBC_FATFS
    return EOF;
#else
    return EOF;
#endif
}

int vlibc_scanf(const char *format, ...)
{
    CHECK_FILE(format, EOF);
#ifdef CONFIG_VLIBC_FATFS
    return EOF;
#else
    return EOF;
#endif
}

int vlibc_sscanf(const char *str, const char *format, ...)
{
    CHECK_FILE(str, EOF);
    CHECK_FILE(format, EOF);
#ifdef CONFIG_VLIBC_FATFS
    return EOF;
#else
    return EOF;
#endif
}

int vlibc_vfscanf(VLIBC_FILE *stream, const char *format, va_list arg)
{
    CHECK_FILE(stream, EOF);
    CHECK_FILE(format, EOF);
#ifdef CONFIG_VLIBC_FATFS
    return EOF;
#else
    return EOF;
#endif
}

int vlibc_vscanf(const char *format, va_list arg)
{
    CHECK_FILE(format, EOF);
#ifdef CONFIG_VLIBC_FATFS
    return EOF;
#else
    return EOF;
#endif
}

int vlibc_vsscanf(const char *str, const char *format, va_list arg)
{
    CHECK_FILE(str, EOF);
    CHECK_FILE(format, EOF);
#ifdef CONFIG_VLIBC_FATFS
    return EOF;
#else
    return EOF;
#endif
}

int vlibc_fgetc(VLIBC_FILE *stream)
{
    CHECK_FILE(stream, EOF);
    IF_IO_DEV(stream)
    {
        return EOF;
    }
    else IF_FILE(stream)
    {
#ifdef CONFIG_VLIBC_FATFS
        int fresult;
        int ch;
        size_t bytes;
        fresult = f_read(stream->file, &ch, 1, &bytes);
        if (fresult != FR_OK) {
            if (fresult <= FR_INVALID_PARAMETER) {
                errno = FRESULT_to_errno[fresult];
            }

            return EOF;
        }
        if (bytes != 1) {
            return EOF;
        }

        return ch;
#else

        return EOF;
#endif
    }
    else
    {
        return EOF;
    }
}

char *vlibc_fgets(char *str, int size, VLIBC_FILE *stream)
{
    CHECK_FILE(str, NULL);
    CHECK_FILE(stream, NULL);
#ifdef CONFIG_VLIBC_FATFS
    return NULL;
#else
    return NULL;
#endif
}

int vlibc_fputc(int chr, VLIBC_FILE *stream)
{
    CHECK_FILE(stream, EOF);

    IF_IO_DEV(stream)
    {
        return EOF;
    }
    else IF_FILE(stream)
    {
#ifdef CONFIG_VLIBC_FATFS
        int fresult;
        size_t bytes;
        fresult = f_write(stream->file, &chr, 1, &bytes);
        if (fresult != FR_OK) {
            if (fresult <= FR_INVALID_PARAMETER) {
                errno = FRESULT_to_errno[fresult];
            }

            return EOF;
        }
        if (bytes != 1) {
            return EOF;
        }

        return 0;
#else

        return EOF;
#endif
    }
    else
    {
        return EOF;
    }
}

int vlibc_fputs(const char *str, VLIBC_FILE *stream)
{
    return vlibc_fprintf(stream, str);
}

int vlibc_getc(VLIBC_FILE *stream)
{
    CHECK_FILE(stream, EOF);
    IF_IO_DEV(stream)
    {
        return EOF;
    }
    else IF_FILE(stream)
    {
#ifdef CONFIG_VLIBC_FATFS
        int fresult;
        int ch;
        size_t bytes;
        fresult = f_read(stream->file, &ch, 1, &bytes);
        if (fresult != FR_OK) {
            if (fresult <= FR_INVALID_PARAMETER) {
                errno = FRESULT_to_errno[fresult];
            }

            return EOF;
        }
        if (bytes != 1) {
            return EOF;
        }

        return ch;
#else

        return EOF;
#endif
    }
    else
    {
        return EOF;
    }
}

int vlibc_getchar(void)
{
#ifdef CONFIG_VLIBC_FATFS
    return EOF;
#else
    return EOF;
#endif
}

char *vlibc_gets(char *str)
{
    CHECK_FILE(str, NULL);
#ifdef CONFIG_VLIBC_FATFS
    return NULL;
#else
    return NULL;
#endif
}

int vlibc_putc(int chr, VLIBC_FILE *stream)
{
    CHECK_FILE(stream, EOF);

    IF_IO_DEV(stream)
    {
        size_t size;
        size = vlibc_fwrite(&chr, 1, 1, stream);
        if (size == 1) {
            return 0;
        } else {
            return EOF;
        }
    }
    else IF_FILE(stream)
    {
#ifdef CONFIG_VLIBC_FATFS
        int fresult;
        size_t bytes;
        fresult = f_write(stream->file, &chr, 1, &bytes);
        if (fresult != FR_OK) {
            if (fresult <= FR_INVALID_PARAMETER) {
                errno = FRESULT_to_errno[fresult];
            }

            return EOF;
        }
        if (bytes != 1) {
            return EOF;
        }

        return 0;
#else

        return EOF;
#endif
    }
    else
    {
        return EOF;
    }
}

int vlibc_putchar(int chr)
{
    IF_IO_DEV(vlibc_stdout)
    {
        size_t size;
        size = vlibc_fwrite(&chr, 1, 1, vlibc_stdout);
        if (size == 1) {
            return 0;
        } else {
            return EOF;
        }
    }
    else IF_FILE(vlibc_stdout)
    {
#ifdef CONFIG_VLIBC_FATFS
        int fresult;
        size_t bytes;
        fresult = f_write(vlibc_stdout->file, &chr, 1, &bytes);
        if (fresult != FR_OK) {
            if (fresult <= FR_INVALID_PARAMETER) {
                errno = FRESULT_to_errno[fresult];
            }

            return EOF;
        }
        if (bytes != 1) {
            return EOF;
        }

        return 0;
#else

        return EOF;
#endif
    }
    else
    {
        return EOF;
    }
}

int vlibc_puts(const char *str)
{
    return vlibc_printf(str);
}

void vlibc_perror(const char *str)
{
    CHECK_FILE(str, /*!< no return */);
}
