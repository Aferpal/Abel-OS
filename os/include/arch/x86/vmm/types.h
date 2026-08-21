#ifndef _X86_VMM_TYPES_H_
#define _X86_VMM_TYPES_H_

#include "stdint.h"

#define X86_PAGE_FLAG_PRESENT 0x0001
#define X86_PAGE_FLAG_RW      0x0002
#define X86_PAGE_FLAG_U_PERM  0x0004
#define X86_PAGE_FLAG_PWT     0x0008
#define X86_PAGE_FLAG_PCD     0x0010
#define X86_PAGE_FLAG_ACC     0x0020
#define X86_PAGE_FLAG_DIRTY   0x0040
#define X86_PAGE_FLAG_4M      0x0080
#define X86_PAGE_FLAG_GLOBAL  0x0100
#define X86_PAGE_FLAG_PAT     0x1000

#define X86_PAGE_FLAG_KERNEL  (X86_PAGE_FLAG_PRESENT | \
                               X86_PAGE_FLAG_RW | \
			       X86_PAGE_FLAG_U_PERM | \
			       X86_PAGE_FLAG_GLOBAL )

#define X86_PAGE_FLAG_USER    (X86_PAGE_FLAG_PRESENT | \
                               X86_PAGE_FLAG_RW )


#define X86_VIRT_ADDR_PDI_MASK  0xffc00000
#define X86_VIRT_ADDR_PDI_RSHFT 22
#define X86_VIRT_ADDR_PTI_MASK  0x003ff000
#define X86_VIRT_ADDR_PTI_RSHFT 12

#define X86_PDE_PT_MASK  0xfffff000

struct page_table_entry {
	ptr_t value;	
};

struct page_table {
	struct page_table_entry ptes[1024];
};

struct page_directory_entry {
	ptr_t value;
};

struct page_directory {
	struct page_directory_entry pdes[1024];
};


struct vmm_addr_space {
	struct page_directory pd;
};

#endif

