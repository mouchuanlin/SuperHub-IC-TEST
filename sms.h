//
// sms.h
//

#ifndef SMS_H
#define	SMS_H

#ifdef	__cplusplus
extern "C" {
#endif

/*****************************************************
 * INCLUDES
 ****************************************************/    
#include "inc.h"
    
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
//void    select_sms_type(bit);
uint8_t respond_setting(uint8_t,uint8_t);
//void    get_access_code(void);
void    check_receive_overrun(void);

/*****************************************************
 * VARIABLES
 ****************************************************/
// To write the message issue Ctrl-Z char (0x1A hex).
// To exit without writing the message issue ESC char (0x1B hex).
#define CTRL_Z  0x1A
#define ESC     0x1B

#define DASH    '-'
#define COMMA   ','


// SMS response
typedef struct sms_resp1
{
    uint8_t     ver;
    uint8_t 	VERSION[4];
    uint8_t     dash;
    uint8_t 	*IP1;				// 01#
    uint8_t     comma1;
    uint8_t 	*IP2;            	// 02#
    uint8_t     comma2;    
    uint8_t 	*IP3;            	// 03#
    uint8_t     comma3;
    uint8_t 	*IP4;            	// 04#   
    uint8_t     comma4;    
    uint8_t 	*ACCESS_CODE;		// 05#    
    uint8_t     comma5;    
    uint8_t 	PROGRAM_ACK;		// 06#    
    uint8_t     comma6;
    uint8_t 	TEST_FREQ;			// 07# 
    uint8_t     comma7;    
    uint8_t 	SERVER_ACK_TIME;	// 08#    
    uint8_t     comma8;    
    uint8_t 	SMS_WAIT_TIME;		// 09#
    uint8_t     comma9;    
    uint8_t 	*UNIT_ACCNT;		// 10#
    uint8_t     comma10;    
    uint8_t 	*LINE_CARD;			// 11# 
    uint8_t     comma11;    
    uint8_t 	ZONE1;				// 12#
    uint8_t     comma12;    
	uint8_t 	ZONE2;				// 13#  
    uint8_t     comma13;    
    uint8_t 	TP_PIN;				// 14#
    uint8_t     comma14;    
    uint8_t 	CYCLE;				// 15#
    uint8_t     comma15;    
    uint8_t 	RETRY;				// 16#
    uint8_t     comma16;
    uint8_t 	*PORT1;				// 31#
    uint8_t     comma17;    
    uint8_t 	*PORT2;				// 32# 
    uint8_t     comma18;    
    uint8_t 	*PORT3;				// 33#
    uint8_t     comma19;    
    uint8_t     *PORT4;				// 34# 
    uint8_t     comma20;
    uint8_t 	*APN;				// 35# 
    uint8_t     comma21;    
    uint8_t     *IP_OTA;         	// 36#
    uint8_t     comma22;    
    uint8_t     *PORT_OTA;        	// 37#    
    uint8_t     cr;
    uint8_t     lf;
} sms_resp1_t;

extern sms_resp1_t resp_1;

typedef struct sms_resp2
{
    uint8_t     device_id_table[DEVICE_SIZE][DEVICE_LEN];       // 41# - 56#
} sms_resp2_t;

extern uint8_t access_code[7];

#ifdef	__cplusplus
}
#endif

#endif	/* SMS_H */

