//
// timer.h
//

#ifndef TIMER_H
#define	TIMER_H

/*****************************************************
 * INCLUDES
 ****************************************************/
#include "inc.h"

#include "state.h"

/*****************************************************
 * FUNCTION PROTOTYPES
 ****************************************************/
void start_timer0(void);
void reload_timer0(void);
void enable_tmr3(void);
void disable_tmr3(void);
void reload_timer3_2s(void);
void reload_timer3_5s(void);
void reload_timer3_100ms(void);
void timer0_init(void);

void reload_timer3_50ms(void);

void TMR0_ISR(void);
void TMR3_ISR(void);

void delayseconds(uint16_t secs);
void delay5ms(uint16_t cnt);
void process_sms_menu(void);
void exit_learning_mode(void);
void calculate_adc_time(void);
void response_low_batt(void);
void handle_smoke_hub(void);

/*****************************************************
 * VARIABLES
 ****************************************************/
#define _100milliseconds    781         // Timer0 interval for 8MHz clk

#define _1seconds           7812        // Timer3 interval for 8MHz clock
#define _2seconds           15625       // Timer interval for 8MHz clock
#define _5seconds           39062       // Timer interval for 8MHz clock
#define TMR0_CFG            0x87       // 0x87 = b1000 0111 - timer 0 ON, 1:256 prescale

// button press
extern bool         inButtonMenu;
extern uint8_t      buttonPressCount;
extern uint8_t      tmr3_cnt;
extern state_t      myState;
extern uint16_t     exit_learn;
// Smoke or Super hub
extern uint8_t      hub_type;

extern uint8_t      err_count;
extern uint8_t      error_status_count;
extern uint8_t      alarm_count;
extern uint8_t      Standby_f;
extern uint8_t      Error_f;
extern uint16_t 	standby_count;
extern uint8_t      Smoke_respond;
extern uint8_t      error_count;
extern uint16_t 	error_time_detect;

// This variable used to turn off LED2/3. Only ON for 1 second.
uint8_t     led_count = 0;
uint16_t 	adc_count = 0;
uint8_t 	Respond_T_Hour = 0;
uint8_t 	Respond_T_Day = 0;
uint8_t 	OTA_flag = 0;

#endif	/* TIMER_H */

