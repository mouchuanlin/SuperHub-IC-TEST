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

uint8_t hex_to_ascii(uint8_t hex);
bool    is_valid_rf_data(void);

/*****************************************************
 * VARIABLES
 ****************************************************/
// The Carriage Return (CR) character (0x0D, \r) moves the cursor to the beginning of the line without advancing to the next line. 
// The Line Feed (LF) character (0x0A, \n) moves the cursor down to the next line without returning to the beginning of the line.
#define CR 		0x0D    // \r
#define LF 		0x0A    // \n


// 7 bytes RF data in HEX - $ + 3byte ID + 1byte status + <CR> + <LF>
//#define MAX_RX2_BUF_SIZE    20
#define MAX_RX2_BUF_SIZE    7
uint8_t 	rx2_cnt = 0;

typedef struct rf_data {
    uint8_t dollar;
    uint8_t rf_id[3];
    uint8_t status;
    uint8_t cr;
    uint8_t lf;
} rf_data_t;

union rx2 {
    uint8_t     data[MAX_RX2_BUF_SIZE];
    rf_data_t   map;
} rx2_buf;



// UART 1 buffer
#define MAX_RX1_BUF_SIZE    50
uint8_t rx1_buf[MAX_RX1_BUF_SIZE];
uint8_t rx1_cnt = 0;

bool    rx1_flag = false;


extern  uint8_t led_count;

#endif	/* UART_H */

