
#include <stdint.h>
#include <FreeRTOS.h>
#include <at_utils.h>

#define UTILS_MEMP_ALLOCED_NODE_PATTERN 0XA5
#define MEM_ALIGN(addr, align) (((addr) + (align) -1) & ~((align)-1))


int utils_memp_init(utils_memp_pool_t **pool, uint16_t node_size, uint16_t pool_cap, uint8_t align_req)
{
    utils_memp_pool_t *npool;
    struct utils_memp_node *node;
    struct utils_memp_node *pool_mem;
    size_t size;
    uint16_t padded_node_size;
    uint16_t i;

    align_req = MEM_ALIGN(align_req, sizeof(void *));
    padded_node_size = MEM_ALIGN(node_size + sizeof(struct utils_memp_node), align_req);

    size = sizeof(utils_memp_pool_t);
    size = MEM_ALIGN(size, align_req);
    size += padded_node_size * pool_cap;

    npool = pvPortMalloc(size);

    if (!npool) {
        return -1;
    }

    node = (struct utils_memp_node *)((uint8_t *)npool + (size - padded_node_size * pool_cap));
    npool->first_node = node;
    npool->node_size = node_size;
    npool->pool_cap = pool_cap;
    npool->pool_size = 0;
    npool->align_req = align_req;
    npool->padded_node_size = padded_node_size;
    npool->mem = NULL;
    pool_mem = npool->mem;

    for(i = 0; i < pool_cap; ++i) {
        node->next = pool_mem;
        pool_mem = node;
        node = (struct utils_memp_node *)((uint8_t *)node + padded_node_size);
    }
    npool->mem = pool_mem;
    npool->last_node = npool->mem;
    *pool = npool;

    return 0;
}

int utils_memp_deinit(utils_memp_pool_t *pool)
{
    if (!pool) {
        return -1;
    }
    vPortFree(pool);

    return 0;
}

void *utils_memp_malloc(utils_memp_pool_t *pool)
{
    struct utils_memp_node *node;
    uint32_t *pat;
    if (!pool) {
        return NULL;
    }
    if (pool->pool_size == pool->pool_cap) {
        return NULL;
    }
    node = pool->mem;
    if (node != NULL) {
        pool->mem = node->next;
        pool->pool_size++;
        pat = (uint32_t *)&node->next;
        *pat = UTILS_MEMP_ALLOCED_NODE_PATTERN;
        return (void *)node + sizeof(struct utils_memp_node);
    } else {
        return NULL;
    }
}

int utils_memp_free(utils_memp_pool_t *pool, void *node)
{
    struct utils_memp_node *utils_memp_node;
    node = node - sizeof(struct utils_memp_node);
    uint32_t *pat;
    int diff;

    if (!pool || !node) {
        return -1;
    }
    if (pool->pool_size == 0) {
        return -1;
    }
    if (!(node >= pool->first_node && node <= pool->last_node)) {
        return -1;
    }
    diff = node - pool->first_node;
    if (diff % pool->padded_node_size) {
        return -1;
    }

    pat = (uint32_t *)(&((struct utils_memp_node *)node)->next);
    if (*pat != UTILS_MEMP_ALLOCED_NODE_PATTERN) {
        return -1;
    }
    utils_memp_node = (struct utils_memp_node*)node;
    utils_memp_node->next = pool->mem;
    pool->mem = utils_memp_node;
    pool->pool_size--;
    return 0;
}
