/* 
 * File:   modem.h
 * Author: THE MASTER
 *
 * Created on April 30, 2018, 10:20 AM
 */

#ifndef MODEM_H
#define	MODEM_H

/*****************************************************
 * INCLUDES
 ****************************************************/
 #include "stdint.h"
#include "stdbool.h"

#include "state.h"

/*****************************************************
 * VARIABLES
 ****************************************************/
// button press
extern bool    inButtonMenu;
extern uint8_t buttonPressCount;
extern uint8_t tmr3_cnt;
extern state_t myState;
extern uint8_t 	OTA_flag;
extern bit listen_sms_state;  


/*****************************************************
 * FUNCTION PROTOTYPES
 ****************************************************/
void 	powerup_modem();
void    start_modem();
bool    md_config_ok();
void    restart_modem();
void    poweroff_modem();
uint8_t wait_AT_cmd_response();
uint8_t check_SIM_state();
uint8_t check_network_registration();
bool    check_apn_status();
uint8_t start_send_alarm();
uint8_t wait_SMS_setting();
void    process_event_queue();
uint8_t process_SMS_setup_state();
void out_sbuf(uint8_t tmp);
void soutdata(uint8_t *dat);
void out_sbuf2(uint8_t);
uint8_t wait_ok_respond(uint16_t);
uint8_t check_module_version(uint8_t type);
uint8_t check_module_run(void);
uint8_t check_sim_card(void);
uint8_t check_register(uint8_t);
uint8_t check_csq(void);


extern void     delay5ms(uint16_t cnt);
extern void     TL_module_first_run(void);
extern void     check_led_type(void);
extern uint8_t  read_ee(uint8_t page,uint8_t addr);
extern void     write_ee(uint8_t page,uint8_t addr,uint8_t data_p);
extern void     set_sms_init(void);
extern void     check_event(void);
extern uint8_t  check_sms(void);


extern uint8_t  check_emc_stack(void);
extern void     check_state();
extern uint8_t  check_OTA(void);

    
// The Carriage Return (CR) character (0x0D, \r) moves the cursor to the beginning of the line without advancing to the next line. 
// The Line Feed (LF) character (0x0A, \n) moves the cursor down to the next line without returning to the beginning of the line.
#define CR 		0x0D
#define LF 		0x0A
    


#endif	/* MODEM_H */

