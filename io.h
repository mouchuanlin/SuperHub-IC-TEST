//
// io.h
//

#ifndef IO_H
#define	IO_H

#ifdef	__cplusplus
extern "C" {
#endif
    
/*****************************************************
 * INCLUDES
 ****************************************************/       
#include "stdint.h"
#include "stdbool.h"
    
/*****************************************************
 * FUNCTION PROTOTYPES
 ****************************************************/  
void ADC_init(void);
void IO_init(void);    
    
/*****************************************************
 * VARIABLES
 ****************************************************/    
//#define M_866
//#define M_910
//#define DEBUG
#define del_ok
#define send_ok
//--------------------
    
#define OUTPUT  0
#define INPUT   1
   
////// PORTA
// RA0
#define BAT_DECT        PORTAbits.RA0
#define BAT_DECT_TRIS   TRISAbits.TRISA0
// RA1     
#define MD_RESET        LATAbits.LATA1
#define MD_RESET_TRIS   TRISAbits.TRISA1
// RA2    
#define MD_START        LATAbits.LATA2
#define MD_START_TRIS   TRISAbits.TRISA2
// RA3    
#define MD_POWER        LATAbits.LATA3
#define MD_POWER_TRIS   TRISAbits.TRISA3

///// PORTC
// RC1
#define VER_SELECT      LATCbits.LATC1
#define VER_SELECT_TRIS TRISCbits.TRISC1   
// RC3 
#define SPK             LATCbits.LATC3
#define SPK_TRIS        TRISCbits.TRISC3
    
    
//================================================
///// LED
// RC4 - LED1 - PCB marking, or LED3 - panel
#define LED_B           LATCbits.LATC4
#define LED_B_TRIS      TRISCbits.TRISC4
// RB3    
#define LED_G           LATBbits.LATB3
#define LED_G_TRIS      TRISBbits.TRISB3
    
// Learn button
#define TEST_PIN        PORTBbits.RB4
#define TEST_PIN_TRIS   TRISBbits.TRISB4

// RB4 - LED3 (SCHEMATICS)
#define LED_RX_IN       LATBbits.LATB4
#define LED_RX_IN_TRIS  TRISBbits.TRISB4
    
// RC5 - LED2 - PCB marking, or LED4 - panel
#define LED_RX_OUT      LATCbits.LATC5
#define LED_RX_OUT_TRIS TRISCbits.TRISC5    

//=================================================    
//PORTC
#define B_LED               LATCbits.LC4        // Illuminates when RC4 goes low. Use WPU on this pin
#define B_LED_TRIS          TRISCbits.TRISC4
//PORTB
#define G_LED               LATBbits.LB3        // Illuminates when RB3 goes low. Use WPU on this pin
#define G_LED_TRIS          TRISBbits.TRISB3

#define OFF                 1           // for LEDs, current sink
#define ON                  0    
//================================================
//================================================
    
// RC6
#define MD_RX           PORTCbits.RC6
#define MD_RX_TRIS      TRISCbits.TRISC6
// RC7
#define MD_TX           LATCbits.LATC7
#define MD_TX_TRIS      TRISCbits.TRISC7
    
///// PORTB
// RB0 - Smoke hub ONLY
#define STANDBY_PIN         PORTBbits.RB0
#define STANDBY_PIN_TRIS    TRISBbits.TRISB0  
// RB1 - Smoke hub ONLY  
#define ALARM_PIN           PORTBbits.RB1
#define ALARM_PIN_TRIS      TRISBbits.TRISB1   
// RB0  
#define TAMPER_PIN         PORTBbits.RB0
#define TAMPER_PIN_TRIS    TRISBbits.TRISB0
// RB1    
#define LEARN_PIN           PORTBbits.RB1
#define LEARN_PIN_TRIS      TRISBbits.TRISB1
// RB2 - Smoke hub ONLY
#define ERROR_PIN           PORTBbits.RB2
#define ERROR_PIN_TRIS      TRISBbits.TRISB2    
    
#define WIFI_INT             PORTBbits.RB5
#define WIFI_INT_TRIS        TRISBbits.TRISB5
#define WIFI_RX             LATBbits.LATB6
#define WIFI_RX_TRIS        TRISBbits.TRISB6
#define WIFI_TX             PORTBbits.RB7
#define WIFI_TX_TRIS        TRISBbits.TRISB7
    
//-----------------
#define HIGH_FREQ_OSCCON 0x68   
#define HIGH_FREQ_T0CON 0x87
#define HIGH_FREQ_TMR0  781     //4M->390       (1/8M)*4*256*781=99.9968ms
//-----    
#define LOW_FREQ_OSCCON 0x00   
#define LOW_FREQ_T0CON 0x80
#define LOW_FREQ_TMR0  383      //360           (1/31.25K)*4*2*390=99.84ms
//--------------------
#define START_MM_COUNT 0x00
#define END_MM_COUNT 0xA0
#define RETRY_NUMBER1 	(5-1)
#define RETRY_NUMBER2 	(3-1)    
#define BUFFER_STACK    6
#define LOG_MAX_T   220   
    
   
//--------------------   
// LED state
#define LED_OFF 0
#define LED_STANDBY 1
#define LED_INTERNET 2
#define LED_NO_SET 3
#define LED_NET_ERR 4
#define LED_REGISTER 5
#define LED_RF_ADD 6 
#define LED_RF_DEL 7 
#define MD_POWER_LOSS 8 

#define POWER_ON    9
#define RF_INT      10
//--------------------       
#define EVENT_MAX 50

// TODO: Supposed to be send oout once every 7 days
#define BT_EOL_RESEND       7       // days
//--------------------
//#ifdef M_866
    #define POWER_ON  0   
    #define POWER_OFF 1
/*#else
    #define POWER_ON  0    
    #define POWER_OFF 1
#endif*/

// Modem chip. Smoke hub use Telit LE866, super hub use Telit LE910.
#define EMS31   0
#define PLS8    1
#define LE910   2
#define LE866   3
#define EHS5    4

#define GEMALTO     (Module_type==EMS31||Module_type==PLS8||Module_type==EHS5)
#define TELIT       (Module_type==LE910||Module_type==LE866)
    
// SMS listening state
// TODO: Why not 5-4???
#define KEY_NONE        0       // btn 5
#define KEY_IN_LEARN    1       // btn 5-1
#define KEY_ADD_ID      2       // btn 5-2
#define KEY_DEL_ID      3       // btn 5-3
    
// Hub type - smoke hub or super hub
#define SMOKE_HUB       0
#define SUPER_HUB       1
//--------------------
uint8_t const VERSION[]="1011";
//--------------------       
bit HL_freq = 0;
//-------------------- 
uint8_t const encryption_code[16]={ 0x6c,0x31,0x6e,0x79,0x52,0x7f,0x26,0x6f,
                                    0x34,0x19,0x41,0x1b,0x54,0x3a,0x64,0x1f };
    
//--------------------
uint8_t encryption = 0;
uint8_t rsp_buffer[250];
uint8_t enc_cnt;
uint8_t stack_buffer[BUFFER_STACK][LOG_MAX_T];
uint8_t random_rx;

// TODO: This should be a array of struct.
uint8_t event_log[EVENT_MAX][2];
uint8_t event_count_f = 0;
uint8_t event_count_l = 0;

#ifdef DEBUG
uint8_t debug_buffer[250];
#endif
uint8_t debug_p;
uint8_t signal_val;
uint8_t key_p,key[100];
uint8_t back_door;
uint8_t respond_day = 0;
uint8_t LED_flash_type = LED_OFF;
uint16_t retry_count = 0;
uint8_t IP_type = 0;
uint8_t test_enable=0;
uint16_t ADC_data = 0;

uint8_t RF_wait_count = 0;

// RF slave device ID table
uint8_t device_id_table[28][9];
uint8_t learning_mode = 0;
uint8_t power_status=0;
uint8_t chk_supervisory = 0;

uint8_t Module_type = EMS31;


#ifdef	__cplusplus
}
#endif

#endif	/* IO_H */

