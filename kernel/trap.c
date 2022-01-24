#include "proto.h"
#include "asm/system.h"
#include "linux/head.h"
#include "asm/io.h"

void divide_error();
void debug();
void nmi();
void int3();
void overflow();
void bounds();
void invalid_op();
void device_not_available();
void double_fault();
void coprocessor_segment_overrun();
void invalid_TSS();
void segment_not_present();
void stack_segment();
void general_protection();
void page_fault();
void coprocessor_error();
void reserved();
void clock();
void keyboard();
//void parallel_interrupt()
//void irq13();

void die(char* str,unsigned long esp_ptr,unsigned long error_code)
{
    long *esp=(long *)esp_ptr;
	disp_str(str);
	disp_int(esp[0]);

}

void do_divide_error(unsigned long esp,unsigned long error_code)
{
	die("\ndivided error!",esp,error_code);
}

void do_debug(unsigned long esp,long unsigned error_code)
{
	die("\ndebug!",esp,error_code);
}

void do_nmi(unsigned long esp,unsigned long error_code)
{
	die("\nnmi!",esp,error_code);
}

void do_int3(unsigned long esp,unsigned long error_code)
{
	die("\nint3!",esp,error_code);
}

void do_overflow(unsigned long esp,unsigned long error_code)
{
	die("\noverflow!",esp,error_code);
}

void do_bounds(unsigned long esp,unsigned long error_code)
{
	die("\nbounce!",esp,error_code);
}

void do_invalid_op(unsigned long esp,unsigned long error_code)
{
	die("\ninvalid_OP!",esp,error_code);
}

void do_device_not_available(unsigned long esp,unsigned long error_code)
{
	die("\ndevice_not_available!",esp,error_code);
}

void do_double_fault(unsigned long esp,unsigned long error_code)
{
	die("\ndouble_fault!",esp,error_code);
}

void do_coprocessor_segment_overrun(unsigned long esp,unsigned long error_code)
{
	die("\ncoprocessor_segment_overrun!",esp,error_code);
}

void do_invalid_TSS(unsigned long esp,unsigned long error_code)
{
	die("\ninvalid_TSS!",esp,error_code);
}

void do_segment_not_present(unsigned long esp,unsigned long error_code)
{
	die("\nsegment_not_present!",esp,error_code);
}

void do_stack_segment(unsigned long esp,unsigned long error_code)
{
	die("\nstack_segment!",esp,error_code);
}

void do_general_protection(unsigned long esp,unsigned long error_code)
{
	die("\ngeneral_protection!",esp,error_code);
}
/*
void do_page_fault()
{
	die("\npage_fault!");
}*/

void do_coprocessor_error(unsigned long esp,unsigned long error_code)
{
	die("\ncoprocessor_error!",esp,error_code);
}

/*void do_parallevel_interrupt(){
	die("\nparallevel_interrupt!");
}

void do_irq13()
{
	die("\nirq13!");
}*/

void do_reserved(unsigned long esp,unsigned long error_code)
{
	die("\nReserved!",esp,error_code);
}
/*void do_clock()
{
    die("\nclock!",esp,error_code);
}*/
void do_keyboard()
{
    disp_str("\nkeyboard!");
}

void trap_init()
{
    int i;

    // 设置除操作出错的中断向量值。
	set_trap_gate(0,divide_error);
	set_trap_gate(1,debug);
	set_trap_gate(2,nmi);
	set_system_gate(3,int3);	/* int3-5 can be called from all */
	set_system_gate(4,overflow);
	set_system_gate(5,bounds);
	set_trap_gate(6,invalid_op);
	set_trap_gate(7,device_not_available);
	set_trap_gate(8,double_fault);
	set_trap_gate(9,coprocessor_segment_overrun);
	set_trap_gate(10,invalid_TSS);
	set_trap_gate(11,segment_not_present);
	set_trap_gate(12,stack_segment);
	set_trap_gate(13,general_protection);
	set_trap_gate(14,page_fault);
	set_trap_gate(15,reserved);
	set_trap_gate(16,coprocessor_error);
    // 下面把int17-47的陷阱门先均设置为reserved,以后各硬件初始化时会重新设置自己的陷阱门。
	for (i=17;i<48;i++)
		set_trap_gate(i,reserved);
    set_intr_gate(33,keyboard);
	//outb_p(inb_p(0x21)&~0x02,0x21);
}
