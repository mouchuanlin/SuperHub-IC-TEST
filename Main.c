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
#include "initial.h"
#include "EE_library.h"
#include "SMS_library.h"
#include "Module_Library.h"
#include "Module_LB_Gemalto.h"
#include "Module_LB_Telit.h"
#include "emc_library.h"

#include "state.h"

// Global variables
state_t myState = POWER_UP;
bool readyForSleep = false;

// LED
bool            G_LED_STATE = 1, B_LED_STATE = 1;
uint8_t         gled_tmr0_tick = 0, bled_tmr0_tick = 0;
//led_states_t    curr_led_state = IDLE;
//led_states_t    prev_led_state = IDLE;
led_states_t    led_state = IDLE;
uint8_t         ver_select = 0;


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
            update_led_state(IDLE);
            SLEEP();							
            NOP();
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
    Uart_initial();      
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

void timer0_init()
{
    // T0CON: TIMER0 CONTROL REGISTER
    // 0x87 = b1000 0111 - Enable Timer0, 16 bit timer, 1:256 prescale value.
    // TODO: how long per tick??? 50,000 us = 0.05 sec
    T0CON = HIGH_FREQ_T0CON;             //1*4000 = 50,000us
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
    T0CON = TMR0_CFG;             //1*4000 = 50,000us
    TMR0L = ((65535-_100milliseconds)%256);//for 8MHz
    TMR0H = ((65535-_100milliseconds)/256);
    TMR0IF = 0;
    TMR0IE = 1;
    INTCONbits.GIE = 1;
}

void reload_timer0()
{
    TMR0L = ((65535-_100milliseconds)%256);//for 8MHz
    TMR0H = ((65535-_100milliseconds)/256);
}

void enable_timer3()
{
    T3CON = 0b00110001;//    0x71;
    INTCONbits.GIE = 0;
    INTCONbits.PEIE = 1;
    PIE2bits.TMR3IE = 1;
    PIR2bits.TMR3IF = 0;
    INTCONbits.GIE = 1;
}
/* Control button press timeout*/
void reload_timer3_2s()
{
    TMR3H = 0x30;
    TMR3L = 0;
}

