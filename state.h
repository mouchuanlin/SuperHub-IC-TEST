/* 
 * File:   state.h
 * Author: THE MASTER
 *
 * Created on April 30, 2018, 10:20 AM
 */

#ifndef STATE_H
#define	STATE_H

/*****************************************************
 * INCLUDES
 ****************************************************/
#include "config.h"

/*****************************************************
 * VARIABLES
 ****************************************************/
typedef enum State {
    POWER_UP,
    SLEEP,
	LISTEN_SMS,
	ADD_SENSOR,
	DEL_SENSOR,
    SEND_TEST,
	RF_INTERRUPT,
    TEMPER_INT,
    SUPERVISORY,
    ADC,
    LED_CTRL,
    OPERATIONAL
} state_t;


/*****************************************************
 * FUNCTION PROTOTYPES
 ****************************************************/
void check_state();
void check_button();
void check_alarm_tamper();
void add_sensor() ;
void delete_sensor()  ;
void check_supervisory_NEW();

extern void control_leds();
//extern void check_event(void)

// Global variables - defined in main.c
//extern led_states_t curr_led_state, prev_led_state;


#endif	/* STATE_H */

