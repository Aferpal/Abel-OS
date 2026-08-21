#include <kernel/pmm/pmm.h>
#include <arch/pmm_hal.h>
#include <kernel/printk.h>

struct physical_memory_map pmm_mmap;

static uint8_t
find_first_zero(uint8_t byte)
{
    uint8_t res = 0xff;

    for (int i = 0; i < 8; i++) {
        if ( ((0x1 << (7-i)) & byte) == 0) {
            res = i;
	    break;
        }
    }

    return res;
}

void
pmm_init(struct boot_info* b)
{
    printk(" * PMM Init\n");
    arch_pmm_init(b, &pmm_mmap);
    printk(" * PMM INITIALISED\n");
}

physical_address_t
pmm_alloc_page()
{
    uint32_t i = 0;
    uint8_t found_flag = 0;

    while (i < pmm_mmap.bitmap_len && !found_flag) {
        if (pmm_mmap.bitmap[i] != 0xff) {
            found_flag = 1;
        } else {
            i++;
        }
    }

    if (!found_flag) {
        return 0;
    }

    uint8_t offset = find_first_zero(pmm_mmap.bitmap[i]);

    /* mark as used */
    uint8_t mask = 0x1 << (7-offset);

    pmm_mmap.bitmap[i] = pmm_mmap.bitmap[i] | mask;

    /* return starting address */
    uint32_t pageid = (8 * i) + offset;
    return (physical_address_t)(pageid*4096);
}

uint32_t 
pmm_get_page(physical_address_t ptr)
{
	uint32_t p = (uint32_t)ptr;
	p = p/4096;
	return p;
}

void
pmm_free_page_n(uint32_t pageid)
{
    uint32_t bitmap_index = pageid/8;
    uint8_t bitmap_offset = pageid % 8;
    uint8_t bitmask = 0x1 << (7 - bitmap_offset);
    pmm_mmap.bitmap[bitmap_index] = (pmm_mmap.bitmap[bitmap_index] | bitmask) ^ bitmask;
}

void
pmm_free_page(physical_address_t ptr)
{
    pmm_free_page_n(pmm_get_page(ptr));
}

void*
pmm_alloc_pages(uint32_t n_pages)
{
    /* unimplemented yet */
    return 0;
}
