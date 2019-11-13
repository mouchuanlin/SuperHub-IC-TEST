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
uint8_t set_n01_02_03_04_35_36(uint8_t cmd);
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

bool 	is_digit(uint8_t digit);

/*****************************************************
 * VARIABLES
 ****************************************************/
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
    set_n01_02_03_04_35_36, 
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
    uint8_t *cmd;
    uint8_t (*func_ptr)(uint8_t cmd);
} sms_setup_fun_t;

//sms_setup_fun_t sms_setup_funs[] = {
//	{1, 2, 3, 4, 35, 36},   set_n01_02_03_04_35_36,
//	{5},                      set_n05, 
//	{6, 14},                set_n06_14,
//	{7},                      set_n07,
//	{8},                      set_n08,
//	{9, 10, 11},            set_n09_15_16,   
//	{12, 13},               set_n12_13,
//	{31, 32, 33, 34, 37},   set_n31_32_33_34_37,
//	{98}, 					set_n98,
//	{41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56}, set_n41_to_56,
//};

typedef struct port_stru {
    uint8_t     id[2];
    uint8_t     pound;
    uint8_t     port[4];
    uint8_t     pound1;
} port_stru_t;  

port_stru_t *port_no;




#ifdef	__cplusplus
}
#endif

#endif	/* EEPROM_SETUP_H */

