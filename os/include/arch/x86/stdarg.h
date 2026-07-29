#ifndef _STD_ARG_H_
#define _STD_ARG_H_

typedef void* va_list;

#define va_start(vl, last) \
	(vl = (void*)&(last) + sizeof(last))

#define va_arg(vl, type) \
	(*(type*)((vl += sizeof(type)) - sizeof(type)))

#define va_end(vl) \
	(vl = (void*)0)

#endif
