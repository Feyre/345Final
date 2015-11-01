#include <avr/io.h>
#include <avr/interrupt.h>
#include "UART.h"

void UART_Init(void)
{
	// Set Baud Rate to 9600 using Prescaler of 0, using the UBRR1 registers
	// UBRR1 = ((F_CPU)/(BAUDRATE*16UL)-1).
	// Hint: Use the defined BAUD value in the UART.h file
	/***** YOUR CODE HERE *****/
	
 	UBRR1 = BAUD;
	
	// Enable Receiver and Transmitter for UART using UCSR1B register
	/***** YOUR CODE HERE *****/
 	UCSR1B |= (1<<TXEN1)|(1<<RXEN1);
	
	
	// Set the Frame Format to - Asynchronous USART, 8 Data Bits, 1 Stop Bits, No Parity
	// This is done using the UCSR1C register
	/***** YOUR CODE HERE *****/
 	UCSR1C |= (1<<UCSZ11)|(1<<UCSZ10);
}

void UART_TransmitChar(char data)
{
	//Wait until transmit buffer is empty
	while(!(UCSR1A & (1<<UDRE1)));
	
	//Put data into the transmit buffer, UDR1
	/***** YOUR CODE HERE *****/
	 UDR1 = data;
}

char UART_Receive(void)
{
	//Wait for data to be received
	while(!(UCSR1A & (1<<RXC1)));
	
	//Return the received data from buffer, UDR0
	/***** YOUR CODE HERE *****/
 	return UDR1;
}

void UART_Flush_Data(void)
{
	char temp = 0;
	//Wait for data to be received
	while(!(UCSR1A & (1<<RXC1)))
	{
		temp = UDR1;
	}
}
