//
// main.c
//

#include <pic18f26k22.h>
#include <xc.h>

// Configuration, global variable and forward declaration.
#include "config.h"
#include "main.h"

#include "io.h"
#include "timer.h"
#include "emc_library.h"

//
// main
//
int main(int argc, char** argv) 
{   	
    uint8_t WDT_count;
    
    // System init - IO, timer, ADC, UART, interrupt
    init_system();
    
    // Buzzer for 10 times when power up - PORTx Output Latch bit value
    buzzer_on(10);
    
    // Programming default configuration in EE.
    init_eeprom();
    
    // Powerup modem, send AT command to init modem.
    start_modem();
    
    // Turn off modem before going to infinite loop.
    LED_flash_type = LED_OFF;
    //Uart_disable();
    poweroff_modem();
    update_led_state(IDLE);

	while (1)
	{          
        // This is the case going to SLEEP mode. This portion need WDT.
        // When RF INT detected, will set RF_wait_count 100 (10 seconds) for RF communication.
        // Can't go SLEEP if we need RF communication.
        
        // TODO: Recalculate WDT value 
        if( RF_wait_count==0)
        {
			SWDTEN = 1;
			SLEEP();   
			NOP();
			NOP();
			NOP();
			SWDTEN = 0;
			
            // ~200 ms
			if( ++WDT_count>=3 )
			{            
				WDT_count = 0;

				check_button();
				control_leds();

				exit_learning_mode();
				calculate_adc_time();
               
				//TODO: use btn 5-4 now.
				// if((VER_SELECT==1) && (TEST_PIN==1))
				// {
					// if( ++test_9sec_count==45 )   //100ms*90
						// add_event(TEST_PIN_T,0);
				// }else test_9sec_count = 0;
				
				// Smoker specific 
				handle_smoker();
			}
        }

        check_state();

        
//        if (event_queue_is_empty() && STATE == IDLE && test_count == 0 && MD_STATE == MD_OFF)
//        {
//            disable_md_rxint();
//            terminate_md_uart();
//            terminate_rf_uart();
//            SLEEP();
//            NOP();
//        }
//        else if (!event_queue_is_empty())
//        {
////            send_data(get_next_event());
//        }
//        
//        SLEEP();
//        NOP();
    }
	  
    return (EXIT_SUCCESS);   
}

void init_system()
{
    // REGISTER 2-1: OSCCON: OSCILLATOR CONTROL REGISTER
    OSCCON = HIGH_FREQ_OSCCON;  // 8MHz
    //MCLRE = 0;
    HL_freq = 1;
    // Enable watchdog timer
    // WDTEN<1:0>: Watchdog Timer Enable bits
    //SWDTEN = 0;  
    CLRWDT();

    IO_init();
    UART_init();      
    ADC_init();
    
    // Init Timer0.
    // timer0_init();
    start_timer0();
    
    // Init interrupt
    int_init();
	
    init_global_variables();
    
    update_led_state(IDLE);
}

void int_init()
{
    // INTEDG0: External Interrupt 0/1/2 Edge Select bit - rising edge.
    INTEDG0 = 1;
    INTEDG1 = 1;
    INTEDG2 = 1;
    
    INT1IP = 1;     //High Priority
    INT2IP = 1;     //High Priority
    
    // IOCB: INTERRUPT-ON-CHANGE PORTB CONTROL REGISTER
    IOCB = 0x10;    //RB.4 IOC enable
    
    INTEDG0 = 0;    //Int0 falling edge
    INTEDG1 = 1;    //Int1 rising edge
    
    // Enable interrupt
    INT0IE = 1;     //Enable Int0 interrupt
    INT1IE = 1;     //Enable Int1 interrupt
    //INT2IE = 1;     //Enable Int2 interrupt
    
    RBIE = 1;       //Enable IOC interrupt

    RC1IE = 1;      //Enable UART1_RX interrupt
    PEIE = 1;       //Enable LOW&HIGH Priority interrupt   
    
    //Enable HIGH Priority interrupt 
    GIE = 1; 
}

void init_stack_buffer()
{    
	uint8_t cnt,rsp,temp;
    
    for( rsp=0;rsp<BUFFER_STACK;rsp++ )
    {
        cnt = 0;
        do{
            stack_buffer[rsp][cnt] = 0x00;
        }while(++cnt<LOG_MAX_T);
    }
}

void init_global_variables()
{
    // Global variable init
    test_count = 0;
    learning_mode = KEY_NONE;
    ADC_time = 1;
    
    load_device_id_table();
    
    // Smoke hub or super hub?
    hub_type = get_hub_type();
    
}

// TODO: Why input/output difference b/w smoke hub and super hub???
uint8_t get_hub_type()
{
    uint8_t hub_type = 0;
    
    if( VER_SELECT==1 )
    {
        hub_type = SMOKE_HUB;
        TEST_PIN = 1;
        TEST_PIN_TRIS = INPUT;
        INT2IE = 1;     //Enable Int2 interrupt
    }else{              //super hub
        hub_type = SUPER_HUB;   
        LED_RX_IN = 1;
        LED_RX_IN_TRIS = OUTPUT;
        INT2IE = 0;     //Disable Int2 interrupt
    }
    
    return hub_type;
}

void buzzer_on(uint8_t count)
{
    for (uint8_t i = 0; i < count; i++)
    {
        SPK = 1;
        delay5ms(20);
        SPK = 0;
        delay5ms(20);
        CLRWDT();
    }
}

