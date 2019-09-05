//
// state.h
//


#ifndef STATE_H
#define	STATE_H

/*****************************************************
 * INCLUDES
 ****************************************************/
#include "inc.h"

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
    OPERATIONAL,
            
    OTA_BOOT
} state_t;

uint8_t 	tamper_status = 1;
uint8_t 	first_tamper = 1;

extern bit ADC_time;
extern uint8_t BT_S_respond;
extern uint8_t BT_L_respond;
extern uint8_t EOL_respond;

// Smoke or Super hub
extern uint8_t      hub_type;


/*****************************************************
 * FUNCTION PROTOTYPES
 ****************************************************/
void check_state();
void check_button();
void check_alarm_tamper();
void add_sensor() ;
void delete_sensor();
bool    process_restart();
void 	process_ADC();
void 	process_RF_interrupt();
void 	process_supervisory();
uint8_t check_supervisory();
void check_alarm_tamper();

extern void process_event_queue();
extern void delayseconds(uint16_t secs);

extern void start_modem();
extern void restart_modem();
extern void poweroff_modem();
extern void delay5ms(uint16_t cnt);
extern void add_event(uint8_t event,uint8_t zone);

extern void UART2_init();
extern void control_leds();
extern void check_RF_device();


#endif	/* STATE_H */

