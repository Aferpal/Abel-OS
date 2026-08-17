#include "kernel/kalloc/kalloc.h"
#include "kernel/pmm/pmm.h"

#define MIN_ALLOCATION_SIZE 16

struct kobject_header* f_list = 0;

static void
kobject_free_init(struct kobject_header* node, uint32_t k_size)
{
	node->size = (k_size - sizeof(struct kobject_header));
	node->next = 0;
}

/*
 * kalloc
 *
 * current limitations: unable to alloc more than 4096 - sizeof(struct kobject_header) bytes
 * waiting for implementation of pmm_alloc_pages(n)
 */
void*
kalloc(uint32_t n)
{
	if (n < MIN_ALLOCATION_SIZE) {
		n = MIN_ALLOCATION_SIZE;
	}

	if ((n % 2) != 0) {
		n++;
	}

	if ((n + sizeof(struct kobject_header)) > 4096) {
		return 0;
	}

	// normalize, we always want to alloc an even amount of mem
	// in order to use our super advanced system of occ/free distinction
	if (f_list == 0) {
		f_list = (struct kobject_header*)pmm_alloc_page();
		kobject_free_init(f_list, 4096);
	}

	uint8_t found = 0;
	struct kobject_header* current = f_list;
	struct kobject_header* prev = 0;
	while (!found) {
		if (current->size >= (n + sizeof(struct kobject_header))) {
			found = 1;
		} else if (current->next) {
			prev = current;
			current = (struct kobject_header*)current->next;
		} else {
			current = pmm_alloc_page();
			kobject_free_init(current, 4096);
			
			struct kobject_header* tmp = f_list;
			prev = 0;

			while (tmp != 0 && tmp < current) {
				prev = tmp;
				tmp = (struct kobject_header*)tmp->next;
			}
			
			if (tmp == 0) {
				prev->next = (uint32_t)current;
			} else if (prev == 0) {
				current->next = (uint32_t)f_list;
				f_list = current;
			} else {
				current->next = prev->next;
				prev->next = (uint32_t)current;
			}
			found = 1;

		}
	}

	// would this create an unusable fragment? then allocate fully and don't fragment
	if (current->size < (n + sizeof(struct kobject_header) + MIN_ALLOCATION_SIZE)) {
		n = current->size;
		if (prev == 0) {
			f_list = (struct kobject_header*)current->next;
		} else {
			prev->next = current->next;
		}
	} else {

		struct kobject_header* cpy = (struct kobject_header*) ((uint8_t*)current + n + sizeof(struct kobject_header));
		cpy->size = (current->size - n - sizeof(struct kobject_header));
		cpy->next = current->next;

		if (prev == 0) {
			f_list = cpy;
		} else {
			prev->next = (uint32_t)cpy;
		}
	}

	current->size = n;
	current->props = KOBJECT_HEADER_USED; // to indicate it is occupied
	
	return (void*)((uint32_t)current + sizeof(struct kobject_header)); 	
}

/*
 * k_try_collide
 * pre condition, h1 and h2 are valid free headers
 *
 */
static void
k_try_collide(struct kobject_header* h1, struct kobject_header* h2)
{
	if (h1 == 0 || h2 == 0) {
		return;
	}

	if ((h1->size + (uint32_t)h1 + sizeof(struct kobject_header)) == (uint32_t)h2 &&
	    pmm_get_page(h1) == pmm_get_page(h2)) {
		h1->size = h1->size + h2->size + sizeof(struct kobject_header);
		h1->next = h2->next;
	}
}

void
kfree(void* ptr)
{
	if (ptr == 0) {
		return;
	}
	
	struct kobject_header* hdr = ((struct kobject_header*)ptr) - 1;

	// first mecanism to avoid miss freeing
	if ((hdr->props & 0x01) != KOBJECT_HEADER_USED) {
		return;
	}

	struct kobject_header* current = f_list;
	struct kobject_header* prev = 0;
	// sus
	while (current != 0 && current < hdr) {
		prev = current;
		current = (struct kobject_header*)current->next;
	}

	hdr->props = KOBJECT_HEADER_FREE;
	
	if (prev == 0) {
		hdr->next = (uint32_t)f_list;
		f_list = hdr;
	} else {
		prev->next = (uint32_t)hdr;
		hdr->next = (uint32_t)current;
	}

	
	k_try_collide(hdr, (struct kobject_header*)hdr->next);

	if (hdr->size == (4096 - sizeof(struct kobject_header))) {
		if (prev == 0) {
			f_list = (struct kobject_header*)hdr->next;
		} else {
			prev->next = hdr->next;
		}
		pmm_free_page(hdr);
	} else if (prev != 0) {
		k_try_collide(prev, hdr);
		if (prev->size == (4096 - sizeof(struct kobject_header))) {
        		struct kobject_header* temp = f_list;
			while (temp != 0 && temp->next != (uint32_t)prev) {
				temp = (struct kobject_header*)temp->next;
			}

			if (temp) {
				temp->next = prev->next;
			}
			pmm_free_page(prev);
		}
	}


}
