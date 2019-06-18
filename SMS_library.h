/* 
 * File:   SMS_library.h
 * Author: YEN
 *
 * Created on 2017年4月28日, 下午 3:35
 */

#ifndef SMS_LIBRARY_H
#define	SMS_LIBRARY_H

#ifdef	__cplusplus
extern "C" {
#endif
    
#include "stdint.h"
#include "stdbool.h"

void    set_sms_init(void);
uint8_t check_sms(void);
uint8_t read_sms(uint8_t, uint8_t, uint8_t);
void    delete_sms(uint8_t, uint8_t, uint8_t);
uint8_t check_remote(void);
uint8_t remote_setting(void);
void    send_respond(uint8_t);
void    select_sms_type(bit);
uint8_t respond_setting(uint8_t,uint8_t);


#ifdef	__cplusplus
}
#endif

#endif	/* SMS_LIBRARY_H */

