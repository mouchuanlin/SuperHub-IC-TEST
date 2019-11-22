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
//void    init_eeprom(void);
void    load_default(void);
//void    check_led_type(void);
void    load_device_id_table(void);
uint8_t get_zone_number(uint8_t *device_id);
uint8_t add_ID(uint8_t *ptr);
uint8_t del_ID(uint8_t id);
uint8_t send_trigger_to_RF(uint8_t type);
//void    write_EE_setting(uint8_t page, uint8_t addr, uint8_t const setting[]);
void    check_ip_setting(void);

void    write_test_device_id(void);
uint8_t *read_eeprom(uint8_t page, uint8_t addr, uint8_t *ptr, uint16_t len);
void    write_eeprom(uint8_t page, uint8_t addr, uint8_t *data_p, uint16_t len);
void 	set_eeprom_value(uint8_t page, uint8_t addr, uint8_t data, uint16_t len);
void 	init_pic18_eeprom(void);

void    update_page_info(void);
void    update_eeprom(void);
void    update_eeprom_page0(void);
void    update_eeprom_page1(void);

void    store_eeprom_init_value(void);
void    restore_eeprom_init_value(void);
void    load_test_device_id(void);
    
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
       
///// Page 0 EEPROM Map
#define VER_ADDR0                           0x00
#define VER_ADDR1                           0x01
#define VER_ADDR2                           0x02   
#define VER_ADDR3                           0x03
#define FIRST_RUN_ADDR                      0x04 
#define MM_COUNT_ADDR                       0x0F

#define APN_ADDR                            0x10    // 35#
#define IP1_ADDR                            0x30    // 01# 
#define IP2_ADDR                            0x50    // 02# 
#define IP3_ADDR                            0x70    // 03# 
#define IP4_ADDR                            0x90    // 04# 

#define PORT1_ADDR                          0xA8    // 31#
#define PORT2_ADDR                          0xAC    // 32#
#define PORT3_ADDR                          0xB0    // 33#
#define PORT4_ADDR                          0xB4    // 34#
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

#define UNIT_ACCT_ADDR                      0xD0    // 10#
#define LINE_CARD_ADDR                      0xD8    // 11#

#define ENCRYPTION_ADDR                     0xE0    // 95#

///// Page 1 EEPROM Map
#define IP_OTA_ADDR                         0xD0    // 36#
#define PORT_OTA_ADDR                       0xF0    // 37#

// 16-bit check sum - same offset for both page 0/1.
#define CHECKSUM_ADDR                       0xFE
    
// EEPROM default value define
uint8_t const   FIRST_RUN       = 0x57;
//uint8_t const 	IP1[]   	= "198.17.112.128";     //01#   // Affiliated portal IP
uint8_t const   IP1[]           = "12.12.201.82";       //01    // Server in IC office IP
//uint8_t const   IP1[]           = "";                   //01    // Default value
uint8_t const 	IP2[]   		= "";                   //02#
uint8_t const 	IP3[]   		= "";                   //03#
uint8_t const 	IP4[]   		= "";                   //04#
uint8_t const 	ACCESS_CODE[]   = "1111";               //05#

uint8_t const 	PROGRAM_ACK     = 0x01;           		//06#
uint8_t const 	TEST_FREQ       = 15;               	//07#
uint8_t const 	SERVER_ACK_TIME = 45;         			//08#
uint8_t const 	SMS_WAIT_TIME   = 10;         			//09#
//uint8_t const SMS_WAIT_TIME   = 3;          			//09#
uint8_t const 	UNIT_ACCNT[]    = "4007";       		//10#
//uint8_t const 	UNIT_ACCNT[]    = "";       		//10#   // Default value
uint8_t const 	LINE_CARD[]     = "7548";        		//11#
//uint8_t const 	LINE_CARD[]     = "";        		//11#   // Default value
uint8_t const 	ZONE1           = '0';                  //12#
//   uint8_t const ZONE2=20;                			//13#
uint8_t const 	TP_PIN          = 0;                   	//14#
uint8_t const 	CYCLE           = 3;                  	//15#

uint8_t const 	RETRY           = 30;                 	//16#

