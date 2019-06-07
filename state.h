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
} STATE;

enum LED_PATTERNS {
    IDLE,           // LEDs off
    WAIT,           // "Power ON"; green LED blinking, waiting for modem
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
};
enum LED_PATTERNS ledPattern;

/*****************************************************
 * FUNCTION PROTOTYPES
 ****************************************************/
void check_state(STATE *state);
void control_leds();

void check_button();
void check_alarm_tamper();
void add_sensor() ;
void delete_sensor()  ;
void check_supervisory_NEW();




bool readyForSleep = false;

bool G_LED_STATE = 1, B_LED_STATE = 1;
uint8_t g_tmr0_tick = 0, b_tmr0_tick = 0;


#endif	/* STATE_H */