void reload_timer3_5s()
{
    TMR3H = 0x78;
    TMR3L = 0;
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

void start_modem()
{
//    CLRWDT();
//    MD_POWER = POWER_ON;
//    //__delay_ms(25000);
//    delayseconds(25);
        
    update_led_state(POWERON);
       
    powerup_modem();
    
    while (!md_config_ok())
        restart_modem();
    //md_started = true;
}

bool md_config_ok()
{
    if (!wait_AT_cmd_response())        
		return false;
        
    //-------- wait SIM ready---------
    if (!check_SIM_state())		
		return false;
 
    //-------- wait register network---------
    if (!check_network_registration())
		return false;
    
//    //--------- Alarm or Report ---------    
//
//	if (!alarm_or_report())
//		return false;
	
    //--------- Wait SMS Setting ---------
	if (!wait_SMS_setting())
    	return false;
    
    return true;
}

void restart_modem()
{
    CLRWDT();
    MD_POWER = POWER_OFF;
//    __delay_ms(5000);
    delayseconds(5);
//    MD_POWER = POWER_ON;
////    __delay_ms(25000);
//    delayseconds(25);
    
    powerup_modem();
}

void stop_modem()
{
    MD_POWER = POWER_OFF;
//    __delay_ms(5000);
    //md_started = false;
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

void powerup_modem()
{
    MD_POWER = POWER_ON;
    delayseconds(2);
    MD_RESET = 1;
    delayseconds(1);
    MD_RESET = 0;   
    delayseconds(2);
    
    MD_START = 0;
    delayseconds(3);
    MD_START = 1;   
    
    // delay 25 seconds for modem to power up
    delayseconds(25);
    
    MD_START = 0;    
}

uint8_t wait_AT_cmd_response()
{
	uint8_t cnt,rsp,temp;
		
	rsp = 15;
    
    // STEPS change UART1 baudrate to 19200 bps
    //  1. Init UART 115200
    //  2. +IRP=19200
    //  3. Init UART 19200
    //  4. 
    do{         
        ///// STEP 1. - change baudrate to 19200 bps in this function.
        //Uart_initial_115200();
        //UART1_init();
        // mlin - setup modem baud rate
        ///// STEP 2. - +IPR to set modem baudrate to 19200 bps
        soutdata("AT+IPR=115200\r\n$");
        //soutdata("AT+IPR=19200\r\n$");
        wait_ok_respond(40);
        //Uart_initial();
        cnt = check_module_run();
        if( cnt!='K' )
        {
            delayseconds(3);
            ///// STEP 3. - We probably do need this since ???
            //Uart_initial_115200();
            UART1_init();
            cnt = check_module_version(1);        
            if( cnt=='K' )
            {
                if( Module_type == PLS8 )
                    cnt = 'K';
                else cnt = 'E';
            }
        }
        CLRWDT();
    }while( --rsp!=0&&cnt!='K' );     
    if( rsp==0 )
    {        
        #ifdef MODULE_OFF_TYPE
            MD_POWER = POWER_OFF;
        #else
            MD_RESET = 1;
        #endif
        cnt = 30;
        do{
            delayseconds(1);
        }while(--cnt!=0);
        #ifndef MODULE_OFF_TYPE
            MD_RESET = 0;
        #endif
        power_status = MD_POWER_LOSS;
        //goto module_start;
		return FALSE;
    }   
       
    //---------------------------------   
    wait_ok_respond(40);
    cnt = 10;
    do{
        rsp = check_module_version(cnt);
        if( rsp=='E' )
            power_status = MD_POWER_LOSS;
        else power_status = 0;
        CLRWDT();
    }while(--cnt!=0&&rsp=='E');
	
    wait_ok_respond(40);
    if( GEMALTO )
        GM_module_first_run();
    else TL_module_first_run();    
    //--------------------------------
    delay5ms(100);
    soutdata("AT+VER=$");
    
    // TODO: This should no functional at all???
    if( VER_SELECT==1 ) //Smoker
    {
        soutdata("Smoker,$");
    }else soutdata("Super HUB,$");
	
    if( Module_type==EMS31 )    
        soutdata("EMS31\r\n$");
    else if( Module_type==PLS8 )    
        soutdata("PLS8\r\n$");    
    else if( Module_type==LE910 )    
        soutdata("LE910\r\n$");    
    else //if( Module_type==LE866 )    
        soutdata("LE866\r\n$");    
    delayseconds(1);
	
	
	return 1;
}

uint8_t check_SIM_state()
{
	uint8_t cnt,rsp,temp;
		
	cnt = 50;
    do{       
        rsp = check_sim_card();
        if( rsp=='N'&&cnt==30 )        
         {
            #ifdef MODULE_OFF_TYPE
                MD_POWER = POWER_OFF;
            #else
                MD_RESET = 1;
            #endif
			
			// TODO: Why 30 secs here???
			delayseconds(30);
			
            #ifndef MODULE_OFF_TYPE
                MD_RESET = 0;
            #endif
			
            power_status = MD_POWER_LOSS;
			
            //goto module_start;
			return FALSE;
        } 
        CLRWDT();
    }while( rsp!='K'&& --cnt!=0 );
    if( cnt==0 )    
    {
        #ifdef MODULE_OFF_TYPE
            MD_POWER = POWER_OFF;
        #else
            MD_RESET = 1;
        #endif
		
		// TODO: Why 30 secs here???
		delayseconds(30);
		
        #ifndef MODULE_OFF_TYPE
            MD_RESET = 0;
        #endif
        //goto module_start;
		return FALSE;
    }
	
	return TRUE;
}

uint8_t check_network_registration()
{
	uint8_t cnt,rsp,temp;
		
	//-------- wait register network---------
    cnt = 50;
    do{
        if( cnt%2==0 )
            rsp = check_register(0);
        else 
            rsp = check_register(1);
        CLRWDT();
    }while( rsp!='K' && --cnt!=0 );
    if( cnt==0 )    
    {
        #ifdef MODULE_OFF_TYPE
            MD_POWER = POWER_OFF;
        #else
            MD_RESET = 1;
        #endif
		// TODO: Why 30 secs here???
		delayseconds(30);
		
        #ifndef MODULE_OFF_TYPE
            MD_RESET = 0;
        #endif
        //goto module_start;
		return FALSE;
    }
    power_status = 0;
    delay5ms(100);
    // mlin - AT+CSQ doesn't seems get response???
    rsp = check_csq();
    check_led_type();
    // mlin - why "AT\\Q0\r\n$"
    soutdata("AT\\Q0\r\n$");
    
//    soutdata(AT+CMGS=\"5665776987\"\r\n$");
    
    //OTA_flag = 1; //----------------------------------
	
	return TRUE;    
}

uint8_t wait_SMS_setting()
{
	uint8_t cnt,rsp,temp;
    uint8_t sms_time;
		
	//    Test_click = 0;
    if( Test_click==1 )    
    {
        sms_time = read_ee(0x00,0xB8);   //wait SMS time
        set_sms_init();    
        do{
            cnt = 12;
            do{
                rsp = 16;
                do{
                    delayseconds(1);
                    if( event_count_f!=event_count_l)     
                        check_event(); 
                    if( stack_buffer[0][0]!=0&&retry_count==0&&IP_type==1 )   
                    {                   
                       //goto alarm_start;
					   return FALSE;
                    }
                }while(--rsp!=0);
              //  LED = 0;
                check_sms();
                delay5ms(100);
                if( check_csq()==0 )
                {
                    ///// STEP 4. - Comment Uart_initial_115200() out. Call Uart_initial() to set to 19200.)
                    //Uart_initial_115200();
                    UART1_init();
                    //Uart_initial(); 
                    //soutdata("AT+IPR=19200\r\n$");
                    //wait_ok_respond(40);
                    //Uart_initial();
                    set_sms_init();    
                }
              //  LED = 1;
            }while(--cnt!=0);      
            CLRWDT();
        }while(--sms_time!=0);
        Test_click = 0;
    }
    if( event_count_f!=event_count_l)     
        check_event();
    if( stack_buffer[0][0]!=0&&retry_count==0&&IP_type==1 )//LED_flash_type==LED_STANDBY )                     
        //goto alarm_start;
		return FALSE;
		
		
	return TRUE;
}

    
void process_event_queue()
{
	// Event queue is not empty
	if (event_count_f != event_count_l)    
	{
		check_event(); 
		retry_count = 0;        // new add on V104
	}
}

uint8_t process_SMS_setup_state()
{
	if ( Test_click==1 )
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

//void __interrupt() INTERRUPT_InterruptManagerHigh (void)
//void interrupt tc_int( void )
void __interrupt isr()
{
	// UART1 ISR - modem
    UART1_ISR();
	
	//UART2 ISR - RF receiver
    UART2_ISR();
	
	// smoker_ISR();
	//superhub_ISR();
	
	// TMR0 ISR
	TMR0_ISR();
}

// UART1 (to OTA/modem) ISR
void UART1_ISR()
{
    uint8_t temp;
    uint8_t junk;
        
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

void TMR0_ISR()
{
    // TMR0 Overflow Interrupt Flag bit          
    if (TMR0IF)
    {
        TMR0IF = 0;
        reload_timer0();
        control_leds();
    }	
}

void smoker_ISR()
{
    uint8_t temp;
	
	if( ver_select == SMOKER )
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
            if( TEST_PIN==1 )            //只計算正緣觸發
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
	if( ver_select == SUPER_HUB )
    {
       if( INT0IF==1 )     //Tamper SW
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
        if( INT1IF==1 )     //Learning
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

void handle_smoker()
{
	if( ver_select == SMOKER )
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

void handle_learn_btn_pressed()
{
	if( test_count!=0 )
	{
		test_time_detect++;
		if( learning_mode == KEY_NONE )
		{
            // learn_btn 5 in 1 second
			if( test_count==5&&test_time_detect>10 ) //1sec
			{
			   learning_mode = KEY_IN_LEARN;
			   exit_learn = 0;
			   test_count = 0;
				test_time_detect = 0;
			}else if( test_time_detect>=20 )  //100ms*20=2sec
			{
				test_count = 0;
				test_time_detect = 0;
			}
		}else{                                                        
			if( ++test_time_detect>=20 )  //100ms*20=2sec
			{               
                // learn_btn 5-1 - SMS setup
				if( test_count==1 )
				{
                    // SMS setup state
					Test_click = 1;
					add_event(GO_SMS_T,0);
					learning_mode = KEY_NONE;
				}
                // learn_btn 5-2 - adding device ID
                else if( test_count==2 )
				{
					learning_mode = KEY_ADD_ID;                        
				}
                // learn_btn 5-3 - deleting device ID
                else if( test_count==3 )
				{
					learning_mode = KEY_DEL_ID;                   
				}
                // learn_btn 5-4 - sending test alarm     
                else if( test_count==4 )
				{
					add_event(TEST_PIN_T,0);
				 //   send_trigger_to_RF(0);
					learning_mode = KEY_NONE;
				}
                else if( test_count==5 )
				{
					learning_mode = KEY_NONE;
				}                         
				test_count = 0;
				test_time_detect = 0;
			}
	    }       
	}
}




uint8_t modem_module_start()
{
    return 1;
}	



