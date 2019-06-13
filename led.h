/* 
 * File:   led.h
 * Author: THE MASTER
 *
 * Created on April 30, 2018, 10:20 AM
 */

#ifndef LED_H
#define	LED_H

/*****************************************************
 * INCLUDES
 ****************************************************/
//#include "config.h"

/*****************************************************
 * VARIABLES
 ****************************************************/
typedef enum LED_STATES {
    IDLE,           		// LEDs off
    POWERON,       			// "Power ON"; green LED blinking, waiting for modem
    STANDBY,        		// learn_btn 5-1 	Blue LED blinking; used in SMS listen mode (APN set)
    SENDING,        		// learn_btn 5-4	Blue LED blinks 0.1s on/off
    BUTTON_MENU,    		// learn_btn 5		Both LEDs on solid
    APN_IP_ACCT_NOT_SET,    // learn_btn 5-1    Both LEDs blink 0.5s on/off; when green off, blue on and vice versa (APN not set)
    SEND_ERR,       		// Blue blinks on 0.1s then while blue off,
							// green blinks 2x 0.1s on/off
    SENSOR_ADD,     		// learn_btn 5-2	Green on solid, blue blinking
    SENSOR_DELETE,  		// learn_btn 5-3	Blue on solid, green blinking
    LINE_FAULT,     		// Both LEDs blink 0.3s ON/4.7s OFF
    RF_INT          		// Green on solid for 10s (or maybe less)
} led_states_t;


#define G_ON()              G_LED=0
#define G_OFF()             G_LED=1
#define B_ON()              B_LED=0
#define B_OFF()             B_LED=1

/*****************************************************
 * FUNCTION PROTOTYPES
 ****************************************************/
void update_led_state(led_states_t new_state);
void control_leds();


// Global variables - defined in main.c
extern uint8_t 		gled_tmr0_tick, bled_tmr0_tick;
extern led_states_t led_state;


#endif	/* LED_H */

