/* 
 * File:   System_Library.h
 * Author: YEN
 *
 * Created on 2017年4月12日, 下午 2:38
 */

#ifndef SYSTEM_LIBRARY_H
#define	SYSTEM_LIBRARY_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "stdint.h"
#include "stdbool.h"

// The Carriage Return (CR) character (0x0D, \r) moves the cursor to the beginning of the line without advancing to the next line. 
// The Line Feed (LF) character (0x0A, \n) moves the cursor down to the next line without returning to the beginning of the line.
#define CR 		0x0D
#define LF 		0x0A

void delay5ms(uint16_t);
void delayseconds(uint16_t secs);
void first_run(void);
void load_default(void);
void check_led_type(void);
void load_ID_to_buffer(void);
uint8_t check_ID(uint8_t *ptr);
uint8_t add_ID(uint8_t *ptr);
uint8_t del_ID(uint8_t id);
uint8_t check_supervisory(void);
uint8_t send_trigger_to_RF(uint8_t type);


#ifdef	__cplusplus
}
#endif

#endif	/* SYSTEM_LIBRARY_H */

