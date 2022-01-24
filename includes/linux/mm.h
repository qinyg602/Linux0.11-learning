#ifndef _MM_H
#define	_MM_H
#include    "linux/sched.h"
#define	PAGE_SIZE	4096
#define PAGE_OFFSET 0xC0000000
#define __va(addr)  ((addr)+PAGE_OFFSET)
#define __pa(addr)  ((addr)-PAGE_OFFSET)
extern unsigned long get_free_page(void);
extern void free_page(unsigned long addr);
extern int free_page_tables(unsigned long dir);
extern int copy_page_tables(struct task_struct* tsk);
#endif
