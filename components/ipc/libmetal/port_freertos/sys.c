#include <metal/io.h>
#include <metal/sys.h>
#include <metal/utilities.h>
#include <stdint.h>
#include <bflb_irq.h>
#include <bflb_l1c.h>

void sys_irq_restore_enable(unsigned int flags)
{
    bflb_irq_restore(flags);
}

unsigned int sys_irq_save_disable(void)
{
    return bflb_irq_save();
}

void sys_irq_enable(unsigned int vector)
{
    bflb_irq_enable(vector);
}

void sys_irq_disable(unsigned int vector)
{
    bflb_irq_disable(vector);
}

void metal_machine_cache_flush(void *addr, unsigned int len)
{
    bflb_l1c_dcache_clean_range((uint32_t)addr, len);
}

void metal_machine_cache_invalidate(void *addr, unsigned int len)
{
    bflb_l1c_dcache_invalidate_range((uint32_t)addr, len);
}

void metal_generic_default_poll(void)
{
    /* Add implementation here */
}

void *metal_machine_io_mem_map(void *va, metal_phys_addr_t pa,
                   size_t size, unsigned int flags)
{
    metal_unused(pa);
    metal_unused(size);
    metal_unused(flags);

    /* Add implementation here */

    return va;
}
