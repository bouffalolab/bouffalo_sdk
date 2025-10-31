/*
 * Copyright (c) 2016 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <stdio.h>
#include "slab.h"
#include "bt_errno.h"
#include "zephyr.h"


//static struct k_spinlock lock;

/**
 * @brief Initialize kernel memory slab subsystem.
 *
 * Perform any initialization of memory slabs that wasn't done at build time.
 * Currently this just involves creating the list of free blocks for each slab.
 *
 * @return N/A
 */
static int create_free_list(struct k_mem_slab *slab)
{
	uint32_t j;
	char *p;

	/* blocks must be word aligned */
	if(((slab->block_size | (uintptr_t)slab->buffer) &
				(sizeof(void *) - 1)) != 0) {
		return -EINVAL;
	}

	slab->free_list = NULL;
	p = slab->buffer;

	for (j = 0U; j < slab->num_blocks; j++) {
		*(char **)p = slab->free_list;
		slab->free_list = p;
		p += slab->block_size;
	}
	return 0;
}

#if 0

/**
 * @brief Complete initialization of statically defined memory slabs.
 *
 * Perform any initialization that wasn't done at build time.
 *
 * @return N/A
 */
static int init_mem_slab_module(const struct device *dev)
{
	int rc = 0;
	ARG_UNUSED(dev);

	Z_STRUCT_SECTION_FOREACH(k_mem_slab, slab) {
		rc = create_free_list(slab);
		if (rc < 0) {
			goto out;
		}
		z_object_init(slab);
	}

out:
	return rc;

	return 0;
}
#endif

//SYS_INIT(init_mem_slab_module, PRE_KERNEL_1,
//	 CONFIG_KERNEL_INIT_PRIORITY_OBJECTS);

int k_mem_slab_init(struct k_mem_slab *slab, void *buffer,
		    size_t block_size, uint32_t num_blocks)
{
	int rc = 0;

	slab->num_blocks = num_blocks;
	slab->block_size = block_size;
	slab->buffer = buffer;
	slab->num_used = 0U;
	rc = create_free_list(slab);
	if (rc < 0) {
		goto out;
	}
	//z_waitq_init(&slab->wait_q);
	//SYS_TRACING_OBJ_INIT(k_mem_slab, slab);

	//z_object_init(slab);

out:
	return rc;
}

int k_mem_slab_alloc(struct k_mem_slab *slab, void **mem, s32_t timeout)
{
	//k_spinlock_key_t key = k_spin_lock(&lock);
	int result;

	if (slab->free_list != NULL) {
		/* take a free block */
		*mem = slab->free_list;
		slab->free_list = *(char **)(slab->free_list);
		slab->num_used++;
		result = 0;
	} else if (timeout == K_NO_WAIT) {
		/* don't wait for a free block to become available */
		*mem = NULL;
		result = -ENOMEM;
	} else {
		///* wait for a free block or timeout */
		//result = z_pend_curr(&lock, key, &slab->wait_q, timeout);
		//if (result == 0) {
		//	*mem = _current->base.swap_data;
		//}
		result = -ENOMEM;
		return result;
	}

	//k_spin_unlock(&lock, key);
	return result;
}

void k_mem_slab_free(struct k_mem_slab *slab, void **mem)
{
	//k_spinlock_key_t key = k_spin_lock(&lock);
	//struct k_thread *pending_thread = z_unpend_first_thread(&slab->wait_q);

	//if (pending_thread != NULL) {
	//	z_thread_return_value_set_with_data(pending_thread, 0, *mem);
	//	z_ready_thread(pending_thread);
	//	z_reschedule(&lock, key);
	//} else {
		**(char ***)mem = slab->free_list;
		slab->free_list = *(char **)mem;
		slab->num_used--;
		//k_spin_unlock(&lock, key);
	//}
}
