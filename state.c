/*
 * File: state.c
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


#include "state.h"
#include "io.h"

extern state_t myState;

void check_state()
{
//    check_alarm_tamper();
    check_button();
    control_leds();
    
    process_event_queue();
//    process_ADC();
//    check_supervisory();()
    
    
    if (process_restart())
    {
        myState = SEND_TEST;
    }
    
    switch (myState)
    {
        case POWER_UP:
            break;
        case SLEEP:
            break;            
        case LISTEN_SMS:
			//restart_modem();
			start_modem();
            break;
        case ADD_SENSOR:
            add_sensor();
            break;   
        case DEL_SENSOR:
            delete_sensor();
            break; 
        case SEND_TEST:
            start_modem();
            break;      
        case RF_INTERRUPT:
            break;
        case TEMPER_INT:
            break;
        case SUPERVISORY:
            break;
        case ADC:
            break;            
    }
}

bool process_restart()
{	
	if( (retry_count == 0 ) && (stack_buffer[0][0] != 0) && (IP_type == 1) )        
	{                   
		MD_POWER = POWER_OFF;
		delayseconds(1);
        MD_RESET = 0;

		//goto module_start;
		return true;
	}     	
	
	return false;
}

void check_alarm_tamper()
{

}

//
 // Primary loop that controls add_sensor 
//
void add_sensor()
{

    
}

//
// Primary loop that controls delete sensor
//
void delete_sensor()
{

}

void process_ADC()
{
	unsigned cnt;
	
	if( ADC_time==1 )
	{
		FVREN = 1;
		cnt = 200;
		do{
			delay5ms(1);
			CLRWDT();
		}while(--cnt!=0&&FVRST==0);
		
		ADON = 1;            
		GO_nDONE = 1; 
		cnt = 200;
		do{
			delay5ms(1);
		}while( ((ADCON0&0x02)!=0)&&(--cnt!=0) );
		
		ADC_data = (ADRESH<<8)+ADRESL;
		ADC_data &= 0x03ff;
		ADON = 0;                               //748->2.80
		ADC_time = 0;                           //776->2.70
		if( ADC_data >776 )                     //806->2.60
		{                                       //838->2.50
			if( BT_L_respond==0 )
			{                    
				add_event(LOW_BATTERY_S,0);                    
				BT_L_respond = BT_EOL_RESEND;
			}
		}else BT_L_respond = 0;
		FVREN = 0;
	}
}

void process_RF_interrupt()
{
	if( WIFI_INT ==1 && RF_wait_count == 0 )
	{
		LED_G = 0;
		OSCCON = HIGH_FREQ_OSCCON;	// 4MHz
		T0CON = HIGH_FREQ_T0CON;             //1*4000 = 50,000us
		HL_freq = 1;
		UART2_init();
		CREN1 = 0;
		RF_wait_count = 100;      
		TMR0IE = 1;//
		TMR0ON = 1;
		CREN1 = 1;
		RC2IE = 1;
	}
}

void process_supervisory()
{
	if( chk_supervisory>=2 )       //2 hour
	{
		check_supervisory();
		chk_supervisory = 0;
	}
}
