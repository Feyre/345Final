// Header file for Interupts & CPU Timing etc.

#ifndef INTERRUPT_H_
#define INTERRUPT_H_

#define F_CPU 16000000UL
#define CPU_PRESCALE(n) (CLKPR = 0x80, CLKPR = (n))
#define CPU_8MHz 0x01

#endif /* INTERRUPT_H_ */