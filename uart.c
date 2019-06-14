/*
 * File: uart.c
 * Author: THE MASTER
 * 
 * This file defines all functions used in control of the state machine;
 * delegates tasks associated with the 4 different states in the hub.
 * 
 * Date Created: 30 Apr. 2018, 10:19 AM
 * Last Modified: 30 Apr. 2018, 10:19 AM
 */


#include <pic18f26k22.h>
#include <xc.h>


#include "config.h"
#include "uart.h"
#include "io.h"



void UART_init(void)
{
    Enable_Uart_IO();
            
    UART1_init();
    UART2_init();
}

void UART1_init(void)
{
    // TxSTAx: TRANSMIT STATUS AND CONTROL REGISTER
    TX1STA = 0x26;       // 0010 0110 - 8-bit, 
    // RCSTAx: RECEIVE STATUS AND CONTROL REGISTER
    RC1STA = 0x90;       //1001 0000  
    // BAUDCONx: BAUD RATE CONTROL REGISTER
    BAUDCON1 = 0xc8;     //11001000
    // mlin 
    SPBRG1 = 16;        // 115200 bps
//    SPBRG1 = 103;       // 19200 bps
    SPBRGH1 = 0;    
}

//---------------------------------------------------
void UART2_init(void)
{   
    TX2STA = 0x26;       //00100110
    RC2STA = 0x90;       //10010000
    BAUDCON2 = 0xc8;     //11001000
    SPBRG2 = 207;       //9600bps
    SPBRGH2 = 0;
    WPUB5 = 1;
    RC2IE = 1;
}

////---------------------------------------------------
//void Uart_initial_115200(void)
//{
//    // TxSTAx: TRANSMIT STATUS AND CONTROL REGISTER
//    TX1STA = 0x26;       // 0010 0110 - 8-bit, 
//    // RCSTAx: RECEIVE STATUS AND CONTROL REGISTER
//    RC1STA = 0x90;       //1001 0000  
//    // BAUDCONx: BAUD RATE CONTROL REGISTER
//    BAUDCON1 = 0xc8;     //11001000
//    // mlin 
//    SPBRG1 = 16;        // 115200 bps
////    SPBRG1 = 103;       // 19200 bps
//    SPBRGH1 = 0;
//}

//---------------------------------------------------
void Uart_disable(void)
{
    RC1STA = 0x00;       //10010000
    MD_RX = 0;
    MD_RX_TRIS = OUTPUT;
    MD_TX = 1;
    MD_TX_TRIS = INPUT;
    RC1IE = 0;
    
    RC2STA = 0x00;       //10010000
   // WIFI_RX = 0;
  //  WIFI_RX_TRIS = OUTPUT;
   // WIFI_TX = 0;
   // WIFI_TX_TRIS = OUTPUT;
    RC2IE = 0;
}

//---------------------------------------------------
void Enable_Uart_IO(void)
{
    MD_RX = 1;
    MD_RX_TRIS = OUTPUT;
    MD_TX = 1;
    MD_TX_TRIS = INPUT;
}

// UART1 (to OTA/modem) ISR
void UART1_ISR()
{
    uint8_t temp;
        
    // RC1IE: EUSART1 Receive Interrupt Enable bit
    // RC1IF: EUSART1 Receive Interrupt Flag bit
    if ((RC1IE == 1) && (RC1IF == 1))
    {
        do{
        //    LED = ~LED;		
            temp = RC1REG;
        }while(RC1IF==1);
       // RC1IF = 0;
    }
}

// UART2 (to RF receiver) ISR
void UART2_ISR()
{
	uint8_t temp;
	uint8_t id[6];
    uint8_t zone,cnt;
    uint8_t temp;
		
    // RC2IE: EUSART2 Receive Interrupt Enable bit
    if ((RC2IE == 1) && (RC2IF == 1))
    {
        //update_led_state(RF_INT);
    }        	
}