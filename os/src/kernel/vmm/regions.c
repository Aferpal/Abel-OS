#include "kernel/vmm/regions.h"
#include "kernel/vmm/vmm.h"
#include "kernel/vmm/defines.h"

#define BASE_STATIC_REGION_POOL_SIZE 16
#define MINIMUM_POOL_SIZE            4

struct vmm_region free_region_static_pool[BASE_STATIC_REGION_POOL_SIZE] = {0};
struct vmm_region* free_regions = &(free_region_static_pool[0]);
uint32_t region_pool_size = BASE_STATIC_REGION_POOL_SIZE;


/* ============================================================= */
/*               STATIC LOCAL FUNCTIONS                          */
/* ============================================================= */

static struct vmm_region*
get_new_free_region()
{

    if (region_pool_size == 0) {
        return 0;
    }

    /* save the pointer to the first free region */
    struct vmm_region* ret = free_regions;

    /* update the list, now pointing to second free region */
    free_regions = free_regions->next;

    if (free_regions) {
        free_regions->prev = 0;
    }

    region_pool_size--;

    /* clean the return region values */
    ret->next = 0;
    ret->prev = 0;

    return ret;
    
}

static void
add_region_to_pool(struct vmm_region* region) {

    region->start_address = 0;
    region->length = 0;
    region->prev = 0;
    region->next = free_regions;
    if (free_regions) {
        free_regions->prev = region;
    }
    free_regions = region;
    region_pool_size++;

}

static void 
coalesce_region(struct vmm_region* region)
{
        /* COALESCING */
    struct vmm_region* tmp;

    /* with left */
    if (region->prev && 
        ((region->prev->flags & region->flags & VMM_REGION_USABLE) ||
         (region->prev->flags == region->flags))) {

        tmp = region->prev;

        region->start_address = region->prev->start_address;
        region->length += region->prev->length;
        region->prev = region->prev->prev;

        if (tmp->prev) {
            tmp->prev->next = region;
        }

        /* return the now not needed region to the pool */
        add_region_to_pool(tmp);

    }

    /* with right */
    if (region->next && 
        ((region->next->flags & region->flags & VMM_REGION_USABLE)||
         (region->next->flags == region->flags))) {

        tmp = region->next;

        region->length += region->next->length;
        region->next = region->next->next;

        if (tmp->next) {
            tmp->next->prev = region;
        }

        /* return the now not needed region to the pool */
        add_region_to_pool(tmp);

    }

}

static uint8_t
vmm_region_pool_should_grow()
{
    return region_pool_size < MINIMUM_POOL_SIZE;
}

static void
vmm_region_pool_grow()
{
    
    struct vmm_region* new_page = (struct vmm_region*)vmm_alloc_page_noalloc(VMM_PROT_WRITE | VMM_PROT_KERNEL);
        
    if (new_page != 0) {

        uint32_t n_regions = (4096/sizeof(struct vmm_region));

        new_page->prev = free_regions;
        new_page->next = &(new_page[1]);

        for (int i = 0; i < n_regions - 1; i++) {
            new_page[i].next = &(new_page[i+1]);
            new_page[i+1].prev = &(new_page[i]);
        }

        new_page[0].prev = 0;
        new_page[n_regions-1].next = free_regions;

        if (free_regions) {
            free_regions->prev = &(new_page[n_regions - 1]);
        }

        free_regions = new_page;

        region_pool_size += n_regions;

    }

}

static uint32_t
vmm_reserve_range_as(struct vmm_region* region, uint32_t len, 
                     uint32_t reg_flags, uint32_t flags)
{
    /* either no region was found or the region asked is not usable */
    if (!region || !(region->flags & VMM_REGION_USABLE)) {
        return VMM_REGION_FAIL;
    }

    /* the given range is invalid, crosses multiple regions */
    if ( region->length < len ) {
        return VMM_REGION_BAD_ARGS;
    }

    /* from here we know that the range asked is g */


    /* SPLITTING */

    /* if it is the full region just ok */
    if (region->length == len) {

        region->flags = VMM_REGION_USED | reg_flags;

    /* otherwise it is at the start */
    } else {
        
        struct vmm_region* new_region = get_new_free_region();

        if (!new_region) {
            return VMM_REGION_FAIL;
        }

        new_region->flags = region->flags;
		new_region->length = (region->length - len);
		new_region->next = region->next;
		new_region->start_address = region->start_address + len;
        new_region->prev = region;

        if (region->next) {
            region->next->prev = new_region;
        }

		region->length = len;
		region->next = new_region;
		region->flags = VMM_REGION_USED | reg_flags;

    }


   coalesce_region(region);

    /* if i can alloc just try to*/
    if (!(flags & VMM_NOALLOC) && vmm_region_pool_should_grow()) {
        vmm_region_pool_grow();
    }

    return VMM_REGION_SUCCESS;

}

