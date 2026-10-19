__attribute__((import_module("env"), import_name("puts")))
extern int host_puts(const char *text);

int main(void)
{
    host_puts("Hello World from WAMR!");
    return 0;
}
