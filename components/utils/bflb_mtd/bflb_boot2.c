#include "bflb_boot2.h"
#include "partition.h"
#include "bflb_flash.h"

#define USER_UNUSED(a) ((void)(a))

#define PARTITION_MAGIC                 (0x54504642)
#define PARTITION_FW_PART_NAME          "FW"
#define PARTITION_FW_PART_HEADER_SIZE   (0x1000)
//TODO use header file from project
#define FW_XIP_ADDRESS                  (FLASH_XIP_BASE)

static struct {
    uint8_t partition_active_idx;
    uint8_t pad[3];
    pt_table_stuff_config table;
} boot2_partition_table;//XXX Dont change the name of varaible, since we refer this boot2_partition_table in linker script

int bflb_boot2_update_ptable(bflb_partition_config_t *ptEntry_hal)
{
    int ret;
    //FIXME force covert
    pt_table_entry_config *ptEntry = (pt_table_entry_config*)ptEntry_hal;

    ptEntry->active_index = !ptEntry->active_index;
    (ptEntry->age)++;
    ret = pt_table_update_entry(!boot2_partition_table.partition_active_idx, &boot2_partition_table.table, ptEntry);
    return ret;
}

static void _dump_partition(void)
{
    int i;
    pt_table_stuff_config *part = &boot2_partition_table.table;

    USER_UNUSED(i);
    USER_UNUSED(part);

    printf("======= PtTable_Config @%p=======\r\n", part);
    printf("magicCode 0x%08X;", (unsigned int)(part->pt_table.magicCode));
    printf(" version 0x%04X;", part->pt_table.version);
    printf(" entryCnt %u;", part->pt_table.entryCnt);
    printf(" age %lu;", part->pt_table.age);
    printf(" crc32 0x%08X\r\n", (unsigned int)part->pt_table.crc32);

    printf("idx  type device active_index     name   Address[0]  Address[1]  Length[0]   Length[1]   age\r\n");
    for (i = 0; i < part->pt_table.entryCnt; i++) {
        printf("[%02d] ", i);
        printf(" %02u", part->pt_entries[i].type);
        printf("     %u", part->pt_entries[i].device);
        printf("         %u", part->pt_entries[i].active_index);
        printf("     %8s", part->pt_entries[i].name);
        printf("  0x%08lx", (part->pt_entries[i].start_address[0]));
        printf("  0x%08lx", (part->pt_entries[i].start_address[1]));
        printf("  0x%08lx", (part->pt_entries[i].max_len[0]));
        printf("  0x%08lx", (part->pt_entries[i].max_len[1]));
        printf("  %lu\r\n", (part->pt_entries[i].age));
    }
}

int bflb_boot2_partition_bus_addr(const char *name, uint32_t *addr0, uint32_t *addr1, uint32_t *size0, uint32_t *size1, int *active)
{
    int i;
    uint32_t addr0_t, addr1_t;

    if (PARTITION_MAGIC != boot2_partition_table.table.pt_table.magicCode) {
        return -EIO;
    }

    /*Get Target partition*/
    for (i = 0; i < boot2_partition_table.table.pt_table.entryCnt; i++) {
        if (0 == strcmp((char *)&(boot2_partition_table.table.pt_entries[i].name[0]), name)) {
            break;
        }
    }
    if (boot2_partition_table.table.pt_table.entryCnt == i) {
        return -ENOENT;
    }
    addr0_t = boot2_partition_table.table.pt_entries[i].start_address[0];
    addr1_t = boot2_partition_table.table.pt_entries[i].start_address[1];
    *active = boot2_partition_table.table.pt_entries[i].active_index;
    *size0 = boot2_partition_table.table.pt_entries[i].max_len[0];
    *size1 = boot2_partition_table.table.pt_entries[i].max_len[1];

    /*cal partition address*/
    for (i = 0; i < boot2_partition_table.table.pt_table.entryCnt; i++) {
        if (0 == strcmp((char *)&(boot2_partition_table.table.pt_entries[i].name[0]), PARTITION_FW_PART_NAME)) {
            break;
        }
    }
    if (boot2_partition_table.table.pt_table.entryCnt == i) {
        return -ECANCELED;
    }
    /*Make sure target partition is after FW partition*/
    if ( (addr0_t && (addr0_t < boot2_partition_table.table.pt_entries[i].start_address[0])) ||
         (addr0_t && (addr0_t < boot2_partition_table.table.pt_entries[i].start_address[1])) ||
         (addr1_t && (addr1_t < boot2_partition_table.table.pt_entries[i].start_address[0])) ||
         (addr1_t && (addr1_t < boot2_partition_table.table.pt_entries[i].start_address[1]))) {
        return -EINVAL;
    }
    if ((0 != boot2_partition_table.table.pt_entries[i].active_index) &&
        (1 != boot2_partition_table.table.pt_entries[i].active_index)) {
        return -EFAULT;
    }
    *addr0 = addr0_t - boot2_partition_table.table.pt_entries[i].start_address[boot2_partition_table.table.pt_entries[i].active_index] - PARTITION_FW_PART_HEADER_SIZE + FW_XIP_ADDRESS;
    *addr1 = addr1_t - boot2_partition_table.table.pt_entries[i].start_address[boot2_partition_table.table.pt_entries[i].active_index] - PARTITION_FW_PART_HEADER_SIZE + FW_XIP_ADDRESS;

    return 0;
}

