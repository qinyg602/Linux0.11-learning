#include <errno.h>
#include <linux/mm.h>
#include <linux/sched.h>

void release(struct tasks *p)
{
    int i;
    if(!p)  return;
    for(i=i;i<NR_TASKS;i++){
        if(tasks[i]==p){
            free_page(tasks[i]->cr3);
            free_page(__pa((unsigned long)tasks[i]));
            tasks[i]=NULL;
            return;
        }
    }
}
int do_exit(long code)
{
    free_page_tables(current->cr3);
    for(int i=1;i<NR_TASKS;i++){
        if(tasks[i] && tasks[i]->father==current->pid){
            tasks[i]->father=0;
            break;
        }
    }
    current->state=TASK_ZOMBIE;
    schedule();
    return 0;
}
int sys_exit(int error_code)
{
    return do_exit((error_code & 0xff)<<8);
}
