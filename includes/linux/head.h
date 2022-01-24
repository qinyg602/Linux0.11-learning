#ifndef	_HEAD_H
#define	_HEAD_H
	typedef struct desc_struct{
		unsigned long low;
		 unsigned long high;
	} desc_table[256];

	extern desc_table idt,gdt;
	extern unsigned long pg_dir[1024];

#endif
