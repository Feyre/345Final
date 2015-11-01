/*
 * UART.h
 *
 * Created: 30/06/2014 12:41:48 PM
 *  Author: James
 */ 
 
#ifndef UART_H_
#define UART_H_

//INCLUDE FILES
#include <avr/io.h>
#include <stdint.h>


//CONSTANTS
#define F_CPU 16000000UL
// #define BAUDRATE 9600 //Baud rate that we want
#define BAUDRATE 19200 //Baud rate that we want
#define BAUD ((F_CPU)/(BAUDRATE*16UL)-1) //only works for clock prescaler of 0


//FUNCTIONS
void UART_Init(void);
void UART_TransmitChar(char data);
char UART_Receive(void);
void UART_Flush_Data(void);


#endif /* UART_H_ */