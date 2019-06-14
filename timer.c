/*
 * File: timer.c
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
#include "timer.h"
#include "queue.h"
#include "io.h"
#include "led.h"



void delay5ms(uint16_t cnt)
{
    uint8_t a,b;
    if( HL_freq==1 )
    {
        do{
            b = 18;
            do{
                a = 50;//100;
                while(--a!=0);
            }while(--b!=0);
            CLRWDT();
        }while(--cnt!=0);
    }else
    {
        do{
            a = 4;//100;
            while(--a!=0); 
            CLRWDT();
        }while(--cnt!=0);
    }
}

void delayseconds(uint16_t secs)
{
    for (uint16_t i = 0; i < secs; i++)
    {
        delay5ms(200);
        CLRWDT();
    }
}

void timer0_init()
{
    // T0CON: TIMER0 CONTROL REGISTER
    // 0x87 = b1000 0111 - Enable Timer0, 16 bit timer, 1:256 prescale value.
    // (1/8M)*4*256*781=99.9968ms
    T0CON = HIGH_FREQ_T0CON;
    // Timer0 Register, High/Low Byte
    TMR0L = ((65535-HIGH_FREQ_TMR0)%256);
    TMR0H = ((65535-HIGH_FREQ_TMR0)/256);
    TMR0IF = 0;
    TMR0IE = 1;
}

// Timer0 used to control LEDs.
void start_timer0()
{
    // T0CON: TIMER0 CONTROL REGISTER
    /* t0con = 0x87: enable timer0, use as 16-bit counter, transition on
     * internal instruction cycle, assign prescaler of: 1:256.
     * Timer0 used to control LEDs.
     */
    // 0x87 = b1000 0111 - timer 0 ON, 1:256 prescale
    T0CON = TMR0_CFG;
    // TODO: this should set timer0 to 100 ms, not 50 ms.
	// (1/8M)*4*256*781=99.9968ms
    TMR0L = ((65535-_100milliseconds)%256);//for 8MHz
    TMR0H = ((65535-_100milliseconds)/256);
    TMR0IF = 0;
    TMR0IE = 1;
    INTCONbits.GIE = 1;
}

void reload_timer0()
{
    TMR0IF = 0;
    TMR0IE = 1;
    TMR0L = ((65535-_100milliseconds)%256);//for 8MHz
    TMR0H = ((65535-_100milliseconds)/256);
}

void enable_tmr3()
{
    // T3CON: Timer3 Control Register
    //  0011 0001 
    //T3CON = 0b00110001;//    0x71;
    T3CON = 0x87;   // 1000 0111
    INTCONbits.GIE = 0;
    INTCONbits.PEIE = 1;
    PIE2bits.TMR3IE = 1;
    
    PIR2bits.TMR3IF = 0;
    INTCONbits.GIE = 1;
}

void disable_tmr3()
{
//    buttonPressCount = 0;
//    tmr3_cnt = 0;
    T3CONbits.TMR3ON = 0;
	
	TMR3IF = 0;
    PIE2bits.TMR3IE = 0;
    PIR2bits.TMR3IF = 0;
}

/* Control button press timeout*/
void reload_timer3_2s()
{
//    TMR3H = 0x30;
//    TMR3L = 0;
    
    TMR0L = ((65535-_2seconds)%256);//for 8MHz
    TMR0H = ((65535-_2seconds)/256);
    
    TMR3IF = 0;
    TMR3IE = 1;
}

void reload_timer3_5s()
{
//    TMR3H = 0x78;
//    TMR3L = 0;'
	//disable_tmr3();
    TMR0L = ((65535-_5seconds)%256);//for 8MHz
    TMR0H = ((65535-_5seconds)/256);
	//enable_tmr3();
}

void reload_timer3_100ms()
{
    TMR0L = ((65535-_100milliseconds)%256);//for 8MHz
    TMR0H = ((65535-_100milliseconds)/256);	
}


// TMR0 used for LED control and learn button process. 100 ms per tick.
void TMR0_ISR()
{
    // TMR0 Overflow Interrupt Flag bit          
    if (TMR0IF)
    {
        TMR0IF = 0;
        reload_timer0();
        control_leds();
		
		// Process learn button push events.
		check_button();
    }	
}

void TMR3_ISR()
{
	if (TMR3IF)
    //if (buttonPressCount != 0)
    {

//        reload_timer3_100ms();
//		
//        //process_sms_menu();    
//
//		process_button_push();	
    }    
}

void process_sms_menu()
{
    tmr3_cnt++;
    TMR3IF = 0;

    if (inButtonMenu && (buttonPressCount > 0))			
    {
        tmr3_cnt = 0;
        // learn_btn 5-1 - SMS setup
        if (buttonPressCount == 1)
        {
            inButtonMenu = false;       // Leave button menu if we're in it
            disable_tmr3();
            //PREV_STATE = STATE;
            myState = LISTEN_SMS;
            //update_led_state(STANDBY);
            update_led_state(APN_IP_ACCT_NOT_SET);

            // Add event
            add_event(GO_SMS_T,0);
        }
        // learn_btn 5-2 - adding device ID
        else if (buttonPressCount == 2)
        {
            inButtonMenu = false;       // Leave button menu if we're in it
            disable_tmr3();
            //PREV_STATE = STATE;
            myState = ADD_SENSOR;
            //start_sensor_tmr();
            update_led_state(SENSOR_ADD);
        }
        // learn_btn 5-3 - deleting device ID
        else if (buttonPressCount == 3)
        {
            inButtonMenu = false;       // Leave button menu if we're in it
            disable_tmr3();
            //PREV_STATE = STATE;
            myState = DEL_SENSOR;
            //start_sensor_tmr();
            update_led_state(SENSOR_DELETE);
        }
        // learn_btn 5-4 - sending test alarm              
        else if (buttonPressCount == 4)
        {
            inButtonMenu = false;       // Leave button menu if we're in it
            disable_tmr3();
//                add_event(TEST_PIN, (uint8_t)(ee_read(0x00, HUB_ZONE_ADDR)));             // If test is in queue, goes into listen mode after

            update_led_state(BUTTON_MENU);
        }    
        else if (buttonPressCount == 5)
        {
            inButtonMenu = (bool)(~inButtonMenu);       // toggle menu on/off
            tmr3_cnt = 0;
            myState = IDLE;
        }   

        buttonPressCount = 0;       // clear button presses once we extract next state info
    }
//    else if (tmr3_cnt >= 2 && inButtonMenu && buttonPressCount == 0) // 10s timeout
//    {
//        inButtonMenu = false;
//        tmr3_cnt = 0;
//        disable_tmr3();
//        
//        //update_led_state(POWERON);
//    }    
}