void __interrupt isr()
{
	// UART1 ISR - modem
    UART1_ISR();
	
	//UART2 ISR - RF receiver
    UART2_ISR();
	
    smokehub_ISR();
	superhub_ISR();
	
	// TMR0 ISR
	TMR0_ISR();
	
     // button press
    // TODO: Super hub use INT1 while smoke hub use IO. Will need to check for smoke hub.
	/*
    if (INT1IF)
    {                   
        // Time out after 2s without edge detection; reset button 
        INT1IF = 0;
        INTCONbits.RBIF = 0;
        PIE2bits.TMR3IE = 1;
        T3CONbits.TMR3ON = 1;
		
        if (inButtonMenu && (buttonPressCount == 0))
            reload_timer3_5s();
		else if (buttonPressCount == 0)
			reload_timer3_2s();
        
        buttonPressCount++;                // press count if it times out.
		
        if (!inButtonMenu && (buttonPressCount == 5))
        {
            reload_timer3_5s();
            buttonPressCount = 0;
            inButtonMenu = true;
            update_led_state(BUTTON_MENU);
        }
		
		
		//test_count++;
    } */
    
    
//        if( INT1IF==1 )     //Learning
//        {
//            INT1IF = 0;
//            if( learn_delay==0 )
//            {
//                test_count++;
//                test_time_detect = 0;                             
//                learn_delay = 2;
//            }
//        }
//    
//    
    //process_button_push();
        
    // timer3 interrupt
	//TMR3_ISR(); 
}

void smokehub_ISR()
{
    uint8_t temp;
	
	if( hub_type == SMOKE_HUB )
    {
        if( INT0IF==1 )     //STANDBY(PIN4)
        {
            INT0IF = 0;
            Standby_f = 1;
        }
        if( INT1IF==1 )     //ALARM(PIN5)
        {
            //LED = 0;
            INT1IF = 0;
            if( TEST_PIN==1 )
            {
                //if( test_enable==1||first_test!=0||Test_click==1 )
                //    add_event(TEST_PIN_T,2);
            }else alarm_count = 1;                              
        }
        if( INT2IF==1 )     //ERROR(PIN6)
        {
            INT2IF = 0;
            Error_f = 1;
        }
        if( RBIF==1 )       //TEST(PIN1)    RB.4
        {
            if( TEST_PIN==1 )
            {
                test_count++;
                test_time_detect = 0;
            }
            temp = PORTB;
            NOP();
            RBIF = 0;
        }
    }
}

void superhub_ISR()
{
	if (hub_type == SUPER_HUB)
    {
       if (INT0IF==1)     //Tamper SW
        {
            INT0IF = 0;
            if( tamper_status==0 )
            {
                add_event(TAMPER_OPEN_T,1);
                tamper_status = 1;        
                SPK = 1;
            }
        }
       // Learn button pressed
        if (INT1IF==1)     //Learning
        {
            INT1IF = 0;
//            if( learn_delay==0 )
//            {
                test_count++;
                test_time_detect = 0;                             
                learn_delay = 2;
//            }
        }
    }
}

void check_button()
{
	if (test_count != 0)
	{
		test_time_detect++;
		if (learning_mode == KEY_NONE)
		{
            // learn_btn 5 in 1 second
			if( test_count==5 && test_time_detect > 10 ) //1sec
			{
                learning_mode = KEY_IN_LEARN; 
                exit_learn = 0;
                test_count = 0;
                test_time_detect = 0;
				
                update_led_state(BUTTON_MENU);
			} 
			else if( test_time_detect >= 20 )  //100ms*20=2sec
			{
				test_count = 0;
				test_time_detect = 0;
			}
		}
		else
		{              
            // Allow 30 seconds for 2nd button push
            // TODO: Is the right place for this check???
//			if( ++exit_learn>=300 )     // 300*100 ms = 30 seconds
//			{
//				learning_mode = KEY_NONE;
//				update_led_state(BUTTON_MENU);
//			}
						
			if (++test_time_detect >= 20)  //100ms*20=2sec
			{               
                sms_menu();
				test_count = 0;
				test_time_detect = 0;
			}
	    }       
	}
}

void sms_menu()
{
    // learn_btn 5-1 - SMS setup
    switch (test_count)
    {
        // learn_btn 5-1 - SMS setup state
        case 1:
			// This bit indicating we are in button 5-1 state
            listen_sms_state = 1;
            add_event(GO_SMS_T,0);
            // SMS listening state
            learning_mode = KEY_NONE;
            //update_led_state(APN_IP_ACCT_NOT_SET);
			myState = LISTEN_SMS;
            break;

        // learn_btn 5-2 - adding device ID
        case 2:
            learning_mode = KEY_ADD_ID;        
            update_led_state(SENSOR_ADD);	
			myState = ADD_SENSOR;			
            break;
            
        // learn_btn 5-3 - deleting device ID
        case 3:
            learning_mode = KEY_DEL_ID;     
            update_led_state(SENSOR_DELETE);	
			myState = DEL_SENSOR;				
            break;
            
        // learn_btn 5-4 - sending test alarm     
        case 4:
            add_event(TEST_PIN_T,0);
         //   send_trigger_to_RF(0);
            learning_mode = KEY_NONE;
            update_led_state(SENDING);
			myState = SEND_TEST;
            break;
            
        case 5:
            learning_mode = KEY_NONE;
            myState = LISTEN_SMS;	
            break;   
    }
}

