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
    INIT,
//	IDLE_Q_EMPTY,
//	IDLE_Q_NOT_EMPTY,
	SMS,
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

typedef enum LED_STATES {
    IDLE,           // LEDs off
    POWERON,       // "Power ON"; green LED blinking, waiting for modem
    STANDBY,        // Blue LED blinking; used in SMS listen mode
    SENDING,        // Blue LED blinks 0.1s on/off
    BUTTON_MENU,    // Both LEDs on solid
    APN_IP_ACCT_NOT_SET,    // Both LEDs blink 0.5s on/off; when green off, blue
                            // on and vice versa
    SEND_ERR,       // Blue blinks on 0.1s then while blue off,
                    // green blinks 2x 0.1s on/off
    SENSOR_ADD,     // Green on solid, blue blinking
    SENSOR_DELETE,  // Blue on solid, green blinking
    LINE_FAULT,     // Both LEDs blink 0.3s ON/4.7s OFF
    RF_INT          // Green on solid for 10s (or maybe less)
} led_states_t;


/*****************************************************
 * FUNCTION PROTOTYPES
 ****************************************************/
void check_state(state_t *state);
void control_leds();

void check_button();
void check_alarm_tamper();
void add_sensor() ;
void delete_sensor()  ;
void check_supervisory_NEW();
void update_led_state(led_states_t new_state);


// Global variables - defined in main.c
extern uint8_t gled_tmr0_tick, bled_tmr0_tick;
//extern led_states_t curr_led_state, prev_led_state;
extern led_states_t led_state;


#endif	/* STATE_H */

