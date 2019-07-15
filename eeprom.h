// 
// File:   eeprom.h
//

#ifndef EEPROM_H
#define	EEPROM_H

#ifdef	__cplusplus
extern "C" {
#endif

/*****************************************************
 * INCLUDES
 ****************************************************/
#include "stdint.h"
#include "stdbool.h"

#include "led.h"
    
    
/*****************************************************
 * FUNCTION PROTOTYPES
 ****************************************************/  
void    init_eeprom(void);
void    load_default(void);
//void    check_led_type(void);
void    load_device_id_table(void);
uint8_t check_ID(uint8_t *ptr);
uint8_t add_ID(uint8_t *ptr);
uint8_t del_ID(uint8_t id);
uint8_t send_trigger_to_RF(uint8_t type);
void    write_EE_setting(uint8_t page, uint8_t addr, uint8_t const setting[]);
uint8_t read_ee(uint8_t, uint8_t );
void    write_ee(uint8_t, uint8_t, uint8_t );

extern void     delay5ms(uint16_t);
extern void     delayseconds(uint16_t secs);    
extern void     add_event(uint8_t event,uint8_t zone);    
extern void     check_ip_setting();
    
/*****************************************************
 * VARIABLES
 ****************************************************/
// The Carriage Return (CR) character (0x0D, \r) moves the cursor to the beginning of the line without advancing to the next line. 
// The Line Feed (LF) character (0x0A, \n) moves the cursor down to the next line without returning to the beginning of the line.
#define CR 		0x0D
#define LF 		0x0A
    

// EEPROM Page#
#define EE_PAGE0                            0x00
#define EE_PAGE1                            0x01
    
// TODO: Check Spec    
#define FIRST_RUN_CHECK                     0xF0        // EEPROM address corresp. to this value    
    
// EEPROM Addresses for each parameter 
#define APN_ADDR                            0x10
#define IP1_ADDR                            0x30
#define IP2_ADDR                            0x50
#define IP3_ADDR                            0x70
#define IP4_ADDR                            0x90
#define PORT1_ADDR                          0xB0
#define PORT2_ADDR                          0xB2
#define PORT3_ADDR                          0xB4
#define PORT4_ADDR                          0xB6
#define SMS_WAIT_TIME_ADDR                  0xB8
#define ACCESS_CODE_ADDR                    0xC0
#define PROGRAM_ACK_ADDR                    0xC7
#define TESTING_FREQ_ADDR                   0xC8
#define SERVER_ACK_TIME_ADDR                0xC9
#define UNIT_ACCT_ADDR                      0xCA
#define LINE_CARD_ADDR                      0xD0
#define ZONE1_ADDR                          0xB9
#define ZONE2_ADDR                          0xBA
#define TP_PIN_ADDR                         0xBB
#define CYCLE_ADDR                          0xBC
#define RETRY_TIMES_ADDR                    0xBD
#define ENCRYPTION_ADDR                     0xE0

#define VER_ADDR0                           0x00
#define VER_ADDR1                           0x01
#define VER_ADDR2                           0x02   
    
#define IP_OTA_ADDR                         0xD0
#define PORT_OTA_ADDR                       0xF0
    
    
#ifdef DEBUG
    uint8_t const APN[]="internet#";            //35#
    //uint8_t const IP1[]="198.17.112.128#";    //01#
    uint8_t const IP1[]="106.104.30.120#";
#else
    uint8_t const APN[]="#";                    //35#
    uint8_t const IP1[]="#";                    //01#
#endif
#ifdef DEBUG
    uint8_t const IP2[]="106.104.30.120#";      //02#
    uint8_t const IP3[]="106.104.30.120#";      //03#
    uint8_t const IP4[]="106.104.30.120#";      //04#
    // TODO: For testing ONLY.
    uint8_t const IP_OTA[]="72.197.171.234#";   //36#
    uint16_t const PORT1 = 2020;                //31#   
    uint16_t const PORT2 = 2020;                //32#   
    uint16_t const PORT3 = 2020;                //33#   
    uint16_t const PORT4 = 2020;                //34#   
    uint16_t const PORT_OTA = 3030;             //37# 
#else
    uint8_t const IP2[]="#";                    //02#
    uint8_t const IP3[]="#";                    //03#
    uint8_t const IP4[]="#";                    //04#
    uint8_t const IP_OTA[]="72.197.171.234#";   //36#
    uint16_t const PORT1 = 2020;                //31#   
    uint16_t const PORT2 = 2020;                //32#   
    uint16_t const PORT3 = 2020;                //33#   
    uint16_t const PORT4 = 2020;                //34#   
    uint16_t const PORT_OTA = 2021;             //37#   
#endif
    uint8_t const ACCESS_CODE[]="1111#";        //05#
    uint8_t const PROGRAM_ACK=0x01;             //06#
    uint8_t const TEST_FREQ=15;                 //07#
    uint8_t const SERVER_ACK_TIME=45;           //08#
    uint8_t const UNIT_ACCNT[]="4007#";         //10#
    uint8_t const LINE_CARD[]="7548#";          //11#
    uint8_t const ZONE1='0';                    //12#
 //   uint8_t const ZONE2=20;                   //13#
    uint8_t const TP_PIN=0;                     //14#
    uint8_t const CYCLE = 3;                    //15#
    uint8_t const RETRY = 30;                   //16#
#ifndef DEBUG
    uint8_t const SMS_WAIT_TIME = 10;           //09#
#else
    uint8_t const SMS_WAIT_TIME = 3;            //09#
#endif
    uint8_t const ENCRYPTION = 1;               //95#    

    
    
extern bool ready_for_sending;    
    
#ifdef	__cplusplus
}
#endif

#endif	/* EEPROM_H */

