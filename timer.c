//
// timer.c
//

#include "timer.h"
#include "emc_library.h"
#include "io.h"
#include "led.h"

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
        
        // Control RF LED2/3 ON/OFF. ON for 1 second.
        if( led_count!=0 )
        {
            if( --led_count==0 )
            {
                LED_RX_IN = 1;
                LED_RX_OUT = 1;
            }
        }
        
        // Don't go SLEEP for 10 seconds for RF communication.
        if( RF_wait_count!=0 )            
            --RF_wait_count;
        
        // 60 seconds to exit add/del sensor mode.
        exit_learning_mode();
        calculate_adc_time();
        
        if (hub_type == SUPER_HUB)
            check_alarm_tamper();
		
		// Smoker specific 
		handle_smoke_hub();
    }	
}

//void TMR3_ISR()
//{
//	if (TMR3IF)
//    //if (buttonPressCount != 0)
//    {
//
////        reload_timer3_100ms();
////		
////        //process_sms_menu();    
////
////		process_button_push();	
//    }    
//}

void handle_smoke_hub()
{
	if( hub_type == SMOKE_HUB )
	{
		if( alarm_count!=0 )
		{
			if( ALARM_PIN==1 )
			{
				if( ++alarm_count>=10 )
				{
					add_event(SMOKE_ALARM_T,1); 
				   // send_trigger_to_RF(1);
					alarm_count = 0;
				}
			}else alarm_count = 0;
		}
		if( ERROR_PIN==1 )
		{
			if( ++err_count>=10 )   //100ms*10 = 1 sec
			{
				if( err_count==10 )
				{
				 //   if( alarm_out(TAMPER_OPEN_T)==0 )
				  //      Tamper_open_f = 1;
						add_event(TAMPER_OPEN_T,1);
				}
				err_count = 10;
			}
		}else
		{
			if( err_count>=10 )
			{
			  //  if( alarm_out(TAMPER_CLOSE_T)==0 )
				//    Tamper_close_f = 1;
					add_event(TAMPER_CLOSE_T,1);
			}
			err_count = 0;
		}   
	
		if( Standby_f==1 )
		{
			Standby_f = 0;
			standby_count = 0;
		}else{
			standby_count++;            //100ms*4000=400Sec
			if( standby_count>= 4000)
			{
				standby_count = 0;
				Smoke_respond = 1;
			}
		}
	
		if( Error_f==1 )
		{
			Error_f = 0;
			error_count++;
			error_time_detect = 0;
		}else
		{            
			if( error_count!=0 )
			{
				if( ++error_time_detect>=300 )  //100ms*300=30sec
				{                    
					if( error_count==1 )            
					{
						if( ++error_status_count>=5 )
						{
							if( BT_S_respond==0 )
							{
								add_event(LOW_BATTERY_T,1);
								error_status_count = 0;
								BT_S_respond = BT_EOL_RESEND;
							}
						}
					}else if( error_count==3 )
					{
						if( ++error_status_count>=5 )
						{
							if( EOL_respond==0 )
							{
								add_event(EOL_T,1);
								error_status_count = 0;
								EOL_respond = BT_EOL_RESEND;
							}
						}
					}else error_status_count = 0;
					error_count = 0;
					error_time_detect = 0;                                            
				}
			}else
			{
				if( ++error_time_detect>=300 )  //100ms*300=30sec
				{ 
					error_time_detect = 0;   
					error_status_count = 0;                    
				}
			}
		} 
    }             
}

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


// From Jens' test code 001/module.c
//void delay5ms(uint16_t cnt)         // Based on 4MHz Fosc, or 1MHz instruction cycle
//{
//    uint8_t i, j;
//    while(cnt-- != 0)
//    {
//        j = 100;
//        while(j-- != 0)
//        {
//            i = 32;     //50;
//            while(--i != 0);
//        }
//    }
//}

void delayseconds(uint16_t secs)
{
    for (uint16_t i = 0; i < secs; i++)
    {
        delay5ms(200);
        CLRWDT();
    }
}

//void timer0_init()
//{
//    // T0CON: TIMER0 CONTROL REGISTER
//    // 0x87 = b1000 0111 - Enable Timer0, 16 bit timer, 1:256 prescale value.
//    // (1/8M)*4*256*781=99.9968ms
//    T0CON = HIGH_FREQ_T0CON;
//    // Timer0 Register, High/Low Byte
//    TMR0L = ((65535-HIGH_FREQ_TMR0)%256);
//    TMR0H = ((65535-HIGH_FREQ_TMR0)/256);
//    TMR0IF = 0;
//    TMR0IE = 1;
//}

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

