__attribute__((import_module("env"), import_name("puts")))
extern int host_puts(const char *text);

__attribute__((export_name("main")))
int wasm_entry(void)
{
    host_puts("Hello World from WAMRsadsadad!");
    return 0;
}
