/* 
 * File:   Main.c
 * Author: YEN
 *
 * Created on 2017?4?11?, ?? 3:13
 */

// Configuration, global variable and forward declaration.
#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <pic18f26k22.h>
#include <xc.h>

#include "System_Library.h"
#include "io.h"
#include "EE_library.h"
#include "SMS_library.h"
#include "Module_Library.h"
//#include "Module_LB_Gemalto.h"
#include "Module_LB_Telit.h"
#include "emc_library.h"

#include "state.h"
#include "led.h"
#include "timer.h"
#include "modem.h"
#include "uart.h"




// Global variables
state_t     myState = POWER_UP;
bool        readyForSleep = false;

// LED
bool            G_LED_STATE = 1, B_LED_STATE = 1;
uint8_t         gled_tmr0_tick = 0, bled_tmr0_tick = 0;
//led_states_t    curr_led_state = IDLE;
//led_states_t    prev_led_state = IDLE;
led_states_t    led_state = IDLE;
uint8_t         ver_select = 0;

// button press
bool    inButtonMenu = false;
uint8_t  buttonPressCount = 0;
uint8_t tmr3_cnt = 0;
bool    g_op_state = false;

//
// MAIN
//
int main(int argc, char** argv) 
{   
    // System init - IO, timer, ADC, UART, interrupt
    init_system();
    
    // Buzzer for 10 times when power up - PORTx Output Latch bit value
    buzzer_on(10);
    
    // Programming default configuration in EE.
    init_eeprom();
    
    // Powerup modem, send AT command to init modem.
    start_modem();

	while (TRUE)
	{   
        //g_op_state = true;
        if (g_op_state)
        {
            check_state(&myState);
        }
        else
        {
//            update_led_state(IDLE);
//            SLEEP();							
//            NOP();
        }
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
    SWDTEN = 0;     

    IO_init();
    UART_init();      
    ADC_init();
    
    // Init Timer0.
    // timer0_init();
    start_timer0();
    
    // Init interrupt
    int_init();
	
    // Smoke hub or super hub?
    ver_select = get_hub_type();
    
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
    
    // Test_click = 1;
    load_ID_to_buffer();
    
    // Smoke hub or super hub?
    ver_select = get_hub_type();
}

// TODO: Why input/output difference b/w smoke hub and super hub???
uint8_t get_hub_type()
{
    uint8_t hub_type = 0;
    
    if( VER_SELECT==1 ) //Smoker
    {
        hub_type = SMOKER;
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
    // This make sure we only run first time.
    if((read_ee(EE_PAGE0,  VER_ADDR0) == VERSION[0]) && 
		(read_ee(EE_PAGE0, VER_ADDR1) == VERSION[1]) && 
		(read_ee(EE_PAGE0, VER_ADDR2) == VERSION[2]))
        return;
    
    for (uint8_t i = 0; i < count; i++)
    {
        SPK = 1;
        delay5ms(20);
        SPK = 0;
        delay5ms(20);
        CLRWDT();
    }
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

uint8_t process_restart()
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
		return FALSE;
	}     	
	
	return TRUE;
}

void __interrupt isr()
{
	// UART1 ISR - modem
    UART1_ISR();
	
	//UART2 ISR - RF receiver
    UART2_ISR();
	
	// smoker_ISR();
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

void superhub_ISR()
{
	if (ver_select == SUPER_HUB)
    {
       if (INT0IF==1)     //Tamper SW
        {
            INT0IF = 0;
            if( tamper_status==0 )
            {
                add_event(TAMPER_OPEN_T,1);
                tamper_status = 1;        
                SPK = 0;
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

void process_button_push()
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
		else{                                                        
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
            Test_click = 1;
            add_event(GO_SMS_T,0);
            learning_mode = KEY_NONE;
            update_led_state(APN_IP_ACCT_NOT_SET);
            break;

        // learn_btn 5-2 - adding device ID
        case 2:
            learning_mode = KEY_ADD_ID;        
            update_led_state(SENSOR_ADD);					
            break;
            
        // learn_btn 5-3 - deleting device ID
        case 3:
            learning_mode = KEY_DEL_ID;     
            update_led_state(SENSOR_DELETE);					
            break;
            
        // learn_btn 5-4 - sending test alarm     
        case 4:
            add_event(TEST_PIN_T,0);
         //   send_trigger_to_RF(0);
            learning_mode = KEY_NONE;
            update_led_state(SENDING);
            break;
            
        case 5:
            learning_mode = KEY_NONE;
            break;   
    }
}




