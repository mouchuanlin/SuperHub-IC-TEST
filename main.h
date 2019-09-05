//
// main.h
//

#ifndef MAIN_H
#define	MAIN_H

/*****************************************************
 * INCLUDES
 ****************************************************/
#include "inc.h"

#include "state.h"
#include "led.h"

/*****************************************************
 * FUNCTION PROTOTYPES
 ****************************************************/
void 	init_system();
void    int_init();
void 	init_stack_buffer();
void 	init_global_variables();
uint8_t get_hub_type();
void 	buzzer_on(uint8_t count);
void    smokehub_ISR();
void    superhub_ISR();
void    sms_menu();
void    prepare_to_sleep();

extern void start_modem();
extern void restart_modem();
extern void poweroff_modem();
extern void check_state();
extern void init_eeprom();
extern void load_device_id_table();
extern void UART_init();
extern void UART1_ISR();
extern void UART2_ISR();
extern void start_timer0();
extern void TMR0_ISR();
extern void disable_UART(void);
extern  void disable_UART1();
extern void check_RF_device();
extern void add_event(uint8_t event, uint8_t zone);
extern void calculate_adc_time();

//extern void check_OTA_status();

/*****************************************************
 * VARIABLES
 ****************************************************/
uint8_t Standby_f = 0;
uint8_t Error_f = 0;
      
uint8_t Smoke_respond = 0;    
//bit Respond_ack = 0;
uint8_t BT_S_respond = 0;
uint8_t BT_L_respond = 0;
uint8_t EOL_respond = 0;
//bit Tamper_open_f = 0;
//bit Tamper_close_f = 0;    
uint16_t test_count = 0;
uint16_t first_test = 18000; //30*60*10 (30mins)

uint8_t 	error_count = 0;
uint16_t 	standby_count = 0;
uint16_t 	error_time_detect = 0;
uint16_t 	test_time_detect = 0;
//uint16_t 	adc_count = 0;
//uint8_t 	Respond_T_Hour = 0;
//uint8_t 	Respond_T_Day = 0;
uint8_t 	err_count=0;
uint8_t 	error_status_count = 0;
uint8_t 	alarm_count = 0;

//uint8_t 	tamper_status = 1;
//uint8_t 	first_tamper = 1;

uint8_t 	learn_delay = 0;    
uint16_t 	test_9sec_count=0;
//uint8_t 	OTA_flag = 0;


// Global variables
state_t     myState = POWER_UP;
bool        readyForSleep = false;

// LED
uint8_t         gled_tmr0_tick = 0, bled_tmr0_tick = 0;
led_states_t    led_state = OFF;
uint8_t         hub_type = 0;

// button press
bool        inButtonMenu = false;
uint8_t     buttonPressCount = 0;
uint8_t     tmr3_cnt = 0;
//bool        g_op_state = false;
bit         listen_sms_state = 0;  
uint16_t 	exit_learn = 0;


bit         ADC_time = 0;


//
bool        ready_for_sending = false;

#endif	/* MAIN_H */