/*
** $Id: luaport.h $
** Lua Port
** See Copyright Notice in lua.h
*/

#ifndef luaport_h
#define luaport_h

#define LUA_USE_MCU
#define LUA_USE_APICHECK
// #define LUAI_ASSERT

#include <assert.h>
#define luaport_assert assert

#include <stdarg.h>
#define luaport_offsetof offsetof

#include <stdio.h>
#define luaport_stdin    stdin
#define luaport_stdout   stdout
#define luaport_stderr   stderr
#define luaport_tmpfile  tmpfile
#define luaport_tmpnam   tmpnam
#define luaport_fclose   fclose
#define luaport_fflush   fflush
#define luaport_freopen  freopen
#define luaport_setvbuf  setvbuf
#define luaport_fprintf  fprintf
#define luaport_printf   printf
#define luaport_fgets    fgets
#define luaport_fputs    fputs
#define luaport_getc     getc
#define luaport_ungetc   ungetc
#define luaport_fread    fread
#define luaport_fwrite   fwrite
#define luaport_fseek    fseek
#define luaport_ftell    ftell
#define luaport_clearerr clearerr
#define luaport_feof     feof
#define luaport_ferror   ferror
#define luaport_fopen    fopen
#define luaport_remove   remove
#define luaport_rename   rename
#define luaport_snprintf snprintf

#include <string.h>
#define luaport_memchr   memchr
#define luaport_memcmp   memcmp
#define luaport_memcpy   memcpy
#define luaport_strchr   strchr
#define luaport_strcmp   strcmp
#define luaport_strcoll  strcoll
#define luaport_strlcpy  strlcpy
#define luaport_strerror strerror
#define luaport_strlen   strlen
#define luaport_strncmp  strncmp
#define luaport_strpbrk  strpbrk
#define luaport_strrchr  strrchr
#define luaport_strspn   strspn
#define luaport_strstr   strstr

#include <errno.h>
#define luaport_errno errno

#include <stdlib.h>
#define luaport_abort   abort
#define luaport_exit    exit
#define luaport_free    free
#define luaport_getenv  getenv
#define luaport_realloc realloc
#define luaport_strtod  strtod
#define luaport_strtof  strtof
#define luaport_system  system

#include <setjmp.h>
#define luaport_longjmp longjmp
#define luaport_setjmp  setjmp

#include <locale.h>
#define luaport_setlocale  setlocale
#define luaport_localeconv localeconv

#include <time.h>
#define luaport_time_t    long long
#define luaport_clock     clock
#define luaport_difftime  difftime
#define luaport_mktime    mktime
#define luaport_time      time
#define luaport_gmtime    gmtime
#define luaport_localtime localtime
#define luaport_strftime  strftime

#define luaport_signal(sig,func) NULL

#endif