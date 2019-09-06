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

extern bool ADC_time;
extern uint8_t BT_S_respond;
extern uint8_t BT_L_respond;
extern uint8_t EOL_respond;

// Smoke or Super hub
extern uint8_t      hub_type;


/*****************************************************
 * FUNCTION PROTOTYPES
 ****************************************************/
void check_state(void);
void check_button(void);
void check_alarm_tamper(void);
void add_sensor(void);
void delete_sensor(void);
bool    process_restart(void);
void 	process_ADC(void);
void 	process_RF_interrupt(void);
void 	process_supervisory(void);
uint8_t check_supervisory(void);
void check_alarm_tamper(void);

extern void process_event_queue(void);
extern void delayseconds(uint16_t secs);

extern void start_modem(void);
extern void restart_modem(void);
extern void poweroff_modem(void);
extern void delay5ms(uint16_t cnt);
extern void add_event(uint8_t event,uint8_t zone);

extern void UART2_init(void);
extern void control_leds(void);
extern void check_RF_device(void);


#endif	/* STATE_H */

