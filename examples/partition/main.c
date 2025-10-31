#include "bflb_flash.h"
#include "partition.h"
#include "board.h"

#define DBG_TAG "MAIN"
#include "log.h"

#define USER_UNUSED(a) ((void)(a))

static void _dump_partition(pt_table_stuff_config *part)
{
    int i;

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

int main(void)
{
    board_init();

    pt_table_set_flash_operation(bflb_flash_erase, bflb_flash_write, bflb_flash_read);

    pt_table_stuff_config ptstuff[2];
    pt_table_id_type active_id;

    active_id = pt_table_get_active_partition_need_lock(ptstuff);

    if (active_id < PT_TABLE_ID_INVALID) {
        _dump_partition(&ptstuff[active_id]);
    } else {
        printf("partition init fail!\r\n");
    }

    while (1) {
    }
}
