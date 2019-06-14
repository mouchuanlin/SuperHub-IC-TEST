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

#include "config.h"
#include "state.h"
#include "queue.h"
#include "io.h"

extern state_t myState;

void check_state()
{
//    check_alarm_tamper();
    check_button();
    control_leds();
//    check_supervisory_NEW();
       
    switch (myState)
    {
        case POWER_UP:
            break;
        case SLEEP:
            break;            
        case LISTEN_SMS:
            break;
        case ADD_SENSOR:
            break;   
        case DEL_SENSOR:
            break; 
        case SEND_TEST:
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

void check_alarm_tamper()
{

}


void check_supervisory_NEW()
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
