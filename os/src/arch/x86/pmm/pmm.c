#include <arch/pmm_hal.h>
#include <arch/x86/boot/boot_info.h>
#include <kernel/printk.h>

extern uint8_t* kernel_start;
extern uint8_t* kernel_end;

void
arch_pmm_init(struct boot_info* b_info, struct physical_memory_map* pmm)
{
	// first of all we want to traverse ant look after the highest address
	// in order to calc the size of our bitmap
	
	uint64_t highest_address = 0;
    uint64_t local_top_address = 0;

	struct e820_frame_info* current_frame = 0;

	printk("[ PMM ] Calculating total system memory\n");

	for (int i = 0; i < b_info->n_entries; i++) {
		current_frame = &(b_info->mmap_list_ptr[i]);
		local_top_address = current_frame->base + current_frame->length;

		if (local_top_address > highest_address && current_frame->type == MEM_USABLE) {
			highest_address = local_top_address;
		}
	}

	printk("[ PMM ] Up to %x:%x memory available\n", (uint32_t)(highest_address >> 32), (uint32_t)highest_address);

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
	
	uint64_t bootstrap_start = (uint64_t)b_info->bootstrap_start;
	uint64_t bootstrap_end = (uint64_t)b_info->bootstrap_end;


	uint64_t e820_table_start = (uint64_t) (uint32_t) b_info->mmap_list_ptr;
	uint64_t e820_table_end = (e820_table_start + b_info->n_entries * sizeof(struct e820_frame_info));

	uint64_t kernel_64b_start = (uint64_t) (uint32_t) &kernel_start;
	uint64_t kernel_64b_end = (uint64_t) (uint32_t) &kernel_end;

	uint64_t current_frame_end;

	uint8_t collides_w_820 = 0;
	uint8_t collides_w_boot = 0;
	uint8_t collides_w_kernel = 0;

	printk("[ PMM ] PMM bitmap will need %d bytes, looking for a frame where to store\n", (uint32_t)bytes_to_alloc);

	for (int i = 0; i < b_info->n_entries; i++) {
		collides_w_820 = 0;
		collides_w_boot = 0;

		current_frame = &(b_info->mmap_list_ptr[i]);

		if (current_frame->type != MEM_USABLE || current_frame->length < bytes_to_alloc) {
			continue;
		}

		/* CURRENT FRAME IS BIG ENOUGH AND USABLE */
		current_frame_end = current_frame->base + current_frame->length;
		/* IS IT THE FRAME WHERE THIS TABLE OR THE BOOTSTRAP IS LOCATED ? */
        if ( (e820_table_start >= current_frame->base && e820_table_start <= current_frame_end) ||
		      (e820_table_end >= current_frame->base && e820_table_end <= current_frame_end) ) {
			collides_w_820 = 1;
		}

		if ( (bootstrap_start >= current_frame->base && bootstrap_start <= current_frame_end) ||
		      (bootstrap_end >= current_frame->base && bootstrap_end <= current_frame_end) ) {
			collides_w_boot = 1;
		}

		if ( (kernel_64b_start >= current_frame->base && kernel_64b_start <= current_frame_end) ||
		      (kernel_64b_end >= current_frame->base && kernel_64b_end <= current_frame_end) ) {
			collides_w_kernel = 1;
		}

	}

	pmm->bitmap_len = bytes_to_alloc;
	pmm->n_pages = pages_to_alloc;
}
