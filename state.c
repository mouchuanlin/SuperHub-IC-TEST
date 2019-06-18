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

//#include "config.h"
#include "state.h"
//#include "queue.h"
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
		#ifdef MODULE_OFF_TYPE
		   MD_POWER = POWER_OFF;
		#else
			MD_RESET = 1;
		#endif
		delayseconds(1);
		#ifndef MODULE_OFF_TYPE
			MD_RESET = 0;
		#endif
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
