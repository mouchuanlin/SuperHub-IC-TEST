//
// uart.h
//

#ifndef UART_H
#define	UART_H

/*****************************************************
 * INCLUDES
 ****************************************************/


/*****************************************************
 * VARIABLES
 ****************************************************/



/*****************************************************
 * FUNCTION PROTOTYPES
 ****************************************************/
void UART1_init();
void UART2_init();
void Enable_Uart_IO();
void Uart_disable();
void UART_init();

void UART1_ISR();
void UART2_ISR();

#endif	/* UART_H */

