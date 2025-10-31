#include "bflb_mtimer.h"
#include "board.h"
#include "bflb_common.h"

extern const bflb_verinf_t app_ver;

int main(void)
{
    uint8_t version = 0xFF;

    board_init();

    printf("anti_rollback case:\r\n");

    if(0 != bflb_get_app_version_from_efuse(&version)){
        printf("error! can't read app version\r\n");
        while(1){
        }
    }else{
        printf("app version in efuse is: %d\r\n", version);
    }

    if(app_ver.anti_rollback < version){
        printf("app version in application is: %d, less than app version in efuse, the application should not run up\r\n", app_ver.anti_rollback);
    }else{
        printf("app version in application is: %d, not less than app version in efuse, the application should run up\r\n", app_ver.anti_rollback);
    }

    /* change app version in efuse to 1 */
    bflb_set_app_version_to_efuse(app_ver.anti_rollback);

    if(0 != bflb_get_app_version_from_efuse(&version)){
        printf("error! can't read app version\r\n");
        while(1){
        }
    }else{
        printf("app version in efuse is: %d\r\n", version);
    }

    while (1) {
        bflb_mtimer_delay_ms(1000);
    }
}
