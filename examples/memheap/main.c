#include "bflb_mtimer.h"
#include "board.h"
#include "string.h"
#include "stdlib.h"

#include "mem.h"

//随机生成堆的参数
//堆区最大值
#define MAX_HEAP_REGION			1024
//堆区最小值
#define MIN_HEAP_REGION			16
//堆区倍乘数值
#define HEAP_BLOCK_SIZE			16
//堆区数量最大值
#define MAX_HEAP_REGION_NUM		10
//堆区数量最小值
#define MIN_HEAP_REGION_NUM		2

//随机申请内存的参数
//随机申请内存的最大对齐,2^MAX_ALIGN_MALLOC
#define MAX_ALIGN_MALLOC		10
//随机申请内存的最小对齐,2^MIN_ALIGN_MALLOC
#define MIN_ALIGN_MALLOC		2
//随机申请的最大内存
#define MAX_WANT_MALLOC			1024
//随机申请的最小内存
#define MIN_WANT_MALLOC			1
//随机申请内存最大次数
#define MAX_MALLOC_CNT			1000

typedef struct heap_region {
    void *addr;
    size_t mem_size;
} Mem_Region;

struct mem_heap_s g_userheap;

//生成指定范围的随机数
static inline unsigned int Get_Rand(unsigned int max, unsigned int min)
{
    return random() % (max - min + 1) + min;
}


//生成一个随机的堆
Mem_Region *Mem_Manage_Generate_Rand_Heap(void)
{
    size_t region_num;
    Mem_Region *pRegion;

    // region_num = Get_Rand(MAX_HEAP_REGION_NUM, MIN_HEAP_REGION_NUM);
    region_num = 1;
    pRegion = malloc(sizeof(Mem_Region) * (region_num + 1));

    if (pRegion == NULL)
        return NULL;

    // for (size_t i = 0; i < region_num; i++) {
        // size_t region_size = Get_Rand(MAX_HEAP_REGION, MIN_HEAP_REGION) * HEAP_BLOCK_SIZE;
        size_t region_size = 50 * 1024;
        size_t i = 0;

        void *paddr = malloc(512); //确保内存不连续

        pRegion[i].addr = malloc(region_size);
        pRegion[i].mem_size = region_size;
        free(paddr);
    // }

    // for (size_t i = 0; i < region_num; i++) { //按地址进行排序，从低到高
    //     for (size_t j = i + 1; j < region_num; j++) {
    //         if (pRegion[i].addr > pRegion[j].addr) {
    //             Mem_Region buf_region = pRegion[i];
    //             pRegion[i] = pRegion[j];
    //             pRegion[j] = buf_region;
    //         }
    //     }
    // }

    pRegion[region_num].addr = NULL;
    pRegion[region_num].mem_size = 0;

    return pRegion;
}

void Mem_Manage_Free_Rand_Heap(Mem_Region *const pHeap)
{
    for (Mem_Region *buf_heap = pHeap; buf_heap->addr != NULL; buf_heap++) {
        free(buf_heap->addr);
    }

    free(pHeap);
}

//打印堆的信息
void Mem_Manage_Show_Heap_Info(Mem_Region *pHeap)
{
    printf("地址\t\t\t大小\r\n");
    for (; pHeap->addr != NULL; pHeap++) {
        printf("0x%08x\t\t%d\r\n", (size_t)pHeap->addr, pHeap->mem_size);
    }
}
//打印当前内存剩余
size_t Mem_Manage_Show_Free_Heap(struct meminfo *info)
{
    bflb_mem_usage(&g_userheap, info);
    printf("剩余内存：%d\r\n", info->free_size);
    printf("内存利用率：%.2f%%\r\n", (info->used_size) * 100.0 / info->total_size);
    printf("最大内存块：%d\r\n", info->max_free_size);
    // printf("最小内存块：%d\r\n", use_state.min_node_size);
    printf("剩余内存块数：%d\r\n", info->free_node);
    return info->free_node;
}

