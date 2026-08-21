#include <arch/pmm_hal.h>
#include <arch/x86/boot/boot_info.h>
#include <kernel/printk.h>
#include <mem.h>

extern uint8_t* kernel_start;
extern uint8_t* kernel_end;

static uint8_t
frames_collide(uint64_t x_start, uint64_t x_end, uint64_t y_start, uint64_t y_end)
{
	return ( (x_start >= y_start && x_start < y_end) ||
		      (y_start >= x_start && y_start < x_end) );
}

static uint64_t
get_maximum_usable_mem_address_from_e820(struct boot_info* b_info)
{
	uint64_t highest_address = 0;
    uint64_t local_top_address = 0;

	struct e820_frame_info* current_frame = 0;

	for (int i = 0; i < b_info->n_entries; i++) {
		current_frame = &(b_info->mmap_list_ptr[i]);
		local_top_address = current_frame->base + current_frame->length;

		if (local_top_address > highest_address && current_frame->type == MEM_USABLE) {
			highest_address = local_top_address;
		}
	}

	return highest_address;
}

static uint8_t
get_free_mem_range_for_bitmap(struct boot_info* b_info, uint64_t bitmap_len, physical_address_t* bitmap_addr)
{

	/* IMPORTANT RESERVED FRAMES */
	uint64_t bootstrap_start = (uint64_t)b_info->bootstrap_start;
	uint64_t bootstrap_end = (uint64_t)b_info->bootstrap_end;

	uint64_t e820_table_start = (uint64_t) (uint32_t) b_info->mmap_list_ptr;
	uint64_t e820_table_end = (e820_table_start + b_info->n_entries * sizeof(struct e820_frame_info));

	uint64_t kernel_64b_start = (uint64_t) (uint32_t) &kernel_start;
	uint64_t kernel_64b_end = (uint64_t) (uint32_t) &kernel_end;

	/**/


	/* VARS NEEDED FOR ALG */
	uint64_t current_frame_end;

	uint64_t bitmap_start_addr = 0;
	uint8_t exit = 0;

	struct e820_frame_info* current_frame;
	uint8_t bitmap_frame_found = 0;

	/* ALGORITHM */

	for (int i = 0; (i < b_info->n_entries) && !bitmap_frame_found; i++) {

		current_frame = &(b_info->mmap_list_ptr[i]);

		if (current_frame->type != MEM_USABLE || current_frame->length < bitmap_len || current_frame->base >= 0xffffffff) {
			continue;
		}
		
		/* CURRENT FRAME IS BIG ENOUGH AND USABLE */
		current_frame_end = current_frame->base + current_frame->length;

		bitmap_start_addr = current_frame->base;
		
		exit = 0;

		while (!exit) {
			if (frames_collide(bitmap_start_addr, bitmap_start_addr +  bitmap_len, bootstrap_start, bootstrap_end)) {
				bitmap_start_addr = bootstrap_end;
				continue;
			}

			if (frames_collide(bitmap_start_addr, bitmap_start_addr + bitmap_len, e820_table_start, e820_table_end)) {
				bitmap_start_addr = e820_table_end;
				continue;
			}

			if (frames_collide(bitmap_start_addr, bitmap_start_addr + bitmap_len, kernel_64b_start, kernel_64b_end)) {
				bitmap_start_addr = kernel_64b_end;
				continue;
			}

			/* If we get here that means there is a potential mem frame [bitmap_start_addr - ...] for our bitmap */
			/* would the end exceed the actual e820 frame we are looking at? */

			/* if not, we found our bitmap address */
			if ((bitmap_start_addr+bitmap_len) < current_frame_end) {
				bitmap_frame_found = 1;	
			}

			exit = 1;
		}

	}

	if (!bitmap_frame_found) {
		/* error case, no frame was found, return failure*/
		return -1;
	}

	*bitmap_addr = (physical_address_t)(bitmap_start_addr & 0xffffffff);
	return 0;
	
}


static void
mark_range_as_usable(struct physical_memory_map* pmm, uint32_t base, uint32_t length)
{

	/* 
	 * if not freeing a whole page it is pointless 
	 * like this we avoid the case were base_page > end_page
	 */
	if (length < 4096) {
		return;
	}

	/* base index rounds upwards always, we can't free a page from its upper half only */
	uint32_t base_page = ((base+(4096-1))/4096);

	/* end index rounds downwards always */
	uint32_t end_page = (base+length)/4096;

	uint32_t base_bitmap_index = base_page/8;
	uint8_t base_bitmap_offset = base_page % 8;
	uint8_t base_mask = 0xff << (8-base_bitmap_offset);

	uint32_t end_bitmap_index = end_page/8;
	uint8_t end_bitmap_offset = (end_page % 8);
	uint8_t end_mask = 0xff >> end_bitmap_offset;

	if (base_bitmap_index == end_bitmap_index) {
		/* special case where they share index */
		/* */
		uint8_t final_mask = base_mask | end_mask;
		pmm->bitmap[base_bitmap_index] = pmm->bitmap[base_bitmap_index] & final_mask;
		return;

	}
	
	/* beginning and end */
	pmm->bitmap[base_bitmap_index] = pmm->bitmap[base_bitmap_index] & base_mask;
	pmm->bitmap[end_bitmap_index] = pmm->bitmap[end_bitmap_index] & end_mask;

	/* in between gaps */
	if (end_bitmap_index - base_bitmap_index >= 2) {
		// clear the easy area
		memset(0x00, (void*)(&pmm->bitmap[base_bitmap_index+1]), (end_bitmap_index - base_bitmap_index - 1));
	}
	
}

