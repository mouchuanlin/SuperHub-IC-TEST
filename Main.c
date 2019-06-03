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

//
// MAIN
//
int main(int argc, char** argv) 
{
    uint8_t cnt,rsp,temp;
    uint8_t module_start_cnt;
    uint8_t sms_time;
	static uint8_t; // WDT_count;
    
    // System Init.
    init_system();
    
    // TODO: What's this for???
    init_stack_buffer();
    
    // Global variable init.
    module_start_cnt = 0;
    init_global_variables();

    // Buzzer for 10 times when power up - PORTx Output Latch bit value
    buzzer_on(10);
    
    //Enable HIGH Priority interrupt 
    GIE = 1;       

//========================Module start(S)========================
module_start:
    SWDTEN = 1;
    RF_wait_count = 0;
    LED_flash_type = LED_REGISTER;
    OSCCON = HIGH_FREQ_OSCCON;	// 4MHz
    T0CON = HIGH_FREQ_T0CON;             //1*4000 = 50,000us
    TMR0IF = 0;
    TMR0IE = 1;
    TMR0ON = 1;
    HL_freq = 1;
    
    Enable_Uart_IO();
    Uart_initial(); 
    
    // Modem init.
    module_start_cnt++;
    powerup_modem();
    
    first_run();   
    load_default();
    
    //----------- wait 10sec ----------
    // TODO: What's this wait 10 seconds for???
    delayseconds(10);     

    //---- wait AT command respond-----
    if (!wait_AT_cmd_response())        
		goto module_start;
        
    //-------- wait SIM ready---------
    if (!check_SIM_state())		
        goto module_start;
 
    //-------- wait register network---------
    if (!check_network_registration())
        goto module_start;
    
    //--------- Alarm or Report ---------    
alarm_start:    
	if (!alarm_or_report())
		goto module_start;
	
    //--------- Wait SMS Setting ---------
	if (!wait_SMS_setting())
		goto alarm_start;
	
//========================Module start(E)========================          
    LED_RX_IN = 1;
	
	// TODO: why assign to 0 again ???
    module_start_cnt = 0;   
	
    LED_flash_type = LED_OFF;
    #ifdef MODULE_OFF_TYPE
        MD_POWER = POWER_OFF;
    #else
        MD_RESET = 1;
        delayseconds(1);
        MD_RESET = 0;
    #endif
    Uart_disable();
   // OSCCON = LOW_FREQ_OSCCON;
   // HL_freq = 0;
    //T0CON = LOW_FREQ_T0CON;             //1*4000 = 50,000us
    //TMR0L = ((65535-LOW_FREQ_TMR0)%256);
    //TMR0H = ((65535-LOW_FREQ_TMR0)/256);
    TMR0IF = 0;
    TMR0IE = 0;//
    TMR0ON = 0;
    PEIE = 1;
    GIE = 1;      
    SWDTEN = 0;
    
	while (TRUE)
	{
        //LED_flash_type = LED_OFF;;
		process_running_system();
        CLRWDT();
		
		// Events in queue
		process_event_queue();
		
        // In SMS setup state???
		if (!process_SMS_setup_state())
			goto module_start;
		
        // Check ADC
		process_ADC();

        // RF interrupt
		process_RF_interrupt();
		
        // Check supervisory state
		process_supervisory();
        
		// Restart 
		if (!process_restart())
			goto module_start;
    }
	  
    return (EXIT_SUCCESS);   
}