uint8_t const 	PORT1[]       	= "2020";				//31#   
uint8_t const 	PORT2[]       	= "2020";				//32#   
uint8_t const 	PORT3[]         = "2020";               //33#   
uint8_t const 	PORT4[]         = "2020";               //34#   
uint8_t const 	APN[]   		= "11583.mcs";			//35#   // Telit
//uint8_t const 	APN[]   		= "";                   //35#   // Default value
//uint8_t const APN[]           = "c2.korem2m.com";     //35#   // Kore
//uint8_t const IP_OTA[]		= "72.197.171.234";     //36#   // OTA Server in John's home office
uint8_t const 	IP_OTA[]    	= "12.12.201.84";		//36#   // OTA Server in IC office
uint8_t const 	PORT_OTA[]      = "2021";            	//37#   

uint8_t const 	ENCRYPTION      = 1;             		//95# 
uint8_t const 	MM_COUNT        = 0;        


// Page 0 EEPROM Map - 256 bytes
typedef struct pg0_eeprom_struct
{
    // 0x00 - 0x0F
    uint8_t 	VERSION[4];         // 0x00 - 0x03
    uint8_t     FIRST_RUN;          // 0x04
    uint8_t 	reserved1[10];      // 0x05 - 0x0E
    uint8_t 	MM_COUNT;           // 0x0F
    
    // 0x10 - 0xAF
    uint8_t 	APN[32];			// 0x10
    uint8_t 	IP1[16];			// 0x30
    uint8_t 	reserved2[16];      // 0x40
    uint8_t 	IP2[16];            // 0x50
    uint8_t 	reserved3[16];      // 0x60
    uint8_t 	IP3[16];            // 0x70
    uint8_t 	reserved4[16];      // 0x80
    uint8_t 	IP4[16];            // 0x90   
    uint8_t 	reserved5[8];       // 0xA0
    
    // 0xB0 - 0xBF
    uint8_t 	PORT1[4];			// 0xA8
    uint8_t 	PORT2[4];			// 0xAC 
    uint8_t 	PORT3[4];			// 0xB0
    uint8_t     PORT4[4];			// 0xB4 
    uint8_t 	SMS_WAIT_TIME;		// 0xB8
	uint8_t 	ZONE1;				// 0xB9
	uint8_t 	ZONE2;				// 0xBA
    uint8_t 	TP_PIN;				// 0xBB
    uint8_t 	CYCLE;				// 0xBC
    uint8_t 	RETRY;				// 0xBD
	uint8_t 	reserved6[2];       // 0xBE	
	
    // 0xC0 - 0xCF
    uint8_t 	ACCESS_CODE[7];		// 0xC0
    uint8_t 	PROGRAM_ACK;		// 0xC7
    uint8_t 	TEST_FREQ;			// 0xC8
    uint8_t 	SERVER_ACK_TIME;	// 0xC9
	uint8_t 	reserved7[6];       // 0xCA
	
	// 0xD0 - 0xDF
    uint8_t 	UNIT_ACCNT[8];		// 0xD0 - 0xD7
    uint8_t 	LINE_CARD[8];		// 0xD8 - 0xDF

    // 0xE0
    uint8_t 	ENCRYPTION;			// 0xE0    
	uint8_t 	reserved8[29];		// 0xE1 - 0xFD
    uint16_t    CHECKSUM;           // 0xFE - 0xFF
	
} pg0_eeprom_map_t;

// Page 1 EEPROM Map - 256 bytes
#define DEVICE_SIZE		16
#define DEVICE_LEN		8
typedef struct pg1_eeprom_struct
{
    uint8_t     device_id_table[DEVICE_SIZE][DEVICE_LEN];       // 0x00 - 0x7F
    uint8_t     reserved1[80];                                  // 0x80 - 0xCF    
    uint8_t     IP_OTA[16];                                     // 0xD0 - 0xDF
    uint8_t     reserved2[16];                                  // 0xE0 - 0xEF
    uint8_t     PORT_OTA[4];                                    // 0xF0 - 0xF4      
    uint8_t     reserved3[10];                                  // 0xF4 - 0xFD
    uint16_t    CHECKSUM;                                       // 0xFE - 0xFF
} pg1_eeprom_map_t;
    
#define EE_PAGE_SIZE    256
#define EE_START_ADDR   0x00
union pg0_eeprom {
    uint8_t             data[EE_PAGE_SIZE];
    pg0_eeprom_map_t    map;
} page0_eeprom, page0_init;

union pg1_eeprom {
    uint8_t             data[EE_PAGE_SIZE];    
    pg1_eeprom_map_t    map;
} page1_eeprom, page1_init;


extern bool ready_for_sending;    
    

#ifdef	__cplusplus
}
#endif

#endif	/* EEPROM_H */

