/* 
 * File:   timer.h
 * Author: THE MASTER
 *
 * Created on April 30, 2018, 10:20 AM
 */

#ifndef TIMER_H
#define	TIMER_H

/*****************************************************
 * INCLUDES
 ****************************************************/
#include "config.h"
#include "state.h"

/*****************************************************
 * VARIABLES
 ****************************************************/
// button press
extern bool    inButtonMenu;
extern uint8_t buttonPressCount;
extern uint8_t tmr3_cnt;
extern state_t myState;

#define _100milliseconds    781         // Timer0 interval for 8MHz clk

#define _1seconds           7812        // Timer3 interval for 8MHz clock
#define _2seconds           15625       // Timer interval for 8MHz clock
#define _5seconds           39062       // Timer interval for 8MHz clock

#define TMR0_CFG            0x87       // 0x87 = b1000 0111 - timer 0 ON, 1:256 prescale


/*****************************************************
 * FUNCTION PROTOTYPES
 ****************************************************/
void start_timer0();
void reload_timer0();
void enable_tmr3();
void disable_tmr3();
void reload_timer3_2s();
void reload_timer3_5s();
void reload_timer3_100ms();
void timer0_init();

void TMR0_ISR();
void TMR3_ISR();

void delayseconds(uint16_t secs);
void delay5ms(uint16_t cnt);

void process_sms_menu();


extern void add_event(uint8_t event,uint8_t zone);
extern void process_button_push();

#endif	/* TIMER_H */

