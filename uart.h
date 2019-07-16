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
 * FUNCTION PROTOTYPES
 ****************************************************/
void UART1_init(uint16_t baudrate);
void UART2_init();
void enable_UART();
void disable_UART();
void disable_UART1();
void UART_init();

void UART1_ISR();
void UART2_ISR();

extern void add_event(uint8_t event,uint8_t zone);
extern void out_sbuf2(uint8_t tmp);
extern uint8_t check_ID(uint8_t *ptr);
extern uint8_t add_ID(uint8_t *ptr);
extern uint8_t del_ID(uint8_t id);

/*****************************************************
 * VARIABLES
 ****************************************************/
uint8_t 	rx2_cnt = 0;
uint8_t 	rx2_buf[20];

extern  uint8_t led_count;

#endif	/* UART_H */