static uint32_t
_vmm_free_range(struct vmm_region* region, uint32_t start, uint32_t len, uint32_t flags)
{
    if (len == 0 || start == 0 || start < region->start_address || start > region->start_address+region->length || start + len > region->start_address + region->length) {
        return VMM_REGION_BAD_ARGS;
    }

    if (!(region->flags & VMM_REGION_USED)) {
        return VMM_REGION_BAD_ARGS;
    }


    /* SPLITTING */
    /* if it is the full region just ok */
    if (region->start_address == start && region->length == len) {

        region->flags = VMM_REGION_USABLE;

    /* otherwise it is at the start */
    } else if (region->start_address == start) {
        
        struct vmm_region* new_region = get_new_free_region();

        if (!new_region) {
            return VMM_REGION_FAIL;
        }

        new_region->flags = region->flags;
		new_region->length = (region->length - len);
		new_region->next = region->next;
		new_region->start_address = region->start_address + len;
        new_region->prev = region;

        if (region->next) {
            region->next->prev = new_region;
        }

		region->length = len;
		region->next = new_region;
		region->flags = VMM_REGION_USABLE;

    } else if ( (region->start_address + region->length) == (start+len)) {

        struct vmm_region* new_region = get_new_free_region();

        if (!new_region) {
            return VMM_REGION_FAIL;
        }

        new_region->flags = region->flags;
		new_region->length = (region->length - len);
		new_region->next = region;
		new_region->start_address = region->start_address;
        new_region->prev = region->prev;

        if (region->prev) {
            region->prev->next = new_region;
        }

		region->length = len;
        region->start_address = start;
		region->prev = new_region;
		region->flags = VMM_REGION_USABLE;

    } else {

        struct vmm_region* left_region = get_new_free_region();
        struct vmm_region* right_region;

        if (!left_region) {
            return VMM_REGION_FAIL;
        }

        right_region = get_new_free_region();

        if (!right_region) {
            add_region_to_pool(left_region);
            return VMM_REGION_FAIL;
        }

        left_region->flags = region->flags;
		left_region->length = (start - region->start_address);
        left_region->start_address = region->start_address;
		left_region->next = region;
        left_region->prev = region->prev;

        if (region->prev) {
            region->prev->next = left_region;
        }

        right_region->flags = region->flags;
        right_region->start_address = (start + len);
		right_region->length = (region->start_address + region->length - right_region->start_address);
		right_region->next = region->next;
        right_region->prev = region;

        if (region->next) {
            region->next->prev = right_region;
        }

		region->length = len;
        region->start_address = start;
        region->prev = left_region;
        region->next = right_region;
		region->flags = VMM_REGION_USABLE;

    }


    coalesce_region(region);

    /* if i can alloc just try to*/
    if (!(flags & VMM_NOALLOC) && vmm_region_pool_should_grow()) {
        vmm_region_pool_grow();
    }

    return VMM_REGION_SUCCESS;

}

/* ============================================================= */
/*                          GLOBAL API                           */
/* ============================================================= */

void
vmm_region_pool_init(void)
{
    for (int i = 0; i < BASE_STATIC_REGION_POOL_SIZE - 1; i++) {
        free_region_static_pool[i].next = &(free_region_static_pool[i+1]);
        free_region_static_pool[i+1].prev = &(free_region_static_pool[i]);
    }
}

struct vmm_region*
get_region_from_ptr(struct vmm_region* region_list, ptr_t ptr)
{
	struct vmm_region* region = region_list;

	while (region != 0 && (ptr >= (region->start_address + region->length))) {
		region = region->next;
	}

	return region;	
}

struct vmm_region*
vmm_find_free_range(struct vmm_region* region_list, uint32_t len)
{
    struct vmm_region* region = region_list;

	while (region != 0 && 
           ((!(region->flags & VMM_REGION_USABLE)) || 
           (region->length < len))) {

		region = region->next;

	}

	return region;	
}

uint32_t
vmm_reserve_range(struct vmm_region* region_list, ptr_t start, uint32_t len, uint32_t flags)
{
    struct vmm_region* region = get_region_from_ptr(region_list, start);

    if (!region ||  /* if there aint such a region */
        !(region->flags & VMM_REGION_USABLE) || /* or it is not usable */
        region->start_address != start || /* or its start address is not the requested*/
        region->length < len){ /* or it is too small */
            return VMM_REGION_FAIL;
    }

    return vmm_reserve_range_as(region, len, VMM_REGION_KERNEL, flags);
}

uint32_t
vmm_reserve_io_range(struct vmm_region* region_list, ptr_t start, uint32_t len, uint32_t flags)
{
    struct vmm_region* region = get_region_from_ptr(region_list, start);

    if (!region ||  /* if there aint such a region */
        !(region->flags & VMM_REGION_USABLE) || /* or it is not usable */
        region->start_address != start || /* or its start address is not the requested*/
        region->length < len){ /* or it is too small */
            return VMM_REGION_FAIL;
    }
    return vmm_reserve_range_as(region, len, VMM_REGION_IO, flags);
}

uint32_t
vmm_free_range(struct vmm_region* region_list, ptr_t start, uint32_t len, uint32_t flags)
{ 
    struct vmm_region* region = get_region_from_ptr(region_list, start);

    if (!region ||  /* if there aint such a region */
        !(region->flags & VMM_REGION_USED)){ /* or it is not used */
            return VMM_REGION_FAIL;
    }
    return _vmm_free_range(region, start, len, flags);
}