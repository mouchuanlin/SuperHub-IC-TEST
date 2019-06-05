/* 
 * File:   Module_Library.h
 * Author: YEN
 *
 * Created on 2017?3?22?, ?? 9:56
 */

#ifndef MODULE_LIBRARY_H
#define	MODULE_LIBRARY_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "stdint.h"
#include "stdbool.h"
    
// The Carriage Return (CR) character (0x0D, \r) moves the cursor to the beginning of the line without advancing to the next line. 
// The Line Feed (LF) character (0x0A, \n) moves the cursor down to the next line without returning to the beginning of the line.
#define CR 		0x0D
#define LF 		0x0A
    
void out_sbuf(uint8_t);
void soutdata(uint8_t*);
void out_sbuf2(uint8_t);
uint8_t wait_ok_respond(uint16_t);
uint8_t check_module_run(void);
uint8_t check_sim_card(void);
uint8_t check_register(uint8_t);
uint8_t check_csq(void);
uint8_t check_module_version(uint8_t type);
void write_EE_setting(uint8_t page, uint8_t addr, uint8_t const setting[]);
void init_modem();
extern void powerup_modem();
    
#ifdef	__cplusplus
}
#endif

#endif	/* MODULE_LIBRARY_H */

