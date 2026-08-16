#include <kernel/pmm/pmm.h>
#include <arch/pmm_hal.h>
#include <kernel/printk.h>

struct physical_memory_map pmm_mmap;

void
pmm_init(struct boot_info* b)
{
    printk(" * PMM Init\n");
    arch_pmm_init(b, &pmm_mmap);
    printk(" * PMM Init [ OK ]\n");
}

void*
pmm_alloc_page()
{
    return 0;
}

void
pmm_free_page_n(uint32_t pageid)
{

}

void
pmm_free_page(void* ptr)
{

}

void*
pmm_alloc_pages(uint32_t)
{
    return 0;
}