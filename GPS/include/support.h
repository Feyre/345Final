// support.h contains some helper macros that may be userful for port writing

#ifndef _SUPPORT_H
#define _SUPPORT_H

#include <avr/io.h> 
#include <util/delay.h> 

// Helpful Macros
#define SET_INPUT(portdir, pin)			portdir &= ~(1 << pin) 
#define SET_OUTPUT(portdir, pin)		portdir |= (1 << pin)
#define OUTPUT_WRITE(port, pin, value)	port = (port & ~(1 << pin)) | (value << pin)
#define OUTPUT_LOW(port, pin)			port &= ~(1 << pin)
#define OUTPUT_HIGH(port, pin)			port |= (1 << pin)
#define READ_LINE(port, pin)			port & (1 << pin)

#endif