/*
 * C++ runtime support for the freestanding Bouffalo SDK image.
 *
 * The SDK deliberately links without the toolchain startup objects.  In a
 * hosted executable __dso_handle is normally supplied by crtbegin.o, but
 * that object is not present in an SDK image.  libstdc++ uses this handle
 * when registering C++ object destructors with __cxa_atexit.
 */

/*
 * Keep this definition weak so targets which deliberately remove
 * -nostartfiles can use the stronger definition from crtbegin.o.
 */
void *__dso_handle __attribute__((weak, visibility("hidden"))) = 0;

extern void __libc_init_array(void);

/*
 * Keep this translation unit in the final image even when the only C++ use
 * comes from a static library.  The link option is installed by the libc
 * CMakeLists.txt file below.
 */
void __cxx_runtime_dummy(void)
{
}

/*
 * Startup code calls the application entry point directly.  Wrapping that
 * symbol lets us run the C/C++ constructor array without changing every
 * chip's assembly startup file.  The extra startup arguments are harmless
 * for the SDK's usual main(void) functions and remain available to a
 * conventional main(int, char **).
 */
extern int __real_main(int argc, char **argv);

int __wrap_main(int argc, char **argv)
{
    __libc_init_array();
    return __real_main(argc, argv);
}
