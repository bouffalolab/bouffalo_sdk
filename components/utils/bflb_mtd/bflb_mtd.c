#include "bflb_boot2.h"
#include "bflb_mtd.h"
#include "bflb_flash.h"

struct bflb_mtd_handle_priv {
    char name[16];
    int id;
    unsigned int offset;
    unsigned int size;
    void *xip_addr;
};
typedef struct bflb_mtd_handle_priv *bflb_mtd_handle_priv_t;

static void __dump_mtd_handle(bflb_mtd_handle_priv_t handle_prv)
{
    printf(  "[MTD] >>>>>> Hanlde info Dump >>>>>>\r\n");
    printf("      name %s\r\n", handle_prv->name);
    printf("      id %d\r\n", handle_prv->id);
    printf("      offset %p(%u)\r\n", (void*)handle_prv->offset, handle_prv->offset);
    printf("      size %p(%uKbytes)\r\n", (void*)handle_prv->size, handle_prv->size/1024);
    printf("      xip_addr %p\r\n", handle_prv->xip_addr);
    printf(  "[MTD] <<<<<< Hanlde info End <<<<<<\r\n");
}

static int _mtd_write_copy2ram(uint32_t addr, uint8_t *src, unsigned int len)
{
    uint8_t buf_tmp[64];
    unsigned int len_tmp;

    while (len > 0) {
        if (len >= 64) {
            len_tmp = 64;
        } else {
            len_tmp = len;
        }

        memcpy(buf_tmp, src, len_tmp);

        bflb_flash_write(addr, buf_tmp, len_tmp);

        addr += len_tmp;
        src += len_tmp;
        len -= len_tmp;
    }

    return 0;
}

static int _mtd_write(uint32_t addr, uint8_t *src, unsigned int len)
{

    bflb_flash_write(addr, src, len);

    return 0;
}

#if defined(BL602) || defined(BL702)
int bflb_sys_isxipaddr(uint32_t addr)
{
    if (((addr & 0xFF000000) == 0x23000000) || ((addr & 0xFF000000) == 0x43000000)) {
        return 1;
    }
    return 0;
}
#elif defined(BL616)
static int bflb_sys_isxipaddr(uint32_t addr)
{
    if ( ((addr & 0xFF000000) == 0xA0000000) || ((addr & 0xFF000000) == 0xA1000000) ||
         ((addr & 0xFF000000) == 0xA2000000) || ((addr & 0xFF000000) == 0xA3000000) ||
         ((addr & 0xFF000000) == 0xA4000000) || ((addr & 0xFF000000) == 0xA5000000) ||
         ((addr & 0xFF000000) == 0xA6000000) || ((addr & 0xFF000000) == 0xA7000000) ) {
        return 1;
    }
    return 0;
}
#elif defined(BL808)
int bflb_sys_isxipaddr(uint32_t addr)
{
    //XXX is 0xD000_0000 Address Range is really used
    if ( ((addr & 0xFF000000) == 0x58000000) || ((addr & 0xFF000000) == 0x5C000000) ||
         ((addr & 0xFF000000) == 0xD8000000) || ((addr & 0xFF000000) == 0xDC000000)) {
        return 1;
    }
    return 0;
}
#endif

void bflb_mtd_init(void)
{
    bflb_boot2_init();
}

