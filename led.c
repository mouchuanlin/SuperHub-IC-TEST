/*
 * File: led.c
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
#include "led.h"
#include "queue.h"
#include "initial.h"

void update_led_state(led_states_t new_state)
{
//    prev_led_state = curr_led_state;
//    curr_led_state = new_state;
    
    led_state = new_state;
}

void control_leds()
{
    // TODO: timer0 is setup to 100 ms per tick. Will need to re-verify the code.
    // 	each tick 100 ms = 0.1 second
    gled_tmr0_tick++;
    bled_tmr0_tick++;

    switch (led_state)
    {
        case IDLE:
            G_OFF();
            B_OFF();
            gled_tmr0_tick = 0;
            bled_tmr0_tick = 0;
            break;
            
        case POWERON:			// Green ON 0.5 sec/OFF 1.5 sec
            B_OFF();
            bled_tmr0_tick = 0;
            switch (gled_tmr0_tick)
            {
                case 1:
                    G_ON();
                    break;
                case 6:
                    G_OFF();
                    break;
                case 16:
                    gled_tmr0_tick = 0;
                    break;
            }
            break;
            
        case STANDBY:			// LISTEN_SMS Blue LED turns ON_0.5s / OFF_1.5s
            G_OFF();
            gled_tmr0_tick = 0;
            switch (bled_tmr0_tick)
            {
                case 1:
                    B_ON();
                    break;
                case 6:		
                    B_OFF();
                    break;
                case 21:	
                    bled_tmr0_tick = 0;
                    break;
            }
            break;
            
        case SENDING:               // B ON 0.1 sec/OFF 0.1 sec
            G_OFF();
            gled_tmr0_tick = 0;
            switch (bled_tmr0_tick)
            {
                case 1:
                    B_ON();
                    break;
                case 2:		
                    B_OFF();
                    break;
                case 3:		//5
                    bled_tmr0_tick = 0;
                    break;
            }
            break;
            
        case BUTTON_MENU:
            G_ON();
            B_ON();
            gled_tmr0_tick = 0;
            bled_tmr0_tick = 0;
            break;
            
        case APN_IP_ACCT_NOT_SET:       // Both LEDs blink oppositely (0.5s)
            switch(gled_tmr0_tick)
            {
                case 1:
                    G_ON();
                    B_OFF();
                    break;
                case 6:		
                    G_OFF();
                    B_ON();
                    break;
                case 11:	
                    gled_tmr0_tick = 0;
                    bled_tmr0_tick = 0;
                    break;
            }
            break;
        // ???? B ON B OFF B ON B OFF, G ON, G OFF    
        case SEND_ERR:                  // Blue-blue-green-green cross-flash (0.1s)
            switch(gled_tmr0_tick)
            {		
                case 1:
                    G_OFF();
                    B_ON();
                    break;
                case 2:		
                    G_OFF();
                    B_OFF();
                    break;
                case 3:		
                    G_OFF();
                    B_ON();
                    break;
                case 4:		
                    G_OFF();
                    B_OFF();
                    break;                    
                case 5:
                    G_ON();
                    B_OFF();
                    break;
                case 6:		
                    G_OFF();
                    B_OFF();
                    break;
                case 7:		
                    G_ON();
                    B_OFF();
                    break;
                case 8:		
                    G_OFF();
                    B_OFF();
                    break;  
                    gled_tmr0_tick = 0;
                    bled_tmr0_tick = 0;
                    break;
            }
            break;
            
        // // BTN 5-2 - G ON, B ON O.5s/OFF 2s
        case SENSOR_ADD:		
            G_ON();
            gled_tmr0_tick = 0;
            switch (bled_tmr0_tick)
            {
                case 1:
                    B_ON();
                    break;
                case 6:		
                    B_OFF();
                    break;
                case 26:	
                    bled_tmr0_tick = 0;
                    break;
            }
            break;
            
        case SENSOR_DELETE:		// SAME 5-2 different lesd
            B_ON();
            bled_tmr0_tick = 0;
            switch (gled_tmr0_tick)
            {
                case 1:
                    G_ON();
                    break;
                case 6:		
                    G_OFF();
                    break;
                case 26:		
                    gled_tmr0_tick = 0;
                    break;
            }
            break;
            
        case LINE_FAULT:            // Both LEDs ON_0.3s / OFF_4.7s
            switch (gled_tmr0_tick)
            {
                case 1:
                    G_ON();
                    B_ON();
                    break;
                case 4:		
                    G_OFF();
                    B_OFF();
                    break;
                case 50:
                    gled_tmr0_tick = 0;
                    bled_tmr0_tick = 0;
                    break;
            }
            break;
            
        case RF_INT:                // Hold here for as long as you want within
                                    // state machine
            G_ON();
            B_OFF();
            gled_tmr0_tick = 0;
            bled_tmr0_tick = 0;
            break;
    }  
}