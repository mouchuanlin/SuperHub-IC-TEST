//
// uart.h
//

#ifndef UART_H
#define	UART_H

/*****************************************************
 * INCLUDES
 ****************************************************/
#include "inc.h"

/*****************************************************
 * FUNCTION PROTOTYPES
 ****************************************************/
void UART1_init(uint32_t baudrate);
void UART2_init(void);
void enable_UART(void);
void disable_UART(void);
void disable_UART1(void);
void UART_init(void);

void UART1_ISR(void);
void UART2_ISR(void);
void process_RF_data(void);
void decode_device_id(uint8_t id[]);
void ACK_to_RF_receiver(void);
void send_sensor_alarm(uint8_t zone, uint8_t id[]);

/*****************************************************
 * VARIABLES
 ****************************************************/
// The Carriage Return (CR) character (0x0D, \r) moves the cursor to the beginning of the line without advancing to the next line. 
// The Line Feed (LF) character (0x0A, \n) moves the cursor down to the next line without returning to the beginning of the line.
#define CR 		0x0D    // \r
#define LF 		0x0A    // \n

#define MAX_RX2_BUF_SIZE    20
uint8_t 	rx2_cnt = 0;
uint8_t 	rx2_buf[MAX_RX2_BUF_SIZE];

extern  uint8_t led_count;

#endif	/* UART_H */

