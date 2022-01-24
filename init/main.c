#include <proto.h>
#include <linux/sched.h>
#include <asm/system.h>
#include <asm/io.h>
#include <asm/memory.h>
#include <unistd.h>
#include <linux/sys.h>

static inline syscall0(int,fork)
static inline syscall1(int,setup,void *,BIOS)
static inline syscall1(int,exit,int,error_code)

extern void mem_init(long start_mem ,long end_mem);
void init();

struct __drive_info{char dummy[32];} drive_info;
//#define EXT_MEM_K (*(unsigned short *)0x90002)
#define MEM_SIZE (*(unsigned long *)0x9000e)
#define DRIVE_INFO (*(struct __drive_info *)0x90080)
#define ORIG_ROOT_DEV (*(unsigned short *)0x901FC)

static long memory_end=0;
static long buffer_memory_end=0;
static long main_memory_start=0;

void qmain()
{
    int child;
	//memory_end=(1<<20)+(EXT_MEM_K<<10);
	memory_end=MEM_SIZE & 0xfffff000;
	if(memory_end>16*1024*1024)
		memory_end=16*1024*1024;
	if(memory_end>12*1024*1024)
		buffer_memory_end=4*1024*1024;
	else if(memory_end>6*1024*1024)
		buffer_memory_end=2*1024*1024;
	else
        	buffer_memory_end=1024*1024;
    	main_memory_start=buffer_memory_end;
	drive_info=DRIVE_INFO;
	clear();
	disp_str("A new system is running!");
	mem_init(main_memory_start,memory_end);
	trap_init();
    sched_init();
    hd_init();
	sti();
   	move_to_user_mode();
	disp_str("\nIn user mode!");

	child=fork();
	if(!child){
        disp_str("in child task!...");
        init();
	}
	while(1){}
}

void init()
{
    setup(&drive_info);
}