static void
mark_range_as_used(struct physical_memory_map* pmm, physical_address_t base, uint32_t length)
{

	/* base index rounds upwards always, we can't free a page from its upper half only */
	uint32_t base_page = base/4096;

	/* end index rounds downwards always */
	uint32_t end_page = ((base+length+4096-1)/4096);

	uint32_t base_bitmap_index = base_page/8;
	uint8_t base_bitmap_offset = base_page % 8;
	uint8_t base_mask = ~(0xff << (8-base_bitmap_offset));

	uint32_t end_bitmap_index = end_page/8;
	uint8_t end_bitmap_offset = (end_page % 8);
	uint8_t end_mask = ~(0xff >> end_bitmap_offset);

	if (base_bitmap_index == end_bitmap_index) {
		/* special case where they share index */
		/* */
		uint8_t final_mask = base_mask & end_mask;
		pmm->bitmap[base_bitmap_index] = pmm->bitmap[base_bitmap_index] | final_mask;
		return;

	}
	
	/* beginning and end */
	pmm->bitmap[base_bitmap_index] = pmm->bitmap[base_bitmap_index] | base_mask;
	pmm->bitmap[end_bitmap_index] = pmm->bitmap[end_bitmap_index] | end_mask;


	/* in between gaps */
	if (end_bitmap_index - base_bitmap_index >= 2) {
		// clear the easy area
		memset(0xff, (void*)(&pmm->bitmap[base_bitmap_index+1]), (end_bitmap_index - base_bitmap_index - 1));
	}
	
}

static void
mark_bitmap_usable_from_e820(struct boot_info* b_info, struct physical_memory_map* pmm)
{
	struct e820_frame_info* current_frame;
	for (int i = 0; i < b_info->n_entries; i++) {

		current_frame = &b_info->mmap_list_ptr[i];

		if (current_frame->type == MEM_USABLE) {
			/* why uint32_t addresses now? Because further is not addressable yet. Change on 64bit arch*/
			mark_range_as_usable(pmm, (uint32_t)current_frame->base, (uint32_t)current_frame->length);
		}

	}
}


void
arch_pmm_init(struct boot_info* b_info, struct physical_memory_map* pmm)
{
	// first of all we want to traverse ant look after the highest address
	// in order to calc the size of our bitmap
	
	uint64_t highest_address = 0;

	printk("[ PMM ] Calculating total system memory\n");

	highest_address = get_maximum_usable_mem_address_from_e820(b_info);

	printk("[ PMM ] Up to %l memory available\n", highest_address);

	// now highest_address contains the highest address implicit in the e820 list
	// we will consider it as the ram limit
	// now we calculate the bitmap size 
	// the formula is:
	// BM_SIZE_BYTES = ((RAM_SIZE / 4096) / 8) = RAM_SIZE / 32768
	// 4096 = PAGE_SIZE
	// 8 = BITS_PER_BYTE
	
	uint64_t bytes_to_alloc = (highest_address / 32768);
	bytes_to_alloc++; // to round up

	uint64_t pages_to_alloc = ( bytes_to_alloc / 4096 );
	pages_to_alloc++;

	// now we know how many pages we need in order to allocate the bitmap
	// we have to traverse the e820 frames and look for a free 
	// big enough space in memory. We also need to consider the bootstrap 
	// structures needed to be preserverd in memory ( gdt ? )

	physical_address_t bitmap_addr;

	uint8_t rc = get_free_mem_range_for_bitmap(b_info, bytes_to_alloc, &bitmap_addr);

	if (rc != 0) {
		printk("[ PMM ] Error: no range was found to place bitmap, not enough memory\n");
		/* panic ? */
		return;
	}

	// we need to convert this physical memory into a virtual address to be stored, we use map?
	// but to use vmm we need pmm first...
	// should this be done before da jump?

	printk("[ PMM ] Bitmap stored in %x-%x\n", bitmap_addr, bitmap_addr+bytes_to_alloc);
	
	pmm->bitmap = (uint8_t*)(uint32_t)bitmap_addr;
	pmm->bitmap_len = bytes_to_alloc;
	pmm->n_pages = pages_to_alloc;

	// set all to 1 meaning everything is used
	memset(0xff, (void*)pmm->bitmap, pmm->bitmap_len);

	// now we free all available/usable frames from e820
	mark_bitmap_usable_from_e820(b_info, pmm);

	// and finally mark as used again boot/e820_list/kernel/bitmap pages
	mark_range_as_used(pmm, (b_info->bootstrap_start), (b_info->bootstrap_end -  b_info->bootstrap_start));
	mark_range_as_used(pmm, (uint32_t)(b_info->mmap_list_ptr), (uint32_t)(b_info->n_entries*sizeof(struct e820_frame_info)));
	mark_range_as_used(pmm, (uint32_t)(&kernel_start), (uint32_t)((uint32_t)&kernel_end - (uint32_t)&kernel_start));
	mark_range_as_used(pmm, (uint32_t)(pmm->bitmap), (uint32_t)(pmm->bitmap_len));

	// and mark also page 0 as used for safety and coherency reasons
	pmm->bitmap[0] = pmm->bitmap[0] | 0x80;
}
