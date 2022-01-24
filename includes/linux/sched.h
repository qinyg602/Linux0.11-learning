#ifndef	_SCHED_H
#define _SCHED_H
#define NR_TASKS 64
#define	FIRST_TASK	task[0]
#define	LAST_TASK	task[NR_TASKS-1]
#define TSS_ENTRY   6
#define FIRST_LDT_ENTRY   (TSS_ENTRY+1)
#define HZ 100

#include <linux/head.h>
#include <linux/fs.h>

#define TASK_RUNNING    0
#define TASK_INTERRUPTIBLE  1
#define TASK_UNINTERRUPTIBLE    2
#define TASK_ZOMBIE 3
#define TASK_STOPPED    4

struct i387_struct{
    long cwd;
    long swd;
    long twd;
    long fip;
    long fcs;
    long foo;
    long fos;
    long st_space[20];
};

struct tss_struct {
	long	back_link;	/* 16 high bits zero */
	long	esp0;
	long	ss0;		/* 16 high bits zero */
	long	esp1;
	long	ss1;		/* 16 high bits zero */
	long	esp2;
	long	ss2;		/* 16 high bits zero */
	long	cr3;
	long	eip;
	long	eflags;
	long	eax,ecx,edx,ebx;
	long	esp;
	long	ebp;
	long	esi;
	long	edi;
	long	es;		/* 16 high bits zero */
	long	cs;		/* 16 high bits zero */
	long	ss;		/* 16 high bits zero */
	long	ds;		/* 16 high bits zero */
	long	fs;		/* 16 high bits zero */
	long	gs;		/* 16 high bits zero */
	long	ldt;		/* 16 high bits zero */
	long	trace_bitmap;	/* bits: trace 0, bitmap 16-31 */
	struct i387_struct i387;
};

struct task_struct{
	long state;	/* -1 unrunnable, 0 runnable, >0 stopped */
	long counter;
	long priority;
	long pid,father;
	long cr3;
	long eip;
	long esp;
	struct desc_struct ldt[3];
};
#define NESP    (4*6)
#define OESP    (4*6+4*16)
#define _TSS (TSS_ENTRY<<3)
#define _LDT(n) ((((unsigned long)n)<<3)+(FIRST_LDT_ENTRY<<3))

#define ltr  __asm__("ltr %%ax"::"a"(_TSS))
#define lldt(n)  __asm__("lldt %%ax"::"a"(_LDT(n)))

#define INIT_TASK \
/* state etc */	{ TASK_RUNNING,5,5,0,0,0x1000,0,0,\
	{		{0,0}, \
/* ldt */	{0,0}, \
		{0,0} \
	} \
}
extern struct task_struct* tasks[NR_TASKS];
extern struct task_struct* current;
extern void sched_init(void);
extern void trap_init(void);
extern void hd_init();
extern void schedule();
extern struct tss_struct tss;
#define switch_to(n) ({ \
    __asm__("cmpl %%ecx,current\n\t" \
            "je 2f\n\t" \
            "pushl  %%eax\n\t"  \
            "pushl  %%ebx\n\t"  \
            "pushl  %%edx\n\t"  \
            "pushl  %%esi\n\t"  \
            "pushl  %%edi\n\t"  \
            "pushl  %%ebp\n\t"  \
            "pushfl\n\t"    \
            "movl   %%esp,%[old_esp]\n\t"  \
            "movl   $1f,%[old_eip]\n\t"   \
            "movl   %[new_esp],%%esp\n\t"   \
            "pushl  %[new_eip]\n\t" \
            "movl   %[new_cr3],%%eax\n\t"    \
            "movl   %%eax,%%cr3\n\t"    \
            "xchgl  %%ecx,current\n\t"  \
            "movl   current,%%edx\n\t"    \
            "addl   $4096,%%edx\n\t"  \
            "movl   %%edx,%[esp0]\n\t"   \
            "ret\n\t" \
            "1:"\
            "popfl\n\t" \
            "popl   %%ebp\n\t"  \
            "popl  %%edi\n\t"  \
            "popl  %%esi\n\t"  \
            "popl  %%edx\n\t"  \
            "popl  %%ebx\n\t"  \
            "popl  %%eax\n\t"  \
			"2:"	\
    :[old_esp]"=m"(current->esp),[old_eip]"=m"(current->eip),[esp0]"=m"(tss.esp0) \
    :[new_cr3]"m"(tasks[n]->cr3),[new_esp]"m"(tasks[n]->esp),[new_eip]"m"(tasks[n]->eip),"c"((long)tasks[n]));\
})

static inline unsigned long get_base(char* addr)
{
    unsigned long __base;
    __asm__("movb   %3,%%dh\n\t"
            "movb   %2,%%dl\n\t"
            "shll   $16,%%edx\n\t"
            "movw   %1,%%dx\n\t"
            :"=&d"(__base)
            :"m"(*((addr)+2)),
            "m"(*((addr)+4)),
            "m"(*((addr)+7)));
    return __base;
}

#define get_limit(segment)  ({ \
    unsigned long __limit; \
    __asm__("lsll %1,%0\n\tincl %0":"=r"(__limit):"r"(segment)); \
    __limit;})

#include "linux/mm.h"
#endif