void memtest(void)
{
    Mem_Region *pRegion;
    struct meminfo info;
    uint64_t max_time = 0;
    float avr_time = 0.0;
    uint64_t time;
    uint64_t time_tmp;

    // while (1) {
        size_t heap_num;
        printf("生成随机内存堆...\r\n");
        printf("<---------内存堆信息------------>\r\n");

        pRegion = Mem_Manage_Generate_Rand_Heap();

        Mem_Manage_Show_Heap_Info(pRegion);

        printf("<-------初始化内存堆--------->\r\n");
        bflb_mem_init(&g_userheap, pRegion[0].addr, pRegion[0].mem_size);

        // printf("start块:0x%08x,%d\r\n", (size_t)Mem_Handle.pStart, Mem_Handle.pStart->mem_size);
        // printf("end 块:0x%08x,%d\r\n", (size_t)Mem_Handle.pEnd, Mem_Handle.pEnd->mem_size);
        printf("总内存:%d\r\n", pRegion[0].mem_size);
        printf("<---------初始空闲列表------------>\r\n");

        heap_num = Mem_Manage_Show_Free_Heap(&info);

        printf("<-------malloc测试------------>\r\n");
        {
            void **addr_buf = malloc(MAX_MALLOC_CNT * sizeof(void *));
            size_t *want_size = malloc(MAX_MALLOC_CNT * sizeof(size_t));
            void *buf_addr;
            size_t malloc_cnt = 0;
            size_t align_size;
            size_t buf_size;

            while (malloc_cnt < MAX_MALLOC_CNT) {
                align_size = 1 << Get_Rand(MAX_ALIGN_MALLOC, MIN_ALIGN_MALLOC); //确保为2的指数次幂
                want_size[malloc_cnt] = Get_Rand(MAX_WANT_MALLOC, MIN_WANT_MALLOC);

                time_tmp = bflb_mtimer_get_time_us();
                addr_buf[malloc_cnt] = bflb_malloc_align(&g_userheap, align_size, want_size[malloc_cnt]);
                time = bflb_mtimer_get_time_us() - time_tmp;

                if (max_time < time) {
                    max_time = time;
                }
                avr_time += (float)time;
                printf("malloc time %lld us %p\r\n", time, addr_buf[malloc_cnt]);

                if (addr_buf[malloc_cnt] == NULL) {
                    printf("malloc分配失败!\r\n");
                    printf("期望对齐：%d,期望大小：%d\r\n", align_size, want_size[malloc_cnt]);
                    break;
                }
                if ((size_t)addr_buf[malloc_cnt] % align_size) {
                    printf("malloc 对齐出错！\r\n");
                }

                memset(addr_buf[malloc_cnt], malloc_cnt, want_size[malloc_cnt]);

                buf_size = Get_Rand(MAX_WANT_MALLOC, want_size[malloc_cnt]);
                buf_addr = bflb_realloc(&g_userheap, addr_buf[malloc_cnt], buf_size);

                if (buf_addr == NULL) {
                    printf("realloc分配失败!\r\n");
                    printf("期望大小：%d\r\n", buf_size);
                    break;
                }
                if ((size_t)buf_addr % align_size) {
                    printf("realloc 对齐出错！%p\r\n", buf_addr);
                }

                memset((uint8_t *)buf_addr + want_size[malloc_cnt], malloc_cnt, buf_size - want_size[malloc_cnt]);
                addr_buf[malloc_cnt] = buf_addr;
                want_size[malloc_cnt] = buf_size;
                malloc_cnt++;
            }

            avr_time = avr_time / (float)malloc_cnt;

            printf("共分配%d次, 平均malloc耗时%.02fus, 最大耗时%ldus\r\n", malloc_cnt, avr_time, max_time);
            printf("<---------测试后空闲列表------------>\r\n");
            Mem_Manage_Show_Free_Heap(&info);

            printf("内存区检查...\r\n");
            for (size_t i = 0; i < malloc_cnt; i++) {
                for (size_t j = 0; j < want_size[i]; j++) {
                    if (((uint8_t *)addr_buf[i])[j] != (uint8_t)i) {
                        printf("校验出错！\r\n");
                    }
                }
            }

            printf("<---------free测试----------->\r\n");
            for (size_t i = 0; i < malloc_cnt; i++) {
                bflb_free(&g_userheap, addr_buf[i]);
            }

            free(addr_buf);
            free(want_size);
        }

        // printf("start块:0x%08x,%d\r\n", (size_t)Mem_Handle.pStart, Mem_Handle.pStart->mem_size);
        // printf("end 块:0x%08x,%d\r\n", (size_t)Mem_Handle.pEnd, Mem_Handle.pEnd->mem_size);
        printf("总内存:%d\r\n", pRegion[0].mem_size);
        printf("<---------内存释放后空闲列表------------>\r\n");
        if (Mem_Manage_Show_Free_Heap(&info) != heap_num) {
            printf("算法异常！\r\n");
        }

        Mem_Manage_Free_Rand_Heap(pRegion);
    // }
}

int memheap_test(void)
{
    char *ptr = NULL;

    for (int i = 1;; i++) {
        ptr = malloc(i * 128);

        if (ptr != NULL) {
            memcpy(ptr, "hello123456789123456789123456789", 33);
            printf("ptr :%s\r\n", ptr);
            printf("get memory :%d byte\r\n", i * 128);
            free(ptr);
            printf("free memory :%d byte\r\n", i * 128);
            ptr = NULL;
            bflb_mtimer_delay_ms(100);
        } else {
            printf("try to get %d byte memory failed!\r\n", i * 128);

            return -1;
        }
    }
    return 0;
}

int main(void)
{
    board_init();

    memtest();

    if (memheap_test() == -1) {
        printf("memheap test fail\r\n");
        while (1) {
        }
    }
    printf("memheap test success\r\n");
    while (1) {
        bflb_mtimer_delay_ms(1000);
    }
}
