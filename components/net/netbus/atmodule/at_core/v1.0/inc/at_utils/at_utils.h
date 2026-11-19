
#ifndef __UTILS_MEMP_H__
#define __UTILS_MEMP_H__


struct utils_memp_node {
    struct utils_memp_node *next;
};

typedef struct pool {
    uint32_t node_size;
    uint32_t pool_cap;
    uint32_t pool_size;
    uint8_t align_req;
    uint32_t padded_node_size;
    void *first_node;
    void *last_node;
    struct utils_memp_node *mem;
}utils_memp_pool_t;

int utils_memp_init(utils_memp_pool_t **pool, uint16_t node_size, uint16_t pool_cap, uint8_t align_req);
int utils_memp_deinit(utils_memp_pool_t *pool);
void *utils_memp_malloc(utils_memp_pool_t *pool);
int utils_memp_free(utils_memp_pool_t *pool, void *node);

#endif

