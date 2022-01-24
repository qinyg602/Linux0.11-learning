#ifndef SYS_H_INCLUDED
#define SYS_H_INCLUDED
extern int sys_setup();
extern int sys_exit();
extern int sys_fork();


typedef int (*fn_ptr)();

fn_ptr sys_calls_table[]={sys_setup,sys_exit,sys_fork,};
#endif // SYS_H_INCLUDED
