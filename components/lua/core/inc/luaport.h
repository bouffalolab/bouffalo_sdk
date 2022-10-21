/*
** $Id: luaport.h $
** Lua Port
** See Copyright Notice in lua.h
*/

#ifndef luaport_h
#define luaport_h

#include <time.h>

#define LUA_USE_APICHECK

/*!< get system now clock ticks, type unsigned int */
extern clock_t luaport_clock(void);

/*!< get utc second */
extern time_t luaport_time(time_t *);

/*!< abort lua */
extern void __attribute__((noreturn)) luaport_abort(void);

/*!< exit lua */
extern void luaport_exit(int status);

/*!< env get */
extern char *luaport_getenv(const char *name);

/*!< can't be zero */
#define LUAPORT_CLOCKS_PER_SEC 1000000

/*!< interpret 'pclose' return status */
#define luaport_inspectstat(stat, what) /*!< no op */

/*!< lua port assert */
#if defined(LUA_USE_APICHECK)
#define luaport_assert(x) ((void)0)
#endif

/*!< stdio.h */
#include "vlibc_stdio.h"

#define LUAPORT_FILEHANDLE "FILE*"
#define LUAPORT_FILE       vlibc_file_t

#define luaport_stdin  vlibc_stdin
#define luaport_stdout vlibc_stdout
#define luaport_stderr vlibc_stderr

#define luaport_fclose   vlibc_fclose
#define luaport_clearerr vlibc_clearerr
#define luaport_feof     vlibc_feof
#define luaport_ferror   vlibc_ferror
#define luaport_fflush   vlibc_fflush
#define luaport_fopen    vlibc_fopen
#define luaport_fread    vlibc_fread
#define luaport_fseek    vlibc_fseek
#define luaport_ftell    vlibc_ftell
#define luaport_fwirte   vlibc_fwrite
#define luaport_remove   vlibc_remove
#define luaport_rename   vlibc_rename
#define luaport_setvbuf  vlibc_setvbuf
#define luaport_tmpfile  vlibc_tmpfile
#define luaport_tmpnam   vlibc_tmpnam
#define luaport_fprintf  vlibc_fprintf
#define luaport_fgets    vlibc_fgets
#define luaport_getc     vlibc_getc
#define luaport_snprintf vlibc_snprintf
#define luaport_sprintf  vlibc_sprintf
#define luaport_ungetc(_c, _f)

#include "stdlib.h"
#define luaport_free    free
#define luaport_realloc realloc

/*!< stdlib.h */
#include <stdlib.h>
#define luaport_system system

/*!< string.h */
#include <string.h>
#define luaport_memcpy memcpy

#endif
