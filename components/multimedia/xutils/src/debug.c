/*
 * Copyright (C) 2017-2022 Bouffalolab Group Holding Limited
 * Change Logs:
 *   Date        Author       Notes
 *   2019-03-25  MSP          the first version
 */

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include <msp/kernel.h>
#include <xutils/debug.h>

int print_driver(const char *fmt, va_list ap, unsigned int buf[]);
extern void vprint(const char *fmt, va_list argp);

#if MSP_DEBUG_COLOR
const char *color_d = "\x1b[36mmspd\x1b[0m";
const char *color_i = "\x1b[32mmspi\x1b[0m";
const char *color_w = "\x1b[33mmspw\x1b[0m";
const char *color_e = "\x1b[31mmspe\x1b[0m";
const char *color_a = "\x1b[35mmspa\x1b[0m";
#endif

static void except_process_function(int err, const char *file, int line, const char *func_name, void *caller)
{
#if defined(CONFIG_MSP_DEBUG) && CONFIG_MSP_DEBUG
    printf("Except! errno is %s, function: %s at %s:%d, caller: %p\n", strerror(err), func_name, file, line, caller);
    while(1);
#else
    printf("Except! errno is %s, caller: %p\n", strerror(err), caller);
    // TODO
    // GLB_SW_POR_Reset();
#endif
}

static except_process_t process = except_process_function;

void msp_set_except_callback(except_process_t except)
{
    process = except;
}

void msp_set_except_default()
{
    process = except_process_function;
}

void msp_except_process(int err, const char *file, int line, const char *func_name, void *caller)
{
    if (process)
    {
        process(err, file, line, func_name, caller);
    }
}

/**
 * This function is print flash debug info.
 *
 * @param file the file which has call this function
 * @param line the line number which has call this function
 * @param format output format
 * @param ... args
 *
 */
void msp_debug(const char *tag, const char *filename, const char *funcname, const long line, const char *format, ...)
{
#ifdef CONFIG_CHIP_BL606P_E907
    va_list args;
    char *color_tag = (char *)tag;

    if ((tag[3] == 'a')
        || (tag[3] == 'e')
        || (tag[3] == 'w')
        || (tag[3] == 'i')
        // || (tag[3] == 'd')
        ) {

        /* args point to the first variable parameter */
        va_start(args, format);

        /* You can add your code under here. */
        //vprint(format, args);
#if MSP_DEBUG_COLOR
        if (tag[3] == 'a') {
            color_tag = (char *)color_a;
        } else if (tag[3] == 'e') {
            color_tag = (char *)color_e;
        } else if (tag[3] == 'w') {
            color_tag = (char *)color_w;
        } else if (tag[3] == 'i') {
            color_tag = (char *)color_i;
        } else if (tag[3] == 'd') {
            color_tag = (char *)color_d;
        }
#endif
        printf("[%lld][%s] %s %s %ld :", msp_now_ms(), color_tag, filename, funcname, line);
        print_driver(format, args, NULL);

        va_end(args);
    }
#else
    //printf("[%lld][%s] %s %s %ld :", msp_now_ms(), color_tag, filename, funcname, line);
#endif
}

