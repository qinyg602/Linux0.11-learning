#include    "linux/mm.h"
#include    "proto.h"
#include    "linux/sched.h"
#include    "asm/memory.h"
#include    "asm/system.h"
#include    "errno.h"
#include    "linux/ptrace.h"

long last_pid=0;
extern void ret_from_sys_call();
int find_empty_process();

//All the tasks in Linux 0.11 share onee page directory.Here I plant to follow the modern standard.
//Let each task has its own page directory.So each task can own the whole 4G virtual address.
int sys_fork(struct pt_regs regs)
{
    struct task_struct* p;
    struct  pt_regs *child_regs;
    unsigned long pgDir;

    int nr=find_empty_process();
    if(nr<0)
        return  -EAGAIN;
    p=(struct task_struct*)(__va(get_free_page()));
    if(!p)
        return -EAGAIN;
    //apply a page of memory for page directory.
    pgDir=get_free_page();
    if(!pgDir){
        free_page((unsigned long)p);
        return -EAGAIN;
    }
    *p=*current;
    p->pid=last_pid;
    p->father=current->pid;
    p->counter=current->counter;
    p->priority=current->priority;
    p->state=TASK_RUNNING;
    child_regs=((struct pt_regs*)((unsigned long)p+PAGE_SIZE))-1;
    *child_regs=regs;
    child_regs->eax=0;
    p->esp=(long)child_regs;
    p->eip=(unsigned long)ret_from_sys_call;

    //now this task has its own page directory.
    p->cr3=(long)pgDir;
    if(copy_page_tables(p))
    {
        free_page(pgDir);
        free_page(__pa((unsigned long)p));
        tasks[nr]=NULL;
        return -EAGAIN;
    }
    tasks[nr]=p;
    //set_ldt_desc((char*)(gdt+FIRST_LDT_ENTRY+nr),&(p->ldt));
    return last_pid;
}
int find_empty_process()
{
    int found_pid=0;
    while(!found_pid)
    {
        if(++last_pid<0) last_pid=1;
        found_pid=1;
        for(int i=1;i<NR_TASKS;i++)
        {
            if(tasks[i] && tasks[i]->pid==last_pid)
            {
                found_pid=0;
                break;
            }
        }
    }

    for(int i=1;i<NR_TASKS;i++)
        if(!tasks[i])
            return i;
    return -EAGAIN;
}
