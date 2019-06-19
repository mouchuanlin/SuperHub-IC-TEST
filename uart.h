//
// uart.h
//

#ifndef UART_H
#define	UART_H

/*****************************************************
 * INCLUDES
 ****************************************************/
#include "stdint.h"
#include "stdbool.h"

/*****************************************************
 * VARIABLES
 ****************************************************/
uint8_t 	rx2_cnt = 0;
uint8_t 	rx2_buf[20];

extern  uint8_t led_count;

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

extern void add_event(uint8_t event,uint8_t zone);
extern void out_sbuf2(uint8_t tmp);
extern uint8_t check_ID(uint8_t *ptr);
extern uint8_t add_ID(uint8_t *ptr);
extern uint8_t del_ID(uint8_t id);

#endif	/* UART_H */