void init_system()
{
    // REGISTER 2-1: OSCCON: OSCILLATOR CONTROL REGISTER
    OSCCON = HIGH_FREQ_OSCCON;  // 8MHz
    //MCLRE = 0;
    HL_freq = 1;
    SWDTEN = 0;     

    IO_init();
    Uart_initial();      
    ADC_init();
    
    T0CON = HIGH_FREQ_T0CON;             //1*4000 = 50,000us
    TMR0L = ((65535-HIGH_FREQ_TMR0)%256);
    TMR0H = ((65535-HIGH_FREQ_TMR0)/256);
    TMR0IF = 0;
    TMR0IE = 1;
    
    INTEDG0 = 1;
    INTEDG1 = 1;
    INTEDG2 = 1;
    INT1IP = 1;     //High Priority
    INT2IP = 1;     //High Priority
    IOCB = 0x10;    //RB.4 IOC enable
    
    INTEDG0 = 0;    //Int0 falling edge
    INTEDG1 = 1;    //Int1 rising edge
    INT0IE = 1;     //Enable Int0 interrupt
    INT1IE = 1;     //Enable Int1 interrupt
    //INT2IE = 1;     //Enable Int2 interrupt
    RBIE = 1;       //Enable IOC interrupt

    RC1IE = 1;      //Enable UART0_RX interrupt
    PEIE = 1;       //Enable LOW&HIGH Priority interrupt        
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
    uint8_t cnt;

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
        Uart_initial_115200();
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
            Uart_initial_115200();
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

uint8_t alarm_or_report()
{
	uint8_t cnt,rsp,temp;
	
    if( LED_flash_type==LED_STANDBY )
    {
        LED_flash_type = LED_INTERNET;
        rsp = check_emc_stack();
        if( rsp=='U' )
        {
            #ifdef MODULE_OFF_TYPE
                MD_POWER = POWER_OFF;
             #else
                MD_RESET = 1;
            #endif
			// TODO: what's this for???
			delayseconds(30);
            #ifndef MODULE_OFF_TYPE
                MD_RESET = 0;
            #endif
            //goto module_start;
			return FALSE;
        }
        
        if( OTA_flag==1 )
        {
            rsp = Check_OTA();
            if( rsp=='E' )
                OTA_flag = 2;
            else OTA_flag = 0;
        }
    }
    check_led_type();	
	
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
                    Uart_initial_115200();
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

static uint8_t WDT_count = 0;
void process_running_system()
{       

	
	if( RF_wait_count==0)
	{
		SWDTEN = 1;
		SLEEP();   
		NOP();
		NOP();
		//SWDTEN = 0;
		if( ++WDT_count>=3 )
		{            
			WDT_count = 0;
			//LED_G = ~LED_G;
			if( first_test!=0 )
			{
				first_test--;
				if( first_test!=0 )
					first_test--;
			}

			if( VER_SELECT==1&&TEST_PIN==1 )
			{
				if( ++test_9sec_count==45 )   //100ms*90
					add_event(TEST_PIN_T,0);
			}else test_9sec_count = 0;
	
			if( ++adc_count>=(18000*0.9888) )       //[3600*10]*100ms=3600sec=1hr            //0.979  -1.56
			{            
				chk_supervisory++;    //----add supervisory
				adc_count = 0;   
				if( ++Respond_T_Hour>=24 )       //24
				{
					Respond_T_Hour = 0;
					Respond_T_Day++;
					if( Respond_T_Day>= respond_day )         
					{
						Respond_T_Day = 0;
						add_event(TEST_CYCLE_S,0);             
						rsp_SUP_LBT();
						OTA_flag = 1;
					}
					if( BT_S_respond!=0 )
						BT_S_respond--;
					if( BT_L_respond!=0 )
						BT_L_respond--;
					if( EOL_respond!=0 )
						EOL_respond--;
					if( OTA_flag==2 )
						OTA_flag = 1;
				}            
			}else if( (adc_count%300)==0 )        //every 10 mins check low battery
			{
				ADC_time = 1;
			}
		   
			if( retry_count!=0 )
			{
				retry_count--;
				if( retry_count!=0 )               
					retry_count--;
			}
	
			if( led_count!=0 )
			{
				if( --led_count==0 )
				{
					LED_RX_IN = 1;
					LED_RX_OUT = 1;
				}else{
					if( --led_count==0 )
					{
						LED_RX_IN = 1;
						LED_RX_OUT = 1;
					}                        
				}
			}
		
			if( tamper_status!=0&&TAMPER_PIN==1  )
			{
				if( ++tamper_status>2 )
				{
					if( first_tamper==0 )                    
						add_event(TAMPER_CLOSE_T,1);
					first_tamper = 0;
					tamper_status = 0;
					SPK = 1;
				}
			}   
	
			if( learn_delay!=0 )
			{
				learn_delay--;
				if( learn_delay!=0 )            
					learn_delay--;                    
			}
		
			if( learning_mode == KEY_NONE )
			{                
				if( retry_count==0 )
				{
					LED_B = 1;
					LED_G = 1;
				}else       //new add V104
				{
					LED_count++;
					if( LED_count < 2 )                
					{
						LED_G = 0;     
						LED_B = 0;
					}
					else
					{
						LED_G = 1;
						LED_B = 1;
					}                
					if( LED_count>=25 )
						LED_count = 0;
				}
			
			}else{  
				if( ++exit_learn>=300 )     //100*100ms=10Sec
					learning_mode = KEY_NONE;
				if( learning_mode==KEY_IN_LEARN)
				{
					LED_B = 0;
					LED_G = 0;
				}else if( learning_mode==KEY_ADD_ID )
				{
					LED_count++;
					if( LED_count < 2 )                
						LED_B = 0;     
					else LED_B = 1;
					LED_G = 0;
					if( LED_count>=10 )
						LED_count = 0;
				}
				else if( learning_mode==KEY_DEL_ID )
				{
					LED_count++;
					if( LED_count < 2 )                
						LED_G = 0;     
					else LED_G = 1;
					LED_B = 0;
					if( LED_count>=10 )
						LED_count = 0;          
				}
			}
			if( ver_select == SMOKER )
			{
				if( alarm_count!=0 )
				{
					if( ALARM_PIN==1 )
					{
						if( ++alarm_count>=5 )
						{
							add_event(SMOKE_ALARM_T,1); 
						// send_trigger_to_RF(1);
							alarm_count = 0;
						}
					}else alarm_count = 0;
				}
				if( ERROR_PIN==1 )
				{
					if( ++err_count>=5 )   //100ms*10 = 1 sec
					{
						if( err_count==5 )
						{
						//   if( alarm_out(TAMPER_OPEN_T)==0 )
						//      Tamper_open_f = 1;
							add_event(TAMPER_OPEN_T,1);
						}
						err_count = 5;
					}
			   }else
				{
					if( err_count>=5 )
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
					if( standby_count>= 2000)
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
						if( ++error_time_detect>=150 )  //100ms*300=30sec
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
						if( ++error_time_detect>=150 )  //100ms*300=30sec
						{ 
							error_time_detect = 0;   
							error_status_count = 0;                    
						}
					}
				} 
			}             
			if( test_count!=0 )
			{
				test_time_detect++;
				if( learning_mode == KEY_NONE )
				{
					if( test_count==5&&test_time_detect>5 ) //1sec
					{
						learning_mode = KEY_IN_LEARN;
						exit_learn = 0;
						test_count = 0;
						test_time_detect = 0;
					}else if( test_time_detect>=10 )  //100ms*20=2sec
					{
						test_count = 0;
						test_time_detect = 0;
					}
				}else{                                                        
					if( ++test_time_detect>=10 )  //100ms*20=2sec
					{               
						if( test_count==1 )
						{
							Test_click = 1;
							add_event(GO_SMS_T,0);
							learning_mode = KEY_NONE;
						}else if( test_count==2 )
						{
							learning_mode = KEY_ADD_ID;                        
						}else if( test_count==3 )
						{
							learning_mode = KEY_DEL_ID;                   
						}else if( test_count==4 )
						{
							add_event(TEST_PIN_T,0);
						 //   send_trigger_to_RF(0);
							learning_mode = KEY_NONE;
						}else if( test_count==5 )
						{
							learning_mode = KEY_NONE;
						}                         
						test_count = 0;
						test_time_detect = 0;
					}
				}           
			}
			if( RF_wait_count!=0 )
			{
				LED_G = 0;
				if( --RF_wait_count==0 )
				{           
				 //   OSCCON = LOW_FREQ_OSCCON;
				 //   HL_freq = 0;
					TMR0ON = 0;
					//T0CON = LOW_FREQ_T0CON;             //1*4000 = 50,000us
				}
			}
		}
	}
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
		Uart_initial_BD2();
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

void interrupt tc_int( void )
{ 
    uint8_t id[6];
    uint8_t zone,cnt;
    uint8_t temp;

	// UART1 ISR - modem
    UART1_ISR();
	
	// UART2 ISR - RF receiver
    UART2_ISR();
	
	smoker_ISR();
	superhub_ISR();
	
	// TMR0 ISR
	TMR0_ISR();
}

// UART1 (to OTA/modem) ISR
void UART1_ISR()
{
    uint8_t temp;
        
    // RC1IE: EUSART1 Receive Interrupt Enable bit
    // RC1IF: EUSART1 Receive Interrupt Flag bit
    if( RC1IE==1 && RC1IF==1 )
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
        do{       	
            temp = RC2REG;
            //LED_G = 1;
            LED_B = 0;
            rx2_buf[rx2_cnt] = temp;
            if( ++rx2_cnt>=20 )
                rx2_cnt = 19;
            //out_sbuf2(rx2_cnt+0x30);
            if( temp=='\n' )
            {         
                if( rx2_cnt>=7 )
                {
                    cnt = 0;
                    if( rx2_buf[rx2_cnt-7]=='$'&&rx2_buf[rx2_cnt-2]=='\r')                    
                    {
                        for(zone=rx2_cnt-7;zone<rx2_cnt;zone++ )
                        {
                            rx2_buf[cnt++] = rx2_buf[zone];
                        }
                        rx2_cnt = 7;
                    }
                }
            /*    out_sbuf2('a');/////
                    out_sbuf2('t');//////
                   out_sbuf2('-');//////
                   for( zone=0;zone<rx2_cnt;zone++ )
                   {
                       temp = rx2_buf[zone];
                       out_sbuf2((temp>>4)+0x30);
                        out_sbuf2((temp&0x0f)+0x30);
                   }
                    out_sbuf2(0x0d);//
                    out_sbuf2(0x0a);//*/
                if( rx2_buf[0]=='$'&&rx2_buf[5]=='\r'&&rx2_buf[6]=='\n' )     //rf data in  $+3byte serial+1byte status+<CR>+<LF>                    
                {                
               //    out_sbuf2('a');/////
               //     out_sbuf2('t');//////
               //    out_sbuf2('-');//////
                    led_count = 10;
                    LED_RX_IN = 0;
                    rx2_cnt = 1;
                    do{
                        temp = (rx2_buf[rx2_cnt]>>4)&0x0f;
                        if( temp>=10 )
                        {
                            temp += 0x41;
                            temp -= 10;
                        }else temp += 0x30;
                        id[(rx2_cnt-1)*2] = temp;
                    //    out_sbuf2(temp);////
                        temp = rx2_buf[rx2_cnt]&0x0f;
                        if( temp>=10 )
                        {
                            temp += 0x41;
                            temp -= 10;
                        }else temp += 0x30;
                        id[(rx2_cnt-1)*2+1] = temp;
                    //    out_sbuf2(temp);//////
                    }while(++rx2_cnt<4); 
                    temp = rx2_buf[4];
                    //out_sbuf2((temp>>4)+0x30);
                    //out_sbuf2((temp&0x0f)+0x30);
                    //out_sbuf2(0x0d);//
                    //out_sbuf2(0x0a);//
                    //out_sbuf2('-');
                    zone = check_ID(&id);       //respond zone number(3~30),error=0
                   
                    if( zone!=0 )
                    {
                        ID_LIST[zone-3][7] = 0;     //clear supervisory count
                        LED_RX_OUT = 0;
                    }
                    if( learning_mode==KEY_NONE )
                    {
                        if( zone==0 )
                        {
                        /*    out_sbuf2('$');
                            out_sbuf2('A');
                            out_sbuf2('N');
                            out_sbuf2(0x0d);
                            out_sbuf2(0x0a);*/
                        }else{                      //Supervisory,nc,nc,low BT,nc,test,tamper,trigger/alarm                                                                                           
                            if( (rx2_buf[4]&0x01)!=0 )   //alarm
                            {                                                                                                
                                if( id[0]=='8')
                                    add_event(SMOKE_ALARM_T,zone);
                                else if( id[0]=='6' ) 
                                    add_event(FLOOD_T,zone); 
                                else if( id[0]=='2' )
                                    add_event(CARBON_T,zone);
                                else if( id[0]=='C' )
                                    add_event(GLASS_T,zone);
                                else if( id[0]=='9' )
                                    add_event(MOTION_T,zone);
                                else if( id[0]=='3' )
                                    add_event(DOOR_T,zone);
                                else if( id[0]=='1' )
                                    add_event(PANIC_T,zone);
                                else if( id[0]=='B' )
                                    add_event(HVAC_T,zone);
                                else if( id[0]=='5' )
                                    add_event(APPLIANCE_T,zone);
                                else if( id[0]=='4' )
                                    add_event(RESERVE1_T,zone);
                                else if( id[0]=='7' )
                                    add_event(RESERVE2_T,zone);
                                else if( id[0]=='A' )
                                    add_event(RESERVE3_T,zone);
                                else if( id[0]=='D' )
                                    add_event(RESERVE4_T,zone);
                                else if( id[0]=='E' )
                                    add_event(RESERVE5_T,zone);
                                else if( id[0]=='0' )
                                    add_event(RESERVE6_T,zone);
                                else if( id[0]=='F' )
                                    add_event(RESERVE7_T,zone);
                            }
                            if( (rx2_buf[4]&0x04)!=0 )   //test
                            {
                                add_event(TEST_PIN_T,zone);                            
                            }    
                            if( ((rx2_buf[4]&0x02)!=0) )   //Tamper open
                            {
                                if( ID_LIST[zone-3][8]==0 )
                                {
                                    add_event(TAMPER_OPEN_T,zone);     
                                    ID_LIST[zone-3][8]=1; 
                                }
                            }else                           //Tamper close
                            {
                                if( ID_LIST[zone-3][8]==1 )
                                {
                                    add_event(TAMPER_CLOSE_T,zone);     
                                    ID_LIST[zone-3][8]=0; 
                                }
                            }
                            if( (rx2_buf[4]&0x10)!=0 )   //Low Battery
                            {
                                add_event(LOW_BATTERY_T,zone);                            
                            }     
                            if( (rx2_buf[4]&0x80)!=0 )   //Supervisory
                            {
                                add_event(SUPERVISORY_T,zone);                            
                            }     
                            /*out_sbuf2('$');
                            out_sbuf2('A');
                            out_sbuf2('S');
                            out_sbuf2(0x0d);
                            out_sbuf2(0x0a);*/
                        }
                    }else{
                        
                        if( learning_mode==KEY_ADD_ID&&zone==0 )
                            zone = add_ID(&id); 
                        else if( learning_mode==KEY_DEL_ID&&zone!=0 )
                            zone = del_ID(zone); 
                        
                        /*out_sbuf2('$');
                        out_sbuf2('A');
                        out_sbuf2('L');
                        out_sbuf2(0x0d);
                        out_sbuf2(0x0a);*/
                    }    
                    //send respond
                    for( rx2_cnt=0;rx2_cnt<7;rx2_cnt++)
                        out_sbuf2(rx2_buf[rx2_cnt]);
                    //------------
                }
                rx2_cnt = 0;
                CREN2 = 0;
                NOP();
                CREN2 = 1;
            }
        }while(RC2IF==1);
       // RC1IF = 0;
    }        	
}

void TMR0_ISR()
{
    // TMR0 Overflow Interrupt Flag bit
    if( TMR0IF==1 )
    {                
        TMR0IF = 0;        
        if( HL_freq==0 )    //31.25KHz
        {
            TMR0L = ((65536-LOW_FREQ_TMR0)%256);
            TMR0H = ((65536-LOW_FREQ_TMR0)/256);
        }else
        {
            TMR0L = ((65536-HIGH_FREQ_TMR0)%256);
            TMR0H = ((65536-HIGH_FREQ_TMR0)/256);
        }
       // SPK = ~SPK;
        if( first_test!=0 )
            first_test--;
    //    if( adc_count%2900==0 )       //for battery voltage test (every 5 mins) 電池電壓測試(每5分鐘)
    //        add_event(TEST_CYCLE_S,2);
        if( VER_SELECT==1&&TEST_PIN==1 )
        {
            if( ++test_9sec_count==90 )   //100ms*90
                add_event(TEST_PIN_T,0);
        }else test_9sec_count = 0;
        
        if( ++adc_count>=(36000*0.9888) )       //[3600*10]*100ms=3600sec=1hr            //0.979  -1.56
        {            
            chk_supervisory++;    //----add supervisory
            adc_count = 0;   
            if( ++Respond_T_Hour>=24 )       //24
            {
                Respond_T_Hour = 0;
                Respond_T_Day++;
                if( Respond_T_Day>= respond_day )         
                {
                    Respond_T_Day = 0;
                    add_event(TEST_CYCLE_S,0);             
                    rsp_SUP_LBT();
                    OTA_flag = 1;
                }
                if( BT_S_respond!=0 )
                    BT_S_respond--;
                if( BT_L_respond!=0 )
                    BT_L_respond--;
                if( EOL_respond!=0 )
                    EOL_respond--;
            }   
            if( OTA_flag==2 )
                OTA_flag = 1;
        }else if( (adc_count%600)==0 )        //every 10 mins check low battery
            ADC_time = 1;
               
        if( retry_count!=0 )
            retry_count--;
        
        if( led_count!=0 )
        {
            if( --led_count==0 )
            {
                LED_RX_IN = 1;
                LED_RX_OUT = 1;
            }
        }
        
        if( tamper_status!=0&&TAMPER_PIN==1  )
        {
            if( ++tamper_status>5 )
            {
                if( first_tamper==0 )                    
                    add_event(TAMPER_CLOSE_T,1);
                first_tamper = 0;
                tamper_status = 0;
                SPK = 1;
            }
        }
        
        if( learn_delay!=0 )
            learn_delay--;

		// Blinking LED
		handle_LED();

		// Smoker specific 
		handle_smoker();
		
		handle_learn_btn_pressed();
		
        if( RF_wait_count!=0 )
        {
            LED_G = 0;
            if( --RF_wait_count==0 )
            {           
               // OSCCON = LOW_FREQ_OSCCON;
                HL_freq = 0;
                T0CON = LOW_FREQ_T0CON;             //1*4000 = 50,000us
                TMR0IE = 0;//
                TMR0ON = 0;
            }
        }
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

void handle_LED()
{    
	if( learning_mode == KEY_NONE )
	{
		if( LED_flash_type!=LED_OFF )
		{
			if( LED_flash_type==LED_REGISTER )
			{
				LED_count++;
				if( LED_count < 5 )                
					LED_G = 0;     
				else LED_G = 1;
				if( power_status==0 )
					LED_B = 1;
				else LED_B = 0;
				if( LED_count>=20 )
					LED_count = 0;
			}else if( LED_flash_type==LED_STANDBY )
			{
				LED_count++;
				if( LED_count < 5 )                
					LED_B = 0;     
				else LED_B = 1;
				LED_G = 1;
				if( LED_count>=20 )
					LED_count = 0;
			}else if( LED_flash_type==LED_INTERNET )
			{
				LED_B = ~LED_B;
				LED_G = 1;
			}else if( LED_flash_type==LED_NO_SET )
			{
				LED_count++;
				if( LED_count<5 )
				{
					LED_B = 0;     
					LED_G = 1;
				}else if( LED_count<=9 )
				{
					LED_B = 1;     
					LED_G = 0;               
				}else LED_count = 0;
			}else if( LED_flash_type==LED_NET_ERR )
			{
				LED_count++;         
				if( LED_count==0 )
				{
					LED_B = 0;     
					LED_G = 1;
				}else if( LED_count==1 )
				{
					LED_B = 1;     
					LED_G = 1;               
				}else if( LED_count==2 )
				{
					LED_B = 0;     
					LED_G = 1;               
				}else if( LED_count==3 )
				{
					LED_B = 1;     
					LED_G = 1;               
				}else if( LED_count==4 )
				{
					LED_B = 1;     
					LED_G = 0;               
				}else if( LED_count==5 )
				{
					LED_B = 1;     
					LED_G = 1;               
				}else if( LED_count==6 )
				{
					LED_B = 1;     
					LED_G = 0;               
				}else if( LED_count==7 )
				{
					LED_B = 1;     
					LED_G = 1;               
				}else LED_count = 0;            
			}
		}else{
			if( retry_count==0 )
			{
				LED_B = 1;
				LED_G = 1;
			}else       //new add V104
			{
				LED_count++;
				if( LED_count < 3 )                
				{
					LED_G = 0;     
					LED_B = 0;
				}
				else
				{
					LED_G = 1;
					LED_B = 1;
				}                
				if( LED_count>=50 )
					LED_count = 0;
			}
		}
	}else{
		if( ++exit_learn>=600 )     //100*100ms=10Sec
			learning_mode = KEY_NONE;
		if( learning_mode==KEY_IN_LEARN)
		{
			LED_B = 0;
			LED_G = 0;
		}else if( learning_mode==KEY_ADD_ID )
		{
			LED_count++;
			if( LED_count < 5 )                
				LED_B = 0;     
			else LED_B = 1;
			LED_G = 0;
			if( LED_count>=20 )
				LED_count = 0;
		}
		else if( learning_mode==KEY_DEL_ID )
		{
			LED_count++;
			if( LED_count < 5 )                
				LED_G = 0;     
			else LED_G = 1;
			LED_B = 0;
			if( LED_count>=20 )
				LED_count = 0;          
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


