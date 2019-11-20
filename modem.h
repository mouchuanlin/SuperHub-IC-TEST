//
// modem.h
//

#ifndef MODEM_H
#define	MODEM_H

/*****************************************************
 * INCLUDES
 ****************************************************/
#include "inc.h"

#include "state.h"

/*****************************************************
 * FUNCTION PROTOTYPES
 ****************************************************/
void 	powerup_modem(void);
void    start_modem(void);
bool    modem_config_ok(void);
void    restart_modem(void);
void    poweroff_modem(void);
uint8_t wait_AT_cmd_response(void);
uint8_t check_SIM_state(void);
uint8_t check_network_registration(void);
bool    check_apn_status(void);
uint8_t start_send_alarm(void);
uint8_t start_sms(void);
void    process_event_queue(void);
uint8_t process_SMS_setup_state(void);
void    out_sbuf(uint8_t tmp);
void    soutdata(const uint8_t *dat);
void    soutdata_1(const uint8_t dat[], uint8_t len);
void    out_sbuf2(uint8_t tmp);
uint8_t wait_ok_respond(uint16_t);
uint8_t check_module_version(uint8_t type);
uint8_t check_module_run(void);
uint8_t check_sim_card(void);
uint8_t check_register(uint8_t);
uint8_t check_csq(void);
void    send_test_AT_commands(void);
void    reset_at_command_default(void);
void    test_at(void);
uint8_t send_at_command(uint8_t str[]);
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
extern bool     listen_sms_state;  
    

#endif	/* MODEM_H */

