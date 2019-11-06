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
#include "inc.h"   
#include "io.h"   
   
    
/*****************************************************
 * FUNCTION PROTOTYPES
 ****************************************************/  
uint8_t read_ee(uint8_t page,uint8_t addr);
void    write_ee(uint8_t page, uint8_t addr, uint8_t data_p);
void    init_eeprom(void);
void    load_default(void);
//void    check_led_type(void);
void    load_device_id_table(void);
uint8_t get_zone_number(uint8_t *device_id);
uint8_t add_ID(uint8_t *ptr);
uint8_t del_ID(uint8_t id);
uint8_t send_trigger_to_RF(uint8_t type);
void    write_EE_setting(uint8_t page, uint8_t addr, uint8_t const setting[]);
void    check_ip_setting(void);
void    write_test_device_id(void);
uint8_t *read_eeprom(uint8_t page, uint8_t addr, uint8_t *ptr, uint16_t len);
void    write_eeprom(uint8_t page, uint8_t addr, uint8_t *data_p, uint16_t len);
    
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
    
// Page 0 EEPROM Map
#define VER_ADDR0                           0x00
#define VER_ADDR1                           0x01
#define VER_ADDR2                           0x02   
#define VER_ADDR3                           0x03
#define MM_COUNT_ADDR                       0x0F

#define APN_ADDR                            0x10    // 35#
#define IP1_ADDR                            0x30    // 01# 
#define IP2_ADDR                            0x50    // 02# 
#define IP3_ADDR                            0x70    // 03# 
#define IP4_ADDR                            0x90    // 04# 

#define PORT1_ADDR                          0xB0    // 31#
#define PORT2_ADDR                          0xB2    // 32#
#define PORT3_ADDR                          0xB4    // 33#
#define PORT4_ADDR                          0xB6    // 34#
#define SMS_WAIT_TIME_ADDR                  0xB8    // 09#
#define ZONE1_ADDR                          0xB9    // 12#
#define ZONE2_ADDR                          0xBA    // 13#
#define TP_PIN_ADDR                         0xBB    // 14#
#define CYCLE_ADDR                          0xBC    // 15#
#define RETRY_TIMES_ADDR                    0xBD    // 16#

#define ACCESS_CODE_ADDR                    0xC0    // 05#
#define PROGRAM_ACK_ADDR                    0xC7    // 06#
#define TESTING_FREQ_ADDR                   0xC8    // 07#
#define SERVER_ACK_TIME_ADDR                0xC9    // 08#
#define UNIT_ACCT_ADDR                      0xCA    // 10#

#define LINE_CARD_ADDR                      0xD0    // 11#
#define ENCRYPTION_ADDR                     0xE0    // 95#


// Page 1 EEPROM Map
#define IP_OTA_ADDR                         0xD0    // 36#
#define PORT_OTA_ADDR                       0xF0    // 37#
    
    
#ifdef DEBUG
    uint8_t const APN[]="internet#";            //35#
    //uint8_t const IP1[]="198.17.112.128#";    //01#
    uint8_t const IP1[]="106.104.30.120#";
#else
    //uint8_t const APN[]="c2.korem2m.com#";           //35# Kore
    uint8_t const APN[]="11583.mcs#";           //35# Telit
    uint8_t const IP1[]="198.17.112.128#";      //01#
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
    // Server in John's home office
    //uint8_t const IP_OTA[]="72.197.171.234#";   //36#
    // Server in Instant Care office
    uint8_t const IP_OTA[]="12.12.201.84#";   //36#
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



typedef struct eeprom_setting1
{
    uint8_t page;
    uint8_t addr;
    uint8_t *data;
} eepromSetting1_t;

eepromSetting1_t eeprom_1[] = 
{
    EE_PAGE0, IP1_ADDR,         (uint8_t *)"198.17.112.128#",
    EE_PAGE0, IP2_ADDR,         (uint8_t *)"#",
    EE_PAGE0, IP3_ADDR,         (uint8_t *)"#",
    EE_PAGE0, IP4_ADDR,         (uint8_t *)"#",
    EE_PAGE1, IP_OTA_ADDR,      (uint8_t *)"72.197.171.234#",
    
    //EE_PAGE0, APN_ADDR,         (uint8_t *)"c2.korem2m.com#", // Kore
    EE_PAGE0, APN_ADDR,         (uint8_t *)"11583.mcs#",        // Telit
    
    EE_PAGE0, ACCESS_CODE_ADDR, (uint8_t *)"1111#",
    EE_PAGE0, UNIT_ACCT_ADDR,   (uint8_t *)"4007#",
    EE_PAGE0, LINE_CARD_ADDR,   (uint8_t *)"7548#",
};

