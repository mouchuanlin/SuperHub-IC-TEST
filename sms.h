//
// sms.h
//

#ifndef SMS_H
#define	SMS__H

#ifdef	__cplusplus
extern "C" {
#endif
    
#include "stdint.h"


void    set_sms_init(void);
uint8_t check_sms(void);
uint8_t read_sms(uint8_t, uint8_t, uint8_t);
void    delete_sms(uint8_t, uint8_t, uint8_t);
uint8_t check_remote(void);
uint8_t remote_setting(void);
void    send_respond(uint8_t);
void    select_sms_type(bit);
uint8_t respond_setting(uint8_t,uint8_t);

extern void     out_sbuf(uint8_t tmp);
extern void     soutdata(uint8_t *dat);
extern uint8_t  wait_ok_respond(uint16_t);
extern uint8_t  check_csq(void);

#ifdef	__cplusplus
}
#endif

#endif	/* SMS_H */