int bflb_boot2_partition_bus_addr_active(const char *name, uint32_t *addr, uint32_t *size)
{
    uint32_t addr0, addr1;
    uint32_t size0, size1;
    int active, ret;

    if ((ret = bflb_boot2_partition_bus_addr(name, &addr0, &addr1, &size0, &size1, &active))) {
        return ret;
    }
    *addr = active ? addr1 : addr0;
    *size = active ? size1 : size0;

    return 0;
}

int bflb_boot2_partition_bus_addr_inactive(const char *name, uint32_t *addr, uint32_t *size)
{
    uint32_t addr0, addr1;
    uint32_t size0, size1;
    int active, ret;

    if ((ret = bflb_boot2_partition_bus_addr(name, &addr0, &addr1, &size0, &size1, &active))) {
        return ret;
    }
    *addr = active ? addr0 : addr1;
    *size = active ? size0 : size1;

    return 0;
}

int bflb_boot2_partition_addr(const char *name, uint32_t *addr0, uint32_t *addr1, uint32_t *size0, uint32_t *size1, int *active)
{
    int i;

    if (PARTITION_MAGIC != boot2_partition_table.table.pt_table.magicCode) {
        return -EIO;
    }

    /*Get Target partition*/
    for (i = 0; i < boot2_partition_table.table.pt_table.entryCnt; i++) {
        if (0 == strcmp((char *)&(boot2_partition_table.table.pt_entries[i].name[0]), name)) {
            break;
        }
    }
    if (boot2_partition_table.table.pt_table.entryCnt == i) {
        return -ENOENT;
    }
    *addr0 = boot2_partition_table.table.pt_entries[i].start_address[0];
    *addr1 = boot2_partition_table.table.pt_entries[i].start_address[1];
    *size0 = boot2_partition_table.table.pt_entries[i].max_len[0];
    *size1 = boot2_partition_table.table.pt_entries[i].max_len[1];
    *active = boot2_partition_table.table.pt_entries[i].active_index;

    return 0;
}

int bflb_boot2_partition_addr_active(const char *name, uint32_t *addr, uint32_t *size)
{
    uint32_t addr0, addr1;
    uint32_t size0, size1;
    int active, ret;

    if ((ret = bflb_boot2_partition_addr(name, &addr0, &addr1, &size0, &size1, &active))) {
        return ret;
    }
    *addr = active ? addr1 : addr0;
    *size = active ? size1 : size0;

    return 0;
}

int bflb_boot2_partition_addr_inactive(const char *name, uint32_t *addr, uint32_t *size)
{
    uint32_t addr0, addr1;
    uint32_t size0, size1;
    int active, ret;

    if ((ret = bflb_boot2_partition_addr(name, &addr0, &addr1, &size0, &size1, &active))) {
        return ret;
    }
    *addr = active ? addr0 : addr1;
    *size = active ? size0 : size1;

    return 0;
}

uint8_t bflb_boot2_get_active_partition(void)
{
    return boot2_partition_table.partition_active_idx;
}

int bflb_boot2_get_active_entries_byname(uint8_t *name, bflb_partition_config_t *ptEntry_hal)
{
    pt_table_entry_config *ptEntry = (pt_table_entry_config*)ptEntry_hal;
    if (pt_table_get_active_entries_by_name(&boot2_partition_table.table, name, ptEntry)) {
        return -1;
    }
    return 0;
}

int bflb_boot2_get_active_entries(int type, bflb_partition_config_t *ptEntry_hal)
{
    pt_table_entry_config *ptEntry = (pt_table_entry_config*)ptEntry_hal;
    if (pt_table_get_active_entries_by_id(&boot2_partition_table.table, type, ptEntry)) {
        return -1;
    }
    return 0;
}

int bflb_boot2_dump(void)
{
    _dump_partition();
    return 0;
}

int bflb_boot2_init(void)
{
    pt_table_stuff_config ptstuff[2];
    pt_table_id_type active_id;

    pt_table_set_flash_operation(bflb_flash_erase, bflb_flash_write, bflb_flash_read);

    active_id = pt_table_get_active_partition_need_lock(ptstuff);

    if (active_id < PT_TABLE_ID_INVALID) {
        boot2_partition_table.partition_active_idx = active_id;
        arch_memcpy_fast(&boot2_partition_table.table, &ptstuff[active_id], sizeof(pt_table_stuff_config));
    } else {
        printf("partition init fail!\r\n");
        return -1;
    }

    printf("Active Partition[%u] consumed %d Bytes\r\n",
            boot2_partition_table.partition_active_idx,
            sizeof(pt_table_stuff_config)
    );
    _dump_partition();

    return 0;
}

#define     PT_OTA_TYPE_NAME         "FW"
#define     PT_MEDIA_TYPE_NAME       "mfg"
void bflb_update_mfg_ptable(void)
{
    pt_table_entry_config ptEntry_fw;
    pt_table_entry_config ptEntry_media;

    printf("update mfg table.\r\n");
    printf("====================\r\n");
    if (0 == bflb_boot2_get_active_entries_byname((uint8_t *)PT_OTA_TYPE_NAME, (bflb_partition_config_t *)(&ptEntry_fw))) {       // ota
        if (0 == bflb_boot2_get_active_entries_byname((uint8_t *)PT_MEDIA_TYPE_NAME, (bflb_partition_config_t *)(&ptEntry_media))) { // media
            if (ptEntry_fw.start_address[1] == ptEntry_media.start_address[0]) {

                memset(ptEntry_media.name, 0, sizeof(ptEntry_media.name));
                pt_table_update_entry(!boot2_partition_table.partition_active_idx, &boot2_partition_table.table, &ptEntry_media);

                printf("===== update mfg partition =====\r\n");
            }
        }
    }

    printf("====================\r\n");
    printf("update mfg table.\r\n");
}
