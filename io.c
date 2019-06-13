//
// io.c
//



#include <pic18f26k22.h>
#include <xc.h>

#include "io.h"

void IO_init(void)
{       
//PORTA
    ANSELA = 0x01;
    BAT_DECT = 1;
    BAT_DECT_TRIS = INPUT;
    
    MD_RESET = 1;
    MD_RESET_TRIS = OUTPUT;   
    
    MD_START = 0;
    MD_START_TRIS = OUTPUT;
    MD_POWER = 1;   //-----
    MD_POWER_TRIS = OUTPUT;

//PORTC
    ANSELC = 0x00;    
    SPK = 1;
    SPK_TRIS = OUTPUT;    
    VER_SELECT = 1;
    VER_SELECT_TRIS = INPUT; 
    LED_B = 1;
    LED_B_TRIS = OUTPUT;
    LED_RX_OUT = 1;      
    LED_RX_OUT_TRIS = OUTPUT;            
    LED_G = 1;      
    LED_G_TRIS = OUTPUT;
    MD_RX = 1;
    MD_RX_TRIS = OUTPUT;
    MD_TX = 1;
    MD_TX_TRIS = INPUT;
    
//PORTB
    ANSELB = 0x00;    
    TAMPER_PIN = 1;
    TAMPER_PIN_TRIS = INPUT;
    
    // PORTBbits.RB1 - learn pin (button press)
    LEARN_PIN = 1;
    LEARN_PIN_TRIS = INPUT;
    
    ERROR_PIN = 1;
    ERROR_PIN_TRIS = INPUT;

    LED_RX_IN = 1;
    LED_RX_IN_TRIS = OUTPUT;
    
    WIFI_INT = 1;
    WIFI_INT_TRIS = INPUT;         
    WIFI_RX = 1;
    WIFI_RX_TRIS = OUTPUT;
    WIFI_TX = 1;
    WIFI_TX_TRIS = INPUT;
    
}


//---------------------------------------------------
void ADC_init(void)
{
    ADCON0 = 0b01111100;
    ADCON1 = 0b00000000;
    ADCON2 = 0b10101111;
    VREFCON0 = 0b00100000;
}

