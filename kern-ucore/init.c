#include <defs.h>
#include <stdio.h>
#include <string.h>
#include <console.h>
#include <kdebug.h>
#include <picirq.h>
#include <trap.h>
#include <clock.h>
#include <intr.h>
#include <pmm.h>
#include <vmm.h>
#include <proc.h>
#include <thumips_tlb.h>
#include <sched.h>

void __noreturn kern_init(void)
{
	tlb_invalidate_all();
	char *p = 0x7ffff000;
	mp_init();
	pic_init();		// init interrupt controller
	cons_init();		// init the console
	clock_init();		// init clock interrupt

	check_initrd();

	const char *message = "(THU.CST) os is loading ...\n\r\n\r";
	kprintf(message);
	print_kerninfo();

#if 0
	kprintf("EX\n\r");
	__asm__ volatile ("syscall");
	kprintf("EX RET\n\r");
#endif

	pmm_init();		// init physical memory management

/* SZY comments: heavy commented here.*/
	vmm_init();		// init virtual memory management

	sched_init();
	proc_init();		// init process table
	ide_init();
	fs_init();
	intr_enable();		// enable irq interrupt
	cpu_idle();
}
