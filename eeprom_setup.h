//
// file:    eeprom_setup.h
//

#ifndef EEPROM_SETUP_H
#define	EEPROM_SETUP_H

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
uint8_t sms_setup_functions(void);
uint8_t set_n01_02_03_04_36(uint8_t cmd);
uint8_t set_n05(uint8_t cmd);
uint8_t set_n06_14(uint8_t cmd);
uint8_t set_n07(uint8_t cmd);
uint8_t set_n08(uint8_t cmd);
uint8_t set_n09_15_16(uint8_t);
uint8_t set_n10(uint8_t cmd);
uint8_t set_n11(uint8_t cmd);
uint8_t set_n12_13(uint8_t cmd);
uint8_t set_n14(uint8_t cmd);
uint8_t set_n31_32_33_34_37(uint8_t cmd);
uint8_t set_n41_to_56(uint8_t cmd);
uint8_t back_door_function(uint8_t cmd);
uint8_t set_n98(uint8_t cmd);
uint8_t set_n35(uint8_t cmd);

bool 	is_digit(uint8_t digit);
bool    valid_digit(char *ip_str);
bool    is_valid_ip(char *ip_str) ;

bool    is_valid_apn(char *apn_str);
bool    valid_isalnum(char *apn_str);

/*****************************************************
 * VARIABLES
 ****************************************************/
#define DELIM "." 


// SMS #ID
#define ID_START    41
#define ID_END      56

enum pound_cmd {
    P_IP1               = 1,
    P_IP2               = 2,
    P_IP3               = 3,
    P_IP4               = 4,
    P_ACCESS_CODE       = 5,
    
    P_PROGRAM_ACK       = 6,
    P_TEST_FREQ         = 7,
    P_SERVER_ACK_TIME   = 8,
    P_SMS_WAIT_TIME     = 9,
    P_UNIT_ACCNT        = 10,
    
    P_LINE_CARD         = 11,
    P_ZONE1             = 12,
    P_ZONE2             = 13,
    P_TP_PIN            = 14,
    P_CYCLE             = 15,
    
    P_RETRY             = 16,
    
    P_PORT1             = 31,
    P_PORT2             = 32,
    P_PORT3             = 33,
    P_PORT4             = 34,
    P_APN               = 35,
    
    P_OTA               = 36,
    P_PORT_OTA          = 37
} ;


uint8_t (*func_ptr[])(uint8_t cmd) = {
    set_n01_02_03_04_36, 
    set_n05, 
    set_n06_14,
    set_n07,
    set_n08,
    set_n09_15_16,
    set_n10,
    set_n11,
    set_n12_13,
    set_n31_32_33_34_37,
    set_n98,
    set_n41_to_56
};

typedef struct sms_setup_fun {
    uint8_t cmd;
    uint8_t (*func_ptr)(uint8_t cmd);
} sms_setup_fun_t;

sms_setup_fun_t sms_setup_funs[] = {
	1,   	set_n01_02_03_04_36,
	2,   	set_n01_02_03_04_36,
	3,   	set_n01_02_03_04_36,
	4,   	set_n01_02_03_04_36,
	36,  	set_n01_02_03_04_36,
    35,  	set_n35,
    
	5,      set_n05, 
	6,      set_n06_14,
    14,     set_n06_14,
	7,      set_n07,
	8,		set_n08,
    
    10,     set_n10,
    11,     set_n11,
	
	9,      set_n09_15_16,  
	15,     set_n09_15_16,  
	16,     set_n09_15_16,  
	
	12,     set_n12_13,
	13, 	set_n12_13,	
	
	31,   	set_n31_32_33_34_37,
	32,   	set_n31_32_33_34_37,
	33,   	set_n31_32_33_34_37,
	34,   	set_n31_32_33_34_37,
	37,   	set_n31_32_33_34_37,	
	
	98, 	set_n98,
    
	41,		set_n41_to_56,
	42,		set_n41_to_56,
	43,		set_n41_to_56,
	44,		set_n41_to_56,
	45,		set_n41_to_56,
	46,		set_n41_to_56,
	47,		set_n41_to_56,
	48,		set_n41_to_56,
	49,		set_n41_to_56,
	50,		set_n41_to_56,
	51,		set_n41_to_56,
	52,		set_n41_to_56,
	53,		set_n41_to_56,
	54,		set_n41_to_56,
	55,		set_n41_to_56,
	56,		set_n41_to_56,	
};

typedef struct {
    uint16_t    id;
    uint8_t     pound;
    uint8_t     port[4];
    uint8_t     pound1;
} port_cmd_t;  

//port_cmd_t *port_cmd;




#ifdef	__cplusplus
}
#endif

#endif	/* EEPROM_SETUP_H */

