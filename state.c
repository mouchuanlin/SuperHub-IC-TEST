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
#include "initial.h"

extern state_t myState;

void check_state(state_t *state)
{
//    check_alarm_tamper();
//    check_button();
//    check_supervisory_NEW();
       
    switch (*state)
    {
        case INIT:
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


void check_button()
{
//    if (state == OPERATIONAL && inButtonMenu && !testButtonTimedOut)
//        ledPattern = BUTTON_MENU;
//    else if (state == OPERATIONAL && inButtonMenu && testButtonTimedOut
//            && testButtonCnt == 1)
//    {
//        testButtonCnt = 0;
//        testButtonTimedOut = false;
//        add_to_queue(Test);             // after transmitting test, change state
//                                        // to LISTEN_SMS
//    }
//    else if (state == OPERATIONAL && inButtonMenu && testButtonTimedOut
//            && testButtonCnt == 2)
//    {
//        ledPattern = SENSOR_ADD;
//        testButtonCnt = 0;
//        testButtonTimedOut = false;
//        state = ADD_SENSOR;
//    }
//    else if (state == OPERATIONAL && inButtonMenu && testButtonTimedOut
//            && testButtonCnt == 3)
//    {
//        ledPattern = SENSOR_DELETE;
//        testButtonCnt = 0;
//        testButtonTimedOut = false;
//        state = DELETE_SENSOR;
//    }
//    else if (state == OPERATIONAL && inButtonMenu && testButtonTimedOut
//            && testButtonCnt == 4)
//    {
//        ledPattern = IDLE;
//        testButtonCnt = 0;
//        testButtonTimedOut = false;
//        inButtonMenu = false;
//        state = OPERATIONAL;
//    }
//    else if (state == OPERATIONAL && !inButtonMenu && testButtonCnt == 0)
//    {
//        ledPattern = IDLE;
//    }
//    
//    if (testButtonCnt == 5 && !inButtonMenu)
//    {
//        testButtonCnt = 0;
//        inButtonMenu = true;
//    }
}

void check_alarm_tamper()
{
//    if ((alarm_int && !stillTriggered) || (tamper_int && !stillTampered))
//    {
//        __delay_ms(30);             // Debounce
//        if (alarm_pin)
//        {
//            stillTriggered = true;
//            if (isLowBattery())
//                add_to_queue(AlarmLB);
//            else
//                add_to_queue(AlarmGB);
//        }
//        if (tamper_pin)
//        {
//            stillTampered = true;
//            if (isLowBattery())
//                add_to_queue(TamperLB);
//            else
//                add_to_queue(TamperGB);
//        }
//    }
//    else if ((!alarm_int && stillTriggered) || (!tamper_int && stillTampered))
//    {
//        __delay_ms(20);
//        if (!alarm_pin)
//            stillTriggered = false;
//        if (!tamper_pin)
//            stillTampered = false;
//    }
}


void check_supervisory_NEW()
{
//    uint32_t testFreq = ee_read(TEST_FREQ);
//    if (systemTick >= (_1HR * 24 * testFreq))
//    {
//        if (isLowBattery())
//            add_to_queue(SupervisoryLB);
//        else
//            add_to_queue(SupervisoryGB);
//        systemTick = 0;
//    }
}

//
 // Primary loop that controls add_sensor 
//
void add_sensor()
{
//    if (id_buf_full())      // if device has already learned max number of sensors
//        return;             // return immediately
//    
//    tmr0Tick = 0;           // Keep track of timer0 ticks with this variable
//    start_timer0();         // control LEDs
//    CLRWDT();
//    while(tmr0Tick <= _2minutes)        // must increment tmr0Tick in tmr0 interrupt
//    {
//        if(receivedRF)
//        {
//            if(checksum_ok())          // store rf txmission in global rfBuf
//            {
//                if(!id_already_learned())
//                    learn_id();
//            }
//        }
//    }
    
}

//
// Primary loop that controls delete sensor
//
void delete_sensor()
{
//    if (id_buf_empty())     // if device hasn't learned any sensors
//        return;             // return immediately
//    
//    uint8_t id_index = 0;
//    tmr0Tick = 0;           // Keep track of timer0 ticks with this variable
//    start_timer0();         // control LEDs
//    CLRWDT();
//    while(tmr0Tick <= _2minutes)        // must increment tmr0Tick in tmr0 interrupt
//    {
//        if(receivedRF)
//        {
//            if (checksum_ok())          // store rf txmission in global rfBuf
//            {
//                if (id_already_learned(*id_index))  // put index of matching id
//                                                    // in variable id_index
//                {
//                    remove_id(id_index);
//                }
//                if (id_buf_empty())     // if device hasn't learned any sensors
//                    return;             // return immediately
//            }
//        }
//    }
}