typedef struct eeprom_setting2
{
    uint8_t page;
    uint8_t addr;
    uint16_t data;
} eepromSetting2_t;

eepromSetting2_t eeprom_2[] = 
{
    EE_PAGE0, PORT1_ADDR,   2020,
    EE_PAGE0, PORT2_ADDR,   2020,
    EE_PAGE0, PORT3_ADDR,   2020,
    EE_PAGE0, PORT4_ADDR,   2020,
    EE_PAGE1, PORT_OTA_ADDR, 2021,
};

typedef struct eeprom_setting3
{
    uint8_t page;
    uint8_t addr;
    uint8_t data;
} eepromSetting3_t;

eepromSetting3_t eeprom_3[] = 
{
//    EE_PAGE0, VER_ADDR0, VERSION[0],
//    EE_PAGE0, VER_ADDR1, VERSION[1],
//    EE_PAGE0, VER_ADDR2, VERSION[2],
};

// const registerSetting_t preferredSettings[]= 
// {
  // {CC1120_IOCFG3,            0xB0}
  
/*  
    // Write registers to radio
    for( i = 0; i < (sizeof(preferredSettings)/sizeof(registerSetting_t)); i++) 
    {
        writeByte = preferredSettings[i].data;
        cc1120SpiWriteReg(preferredSettings[i].addr, &writeByte, 1);
    }  
*/	

// Page 0 EEPROM Map - 256 bytes
typedef struct pg0_eeprom_struct
{
    // 0x00 - 0x0F
    uint8_t VERSION[4]; 
    uint8_t reserved1[11];
    uint8_t MM_Count;
    
    // 0x10
    uint8_t APN[16];			//35#
    // 0x20 
    uint8_t reserved2[16];    
    // 0x30
    uint8_t IP1[16];			//01#
    // 0x40
    uint8_t reserved3[16];        
    // 0x50
    uint8_t IP2[16];			//02#
    // 0x60
    uint8_t reserved4[16];        
    // 0x70
    uint8_t IP3[16];            //03#
    // 0x80
    uint8_t reserved5[16];  
    // 0x90
    uint8_t IP4[16];            //04#    
    
    // 0xA0
    uint8_t reserved6[16];
    
    // 0xB0 - 0xBF
    uint16_t PORT1;				//31#   
    uint16_t PORT2;				//32#   
    uint16_t PORT3;				//33#   
    uint16_t PORT4;				//34#   
    uint8_t SMS_WAIT_TIME;		//09#	
	uint8_t ZONE1;				//12#
	uint8_t ZONE2;				//12#
    uint8_t TP_PIN;				//14#
    uint8_t CYCLE;				//15#
    uint8_t RETRY;				//16#  	
	uint8_t reserved7[2];
	
    // 0xC0 - 0xCF
    uint8_t ACCESS_CODE[7];		//05#
    uint8_t PROGRAM_ACK;		//06#
    uint8_t TEST_FREQ;			//07#
    uint8_t SERVER_ACK_TIME;	//08#
    uint8_t UNIT_ACCNT[6];		//10#
	
	// 0xD0 - 0xDF
    uint8_t LINE_CARD[5];		//11#
	uint8_t reserved8[11];
    
    // 0xE0
    uint8_t ENCRYPTION;			//95#    
    // 0xE1 - 0xFF
	uint8_t reserved9[31];
	
} pg0_eeprom_map_t;

// Page 1 EEPROM Map - 256 bytes
typedef struct pg1_eeprom_struct
{
    // 0x00 - 0x0F
    uint8_t device_id[16][8];
 
    // 0x80 - 0xCF
    uint8_t reserved1[80];    
    
    // 0xD0 - 0xDF
    uint8_t IP_OTA;            //36#
    
    // 0xE0 - 0xEF
    uint8_t reserved2[16];
        
    // 0xF0 - 0xFF
    uint16_t PORT_OTA;         //37#      
    uint8_t reserved3[15];
    
} pg1_eeprom_map_t;
    

#define EE_PAGE_SIZE    256
union pg0_eeprom {
    uint8_t             data[EE_PAGE_SIZE];
    pg0_eeprom_map_t    pg0_eeprom_map;
} page0_eeprom;


union pg1_eeprom {
    uint8_t             data[EE_PAGE_SIZE];    
    pg1_eeprom_map_t    pg1_eeprom_map;
} page1_eeprom;
    
extern bool ready_for_sending;    
    




#ifdef	__cplusplus
}
#endif

#endif	/* EEPROM_H */

