//
// led.h
//

#ifndef LED_H
#define	LED_H

/*****************************************************
 * INCLUDES
 ****************************************************/
#include "inc.h"

/*****************************************************
 * FUNCTION PROTOTYPES
 ****************************************************/
typedef enum LED_STATES {
    OFF,           		// LEDs off
    POWERON,       			// "Power ON"; green LED blinking, waiting for modem
    STANDBY,        		// learn_btn 5-1 	Blue LED blinking; used in SMS listen mode (APN set)
    SENDING,        		// learn_btn 5-4	Blue LED blinks 0.1s on/off
    BUTTON_MENU,    		// learn_btn 5		Both LEDs on solid
    APN_IP_ACCT_NOT_SET,    // learn_btn 5-1    Both LEDs blink 0.5s on/off; when green off, blue on and vice versa (APN not set)
    SEND_ERR,       		// Blue blinks on 0.1s then while blue off,
							// green blinks 2x 0.1s on/off
    SENSOR_ADD,     		// learn_btn 5-2	Green on solid, blue blinking
    SENSOR_DEL,  		// learn_btn 5-3	Blue on solid, green blinking
    LINE_FAULT,     		// Both LEDs blink 0.3s ON/4.7s OFF
    RF_INTR          		// Green on solid for 10s (or maybe less)
} led_states_t;

void update_led_state(led_states_t new_state);
void control_leds(void);

// Global variables - defined in main.c
extern uint8_t 		gled_tmr0_tick, bled_tmr0_tick;
extern led_states_t led_state;

/*****************************************************
 * VARIABLES
 ****************************************************/
//--------------------   
// LED state
//#define LED_OFF 0
//#define LED_STANDBY 1
//#define LED_INTERNET 2
//#define LED_NO_SET 3
//#define LED_NET_ERR 4
//#define LED_REGISTER 5
//#define LED_RF_ADD 6 
//#define LED_RF_DEL 7 
//#define MD_POWER_LOSS 8 

#define G_ON()              G_LED=0
#define G_OFF()             G_LED=1
#define R_ON()              R_LED=0
#define R_OFF()             R_LED=1

// LED for RF
#define LED_RF_RX_ON()          LED_RX_IN=0
#define LED_RF_RX_OFF()         LED_RX_IN=1

#define LED_RF_ID_MATCH_ON()    LED_RX_OUT=0
#define LED_RF_ID_MATCH_OFF()   LED_RX_OUT=1

#endif	/* LED_H */

