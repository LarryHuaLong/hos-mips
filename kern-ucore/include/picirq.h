#ifndef __KERN_DRIVER_PICIRQ_H__
#define __KERN_DRIVER_PICIRQ_H__

void pic_init(void);
void pic_enable(unsigned int irq);
void pic_disable(unsigned int irq);
void xilinx_intc_init(void); // added by xiaohan for simplicity. 
							 // When EIC interrupt are handled, I simply re-initialize
							 // EIC to avoid writing codes that tell EIC that 
							 // its interrupt requested has been handled.
int get_eic_cause();// added by hualong to get cause when EIC interrupt are 
int get_eic_pending();
void eic_acknowledge(int n);// added by hualong to acknowledge EIC interrupt

#define IRQ_OFFSET      32

#endif /* !__KERN_DRIVER_PICIRQ_H__ */