//void enable_tmr3()
//{
//    // T3CON: Timer3 Control Register
//    //  0011 0001 
//    //T3CON = 0b00110001;//    0x71;
//    T3CON = 0x87;   // 1000 0111
//    INTCONbits.GIE = 0;
//    INTCONbits.PEIE = 1;
//    PIE2bits.TMR3IE = 1;
//    
//    PIR2bits.TMR3IF = 0;
//    INTCONbits.GIE = 1;
//}

//void disable_tmr3()
//{
////    buttonPressCount = 0;
////    tmr3_cnt = 0;
//    T3CONbits.TMR3ON = 0;
//	
//	TMR3IF = 0;
//    PIE2bits.TMR3IE = 0;
//    PIR2bits.TMR3IF = 0;
//}

/* Control button press timeout*/
//void reload_timer3_2s()
//{
////    TMR3H = 0x30;
////    TMR3L = 0;
//    
//    TMR3L = ((65535-_2seconds)%256);//for 8MHz
//    TMR3H = ((65535-_2seconds)/256);
//    
//    TMR3IF = 0;
//    TMR3IE = 1;
//}

//void reload_timer3_5s()
//{
////    TMR3H = 0x78;
////    TMR3L = 0;'
//	//disable_tmr3();
//    TMR3L = ((65535-_5seconds)%256);//for 8MHz
//    TMR3H = ((65535-_5seconds)/256);
//	//enable_tmr3();
//}

//void reload_timer3_100ms()
//{
//    T3CON = 0x87;
//    TMR3L = ((65535-_100milliseconds)%256);//for 8MHz
//    TMR3H = ((65535-_100milliseconds)/256);	
//}

void reload_timer3_50ms(void)
{
    // 0111 0001 - Fosc, 1:8 pre-scale, enable
    T3CON = 0x71;
    TMR3H = 0x40;   //50ms
    TMR3L = 0;    
}

void exit_learning_mode()
{
    if( ++exit_learn>=600 )     //600*100ms=60Sec
        learning_mode = KEY_NONE;
}

/*
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
            update_led_state(ADD_SENSOR);
        }
        // learn_btn 5-3 - deleting device ID
        else if (buttonPressCount == 3)
        {
            inButtonMenu = false;       // Leave button menu if we're in it
            disable_tmr3();
            //PREV_STATE = STATE;
            myState = DEL_SENSOR;
            //start_sensor_tmr();
            update_led_state(DEL_SENSOR);
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
            myState = OFF;
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
*/

void calculate_adc_time()
{
    // Use timer0 to calculate.
    if( ++adc_count >= (18000*0.9888) )       //[3600*10]*100ms=3600sec=1hr            //0.979  -1.56
    // TODO: FOR TESTING ONLY
    //if( ++adc_count >= 200 )       //[3600*10]*100ms=3600sec=1hr            //0.979  -1.56
    {            
        chk_supervisory++;    //----add supervisory
        adc_count = 0;   
        //if( ++Respond_T_Hour >= 24 )       // 24 hours
        if( ++Respond_T_Hour >= 2 )       // 24 hours
        {
            Respond_T_Hour = 0;
            Respond_T_Day++;
            // How often for supervisory message. respond_day is in EEPROM TESTING_FREQ_ADDR address.
            // TODO: FOR TESTING ONLY
            //respond_day = 0x05;
            //if( Respond_T_Day >= respond_day )   
            if( Respond_T_Day >= 0x02 )  
            {
                Respond_T_Day = 0;
                add_event(TEST_CYCLE_S,0);             
                response_low_batt();
                OTA_flag = 1;
            }
            // BT_S_response - battery for smoke hub (smoke hub only)
            // BT_S_response - battery for board (for both smoke/super hub)
            if( BT_S_respond!=0 )
                BT_S_respond--;
            if( BT_L_respond!=0 )
                BT_L_respond--;
            if( EOL_respond!=0 )
                EOL_respond--;
            
            if( OTA_flag==2 )
                OTA_flag = 1;
        }            
    }
    else if( (adc_count%300)==0 )        //every 10 mins check low battery
    {
        ADC_time = 1;
    }

}

// Check one per 24 hours
// Once not receive the data, then report device failure
void response_low_batt()
{
    uint8_t cnt1;
    
    // smoke hub battery
    for( cnt1=0;cnt1<16;cnt1++ )    //28
    {
        if( device_id_table[cnt1][0]!=0 )
        {
            // RF Battery
            if( device_id_table[cnt1][6]=='B' )                  
                add_event(LOW_BATTERY_T,cnt1+3U); 
            // RF periodical report
            if( device_id_table[cnt1][7]=='S' )                  
                add_event(SUPERVISORY_T,cnt1+3U);            
        }
    }
    CLRWDT();
}
