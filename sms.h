//
// sms.h
//

#ifndef SMS_H
#define	SMS__H

#ifdef	__cplusplus
extern "C" {
#endif

/*****************************************************
 * INCLUDES
 ****************************************************/    
#include "stdint.h"  
    
/*****************************************************
 * FUNCTION PROTOTYPES
 ****************************************************/    
void    set_sms_init(void);
uint8_t check_sms(void);
uint8_t read_sms(uint8_t, uint8_t, uint8_t);
void    delete_sms(uint8_t, uint8_t, uint8_t);
uint8_t check_remote(void);
uint8_t remote_setting(void);
void    send_respond(uint8_t);
void    select_sms_type(bit);
uint8_t respond_setting(uint8_t,uint8_t);
void    get_access_code();
void    check_receive_overrun();

extern void     out_sbuf(uint8_t tmp);
extern void     soutdata(uint8_t *dat);
extern uint8_t  wait_ok_respond(uint16_t);
extern uint8_t  check_csq(void);
extern void     reload_timer3_100ms();
extern void     reload_timer3_50ms();

/*****************************************************
 * VARIABLES
 ****************************************************/
// To write the message issue Ctrl-Z char (0x1A hex).
// To exit without writing the message issue ESC char (0x1B hex).
#define CTRL_Z  0x1A
#define ESC     0x1B

#ifdef	__cplusplus
}
#endif

#endif	/* SMS_H */