int bflb_mtd_open(const char *name, bflb_mtd_handle_t *handle, unsigned int flags)
{
    uint32_t addr = 0;
    uint32_t size = 0;
    int ret;
    bflb_mtd_handle_priv_t handle_prv;

    handle_prv = (bflb_mtd_handle_priv_t)malloc(sizeof(struct bflb_mtd_handle_priv));

    if (NULL == handle_prv) {
        return -1;
    }
    memset(handle_prv, 0, sizeof(struct bflb_mtd_handle_priv));
    if(strlcpy(handle_prv->name, name, sizeof(handle_prv->name)) >= sizeof(handle_prv->name))
        printf("[OS]: strlcpy truncated \r\n");

    if (flags & BFLB_MTD_OPEN_FLAG_BACKUP) {
        /* open backup mtd partition*/
        if (flags & BFLB_MTD_OPEN_FLAG_BUSADDR) {
            if ((ret = bflb_boot2_partition_bus_addr_inactive(name, &addr, &size)) || 0 == addr) {
                printf("[MTD] [PART] [XIP] error when get %s partition %d\r\n", name, ret);
                printf("[MTD] [PART] [XIP] Dead Loop. Reason: no Valid %s partition found\r\n", name);
                while (1) {
                }
            }
            handle_prv->xip_addr = (void*)addr;
        } else {
            handle_prv->xip_addr = 0;
        }
        if ((ret = bflb_boot2_partition_addr_inactive(name, &addr, &size)) || 0 == addr) {
            printf("[MTD] [PART] [XIP] error when get %s partition %d\r\n", name, ret);
            printf("[MTD] [PART] [XIP] Dead Loop. Reason: no Valid %s partition found\r\n", name);
            while (1) {
            }
        }
        handle_prv->offset = addr;
        handle_prv->size = size;
    } else {
        /* open active mtd partition*/
        if (flags & BFLB_MTD_OPEN_FLAG_BUSADDR) {
            if ((ret = bflb_boot2_partition_bus_addr_active(name, &addr, &size)) || 0 == addr) {
                printf("[MTD] [PART] [XIP] error when get %s partition %d\r\n", name, ret);
                printf("[MTD] [PART] [XIP] Dead Loop. Reason: no Valid %s partition found\r\n", name);
                while (1) {
                }
            }
            handle_prv->xip_addr = (void*)addr;
        } else {
            handle_prv->xip_addr = 0;
        }
        if ((ret = bflb_boot2_partition_addr_active(name, &addr, &size)) || 0 == addr) {
            printf("[MTD] [PART] [XIP] error when get %s partition %d\r\n", name, ret);
            printf("[MTD] [PART] [XIP] Dead Loop. Reason: no Valid %s partition found\r\n", name);
            while (1) {
            }
        }
        handle_prv->offset = addr;
        handle_prv->size = size;
    }
    __dump_mtd_handle(handle_prv);
    *handle = handle_prv;

    return 0;
}

int bflb_mtd_close(bflb_mtd_handle_t handle)
{
    free(handle);

    return 0;
}

int bflb_mtd_info(bflb_mtd_handle_t handle, bflb_mtd_info_t *info)
{
    bflb_mtd_handle_priv_t handle_prv = (bflb_mtd_handle_priv_t)handle;

    if(strlcpy(info->name, handle_prv->name, sizeof(info->name)) >= sizeof(info->name))
        printf("[OS]: strlcpy truncated \r\n");
    info->offset = handle_prv->offset;
    info->size = handle_prv->size;
    info->xip_addr = handle_prv->xip_addr;

    return 0;
}

int bflb_mtd_erase(bflb_mtd_handle_t handle, unsigned int addr, unsigned int size)
{
    bflb_mtd_handle_priv_t handle_prv = (bflb_mtd_handle_priv_t)handle;

    bflb_flash_erase(
            handle_prv->offset + addr,
            size
    );

    return 0;
}

int bflb_mtd_erase_all(bflb_mtd_handle_t handle)
{
    bflb_mtd_handle_priv_t handle_prv = (bflb_mtd_handle_priv_t)handle;

    bflb_flash_erase(
            handle_prv->offset + 0,
            handle_prv->size
    );

    return 0;
}

int bflb_mtd_write(bflb_mtd_handle_t handle, unsigned int addr, unsigned int size, const uint8_t *data)
{
    bflb_mtd_handle_priv_t handle_prv = (bflb_mtd_handle_priv_t)handle;
    uint32_t real_addr = handle_prv->offset + addr;

    if (bflb_sys_isxipaddr((uint32_t)data)) {
        printf("addr@%p is xip flash, size %d\r\n", data, size);
        _mtd_write_copy2ram(real_addr, (uint8_t*)data, size);
    } else {
        _mtd_write(real_addr, (uint8_t*)data, size);
    }

    return 0;
}

int bflb_mtd_read(bflb_mtd_handle_t handle,  unsigned int addr, unsigned int size, uint8_t *data)
{
    bflb_mtd_handle_priv_t handle_prv = (bflb_mtd_handle_priv_t)handle;

    bflb_flash_read(
            handle_prv->offset + addr,
            data,
            size
    );

    return 0;
}

int bflb_mtd_size(bflb_mtd_handle_t handle, unsigned int *size)
{
    bflb_mtd_handle_priv_t handle_prv = (bflb_mtd_handle_priv_t)handle;
    if (NULL == handle) {
        return -1;
    }
    *size = handle_prv->size;

    return 0;
}
