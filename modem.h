//
// modem.h
//

#ifndef MODEM_H
#define	MODEM_H

/*****************************************************
 * INCLUDES
 ****************************************************/
#include "stdint.h"
#include "stdbool.h"

#include "state.h"

/*****************************************************
 * FUNCTION PROTOTYPES
 ****************************************************/
void 	powerup_modem();
void    start_modem();
bool    modem_config_ok();
void    restart_modem();
void    poweroff_modem();
uint8_t wait_AT_cmd_response();
uint8_t check_SIM_state();
uint8_t check_network_registration();
bool    check_apn_status();
uint8_t start_send_alarm();
uint8_t start_sms();
void    process_event_queue();
uint8_t process_SMS_setup_state();
void    out_sbuf(uint8_t tmp);
void    soutdata(uint8_t *dat);
void    out_sbuf2(uint8_t);
uint8_t wait_ok_respond(uint16_t);
uint8_t check_module_version(uint8_t type);
uint8_t check_module_run(void);
uint8_t check_sim_card(void);
uint8_t check_register(uint8_t);
uint8_t check_csq(void);
void    send_test_AT_commands();


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
extern void     check_nework();
extern void     check_receive_overrun();

/*****************************************************
 * VARIABLES
 ****************************************************/
// The Carriage Return (CR) character (0x0D, \r) moves the cursor to the beginning of the line without advancing to the next line. 
// The Line Feed (LF) character (0x0A, \n) moves the cursor down to the next line without returning to the beginning of the line.
#define CR 		0x0D
#define LF 		0x0A

// __delay_ms() need this define)
#define _XTAL_FREQ 8000000

// button press
extern bool     inButtonMenu;
extern uint8_t  buttonPressCount;
extern uint8_t  tmr3_cnt;
extern state_t  myState;
extern uint8_t  OTA_flag;
extern bit      listen_sms_state;  
    

#endif	/* MODEM_H */

