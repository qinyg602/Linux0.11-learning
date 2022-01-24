#include "linux/sched.h"
#include "linux/mm.h"
#include "asm/system.h"
#include "asm/io.h"
#include "proto.h"
#include  "linux/segment.h"
#include "asm/memory.h"
union task_union{
    struct task_struct task;
	char	stack[PAGE_SIZE];
};
static union task_union init_task={INIT_TASK,};

struct task_struct* tasks[NR_TASKS]={&init_task.task,};
long user_stack[PAGE_SIZE>>2];
long user_stack_top=(long)(&user_stack[PAGE_SIZE>>2]);


struct task_struct* current=&init_task.task;
struct tss_struct tss;

extern int timer_interrupt();
extern int system_call();

#define LATCH (1193180/HZ)
void sched_init()
{
    struct desc_struct *p;

    set_tss_desc((char*)(gdt+TSS_ENTRY),(unsigned long)&tss);
    //set_ldt_desc((char*)(gdt+FIRST_LDT_ENTRY),(unsigned long)&init_task.task.ldt);

    p=(struct desc_struct*)(gdt+1+TSS_ENTRY);
    for(int i=1;i<NR_TASKS;i++){
        tasks[i]=NULL;
        p->low=p->high=0;
        p++;
    }
    __asm__("pushfl;andl $0xffffbfff,(%esp);popfl");
    tss.ss0=KERNEL_DS;
    tss.esp0=(long)&init_task.task+PAGE_SIZE;
    ltr;
    //lldt(0);
    outb_p(0x36,0x43);
    outb_p(LATCH & 0xff,0x40);
    outb_p(LATCH >>8,0x40);
    set_intr_gate(0x20,timer_interrupt);
    set_system_gate(0x80,system_call);
	outb(inb_p(0x21)&~0x01,0x21);

}

static int nTask=0;
void schedule()
{
   while(1)
    {
        nTask++;
        if(nTask>=NR_TASKS) nTask=0;
        if(tasks[nTask] && tasks[nTask]->state==TASK_RUNNING) break;
    }
    switch_to(nTask);
}

void do_timer(long cpl)
{
    /*if(current==tasks[0]) schedule();
    if(--current->counter>0) return ;
    current->counter=current->priority;
    if(!cpl)    return;*/
    schedule();
}
