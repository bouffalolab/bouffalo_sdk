#ifndef CONFIG_FREERTOS
int *__errno()
{
    static int __errno_value = 0;
    return &__errno_value;
}
#endif
