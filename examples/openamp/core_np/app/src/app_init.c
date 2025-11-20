#include <stdint.h>

extern void app_sample_init(void);

int app_init(void)
{
    app_sample_init();
    return 0;
}
