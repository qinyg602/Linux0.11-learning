#ifndef _LINUX_PTRACE_H
#define _LINUX_PTRACE_H

	struct	pt_regs{
		long	ebx;
		long	ecx;
		long	edx;
		long	esi;
		long	edi;
		long	ebp;
		long	eax;
		unsigned short	ds,_dsu;
		unsigned short	es,_esu;
		unsigned short	fs,_fsu;
		unsigned short	gs,_gsu;
		long	eip;
		unsigned short	cs,_csu;
		long	eflag;
		long	esp;
		unsigned short	ss,_ssu;
	};

#endif
