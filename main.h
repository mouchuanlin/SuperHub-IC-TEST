/* 
 * File:   main.h
 * Author: Dick
 *
 * Created on May 7, 2019, 9:52 AM
 */

#ifndef MAIN_H
#define	MAIN_H

/*****************************************************
 * INCLUDES
 ****************************************************/
#include <stdint.h>
#include <stdbool.h>

/*****************************************************
 * FUNCTION PROTOTYPES
 ****************************************************/
void 	init_system();
void    int_init();
void 	init_stack_buffer();
void 	init_global_variables();
uint8_t get_hub_type();
void 	buzzer_on(uint8_t count);
//void 	process_ADC();
//void 	process_RF_interrupt();
//void 	process_supervisory();
void    superhub_ISR();
void    sms_menu();
void    handle_smoker();

// Jen's simple_state_machine pronect
extern void start_modem();
extern void restart_modem();
extern void poweroff_modem();
extern void check_state();


//#define MODULE_OFF_TYPE  

/*****************************************************
 * VARIABLES
 ****************************************************/

//uint8_t Alarm_f = 0;
uint8_t Standby_f = 0;
uint8_t Error_f = 0;
//  uint8_t Test_f = 0;
//  bit Low_battery = 0;
// bit Smoke_EOL = 0;
      
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
uint16_t 	adc_count = 0;
uint8_t 	Respond_T_Hour = 0;
uint8_t 	Respond_T_Day = 0;
uint8_t 	err_count=0;
uint8_t 	LED_count = 0;
uint8_t 	error_status_count = 0;
uint8_t 	alarm_count = 0;
//uint8_t 	rx2_cnt = 0;
//uint8_t 	rx2_buf[20];
uint16_t 	exit_learn;
uint8_t 	tamper_status = 1;
uint8_t 	first_tamper = 1;
uint8_t 	led_count = 0;
uint8_t 	learn_delay = 0;    
uint16_t 	test_9sec_count=0;
uint8_t 	OTA_flag = 0;


#endif	/* MAIN_H */