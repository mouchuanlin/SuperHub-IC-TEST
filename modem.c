//
// modem.c
//

#include "modem.h"
#include "io.h"
#include "led.h"
#include "uart.h"
#include "eeprom.h"
#include "OTA.h"
#include "timer.h"
#include "telit.h"
#include "emc_library.h"
#include "sms.h"

bool modem_config_ok()
{			
    SWDTEN = 0;
    
//    soutdata((uint8_t *) "AT+IPR=115200\r\n$");
//    if (wait_ok_respond(40) != 'K')
//		return false;
	
//	if (test_at() != 'K')
//		return false;
    
//    test_at();
//    check_ip_setting();
	

    if (!wait_AT_cmd_response())        
		return false;
        
    //-------- wait SIM ready---------
    if (!check_SIM_state())		
		return false;
 
    //-------- wait register network---------
    if (!check_network_registration())
		return false;
    
    //--------- Alarm or Report ---------    
alarm_start:    
	if (!start_send_alarm())
		return false;

    // In OTA_BOOT mode, there is no need to do anything else.
    if (myState == OTA_BOOT)
        return true;
	
    //--------- Wait SMS Setting ---------
	if (!start_sms())
    {
        goto alarm_start;
    }
    
    return true;
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
        UART1_init(115200);
        //UART1_init(19200);
        // mlin - setup modem baud rate
        ///// STEP 2. - +IPR to set modem baudrate to 19200 bps
        soutdata((uint8_t *) "AT+IPR=115200\r\n$");
        //soutdata((uint8_t *) "AT+IPR=19200\r\n$");
        wait_ok_respond(40);
        //UART_init();
        delay5ms(20);
        // AT
        cnt = check_module_run();
//        cnt = 'K';
        if( cnt!='K' )
        {
            //delayseconds(3);
            ///// STEP 3. - We probably do need this since ???
            //Uart_initial_115200();
            //UART1_init(19200);
            UART1_init(115200);
            // ATI4
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
        MD_POWER = POWER_OFF;

        cnt = 30;
        do{
            delayseconds(1);
        }while(--cnt!=0);

        power_status = MD_POWER_LOSS;
        //goto module_start;
		return false;
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
 
    TL_module_first_run();
    //--------------------------------
    delay5ms(100);
    soutdata((uint8_t *) "AT+VER=$");
    
    // TODO: This should no functional at all???
    //      THIS IS FOR DEBUG PURPOSE.
    if( VER_SELECT==1 )
        soutdata((uint8_t *) "Smoke HUB,$");
    else 
        soutdata((uint8_t *)"Super HUB,$");
	
    if( Module_type==EMS31 )    
        soutdata((uint8_t *) "EMS31\r\n$");
    else if( Module_type==PLS8 )    
        soutdata((uint8_t *) "PLS8\r\n$");
    else if( Module_type==LE910 )    
        soutdata((uint8_t *) "LE910\r\n$");    
    else //if( Module_type==LE866 )    
        soutdata((uint8_t *) "LE866\r\n$");    
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
            MD_POWER = POWER_OFF;

			// TODO: Why 30 secs here???
			delayseconds(30);
			
            power_status = MD_POWER_LOSS;
			
            //goto module_start;
			return false;
        } 
        CLRWDT();
    }while( rsp!='K'&& --cnt!=0 );
    if( cnt==0 )    
    {
        MD_POWER = POWER_OFF;
		
		// TODO: Why 30 secs here???
		delayseconds(30);
		
        //goto module_start;
		return false;
    }
	
	return true;
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
        MD_POWER = POWER_OFF;
            
		// TODO: Why 30 secs here???
		delayseconds(30);

        //goto module_start;
		return false;
    }
    power_status = 0;
    delay5ms(100);
    // mlin - AT+CSQ doesn't seems get response???
    rsp = check_csq();
    check_ip_setting();
    
    // TODO: Send TEST AT commands. 
//    send_test_AT_commands();
    
    //test_at();
    
    // mlin - why "AT\\Q0\r\n$"
    //soutdata((uint8_t *) "AT\\Q0\r\n$");
    
//    soutdata(AT+CMGS=\"5665776987\"\r\n$");
    
    //OTA_flag = 1; //----------------------------------
	
	return true;    
}

uint8_t send_at_command(uint8_t str[])
{
    CREN1 = 0;
    RC1IE = 0;
    CREN1 = 1;
    reload_timer3_50ms();
    //reload_timer3_100ms();
    //reload_timer3_2s();
    //    rx1_flag = false;
    //    rx1_cnt = 0;  
    
    soutdata((uint8_t *)str);  
//    while (!rx1_flag)
//        ;
    
     if (wait_ok_respond(40) != 'K')
		return false;
    
    return true;
//    __delay_ms(100);
}

void test_at(void)
{
    send_at_command((uint8_t *)"AT&F1\r\n$");   
    send_at_command((uint8_t *)"AT+CMGF=1\r\n$");    
    send_at_command((uint8_t *)"AT+CMEE=2\r\n$");    
    //send_at_command((uint8_t *)"AT+CGDCONT=3,\"IP\",\"11583.mcs\"\r\n$");    
    send_at_command((uint8_t *)"AT+CGDCONT?\r\n$");    
    send_at_command((uint8_t *)"AT+CGMR\r\n$");    
    send_at_command((uint8_t *)"AT+CGSN\r\n$");    
    send_at_command((uint8_t *)"AT#SCFG=1,3,300,90,200,50\r\n$");    
    send_at_command((uint8_t *)"AT+CGATT=1\r\n$");    
    send_at_command((uint8_t *)"AT#SCFGEXT?\r\n$");    
    send_at_command((uint8_t *)"AT+CREG=1\r\n$");    
    send_at_command((uint8_t *)"AT+CREG?\r\n$");    
    send_at_command((uint8_t *)"AT#SGACT=3,1\r\n$");    
    send_at_command((uint8_t *)"AT+CMEE=2\r\n$");    
    send_at_command((uint8_t *)"AT#CCID\r\n$");    
    send_at_command((uint8_t *)"AT+CSQ\r\n$");    
    send_at_command((uint8_t *)"AT#MONI\r\n$");    
    send_at_command((uint8_t *)"AT+COPS?\r\n$");    
    send_at_command((uint8_t *)"AT+CGSN\r\n$");    
    send_at_command((uint8_t *)"AT+CNUM\r\n$");       
    send_at_command((uint8_t *)"AT+CGMR\r\n$");    
    send_at_command((uint8_t *)"AT+CIMI\r\n$");    
    send_at_command((uint8_t *)"AT#BND?\r\n$");    
    send_at_command((uint8_t *)"AT#CCID\r\n$");    
    //send_at_command((uint8_t *)"AT#SD=1,0,2020,\"12.12.201.82\",0,0,1\r\n$");    
    send_at_command((uint8_t *)"AT#SS\r\n$");    
    send_at_command((uint8_t *)"AT#SH=1\r\n$");    
    send_at_command((uint8_t *)"AT#SH=2\r\n$");    
}

void send_test_AT_commands(void)
{
    
    // TODO:
    soutdata((uint8_t *) "AT+CMEE=2\r\n$");    
    delay5ms(20);

    soutdata((uint8_t *) "AT#CCID\r\n$");    
    delay5ms(20);

    soutdata((uint8_t *) "AT+CSQ\r\n$");    
    delay5ms(20);
    
    soutdata((uint8_t *) "AT#MONI\r\n$");    
    delay5ms(20);  
    
    soutdata((uint8_t *) "AT+COPS?\r\n$");    
    delay5ms(20); 
    
    soutdata((uint8_t *) "AT+CREG?\r\n$");    
    delay5ms(20);     
    
    soutdata((uint8_t *) "AT+CGREG?\r\n$");    
    delay5ms(20); 
    
    soutdata((uint8_t *) "AT+CEREG?\r\n$");    
    delay5ms(20);
	
	// Core SIM
	//soutdata("AT+CGDCONT=1,\"IP\",\"11583.mcs\",\"0.0.0.0\",0,0\r\n$");
	//soutdata("AT+CGDCONT=3,\"IP\",\"11583.mcs\",\"0.0.0.0\",0,0\r\n$")	
	// delay5ms(20);
    
	// Telit SIM
//	soutdata("AT+CGDCONT=1,\"IP\",\"11583.mcs\",\"0.0.0.0\",0,0\r\n$");
//	delay5ms(100);
//	soutdata("AT+CGDCONT=3,\"IP\",\"11583.mcs\",\"0.0.0.0\",0,0\r\n$");
//	delay5ms(20);

    soutdata((uint8_t *) "AT+CGDCONT?\r\n$");    
    delay5ms(20);   
    
    soutdata((uint8_t *) "AT#SGACT=3,1\r\n$");    
    delay5ms(20);
    
    soutdata((uint8_t *) "AT+CGSN\r\n$");    
    delay5ms(20);
    
//    soutdata((uint8_t *) "AT#SNUM=1,9566405896\r\n$");    
//    delay5ms(20);
    
    soutdata((uint8_t *) "AT+CNUM\r\n$");    
    delay5ms(20); 
    
    soutdata((uint8_t *) "AT+CNUM=?\r\n$");    
    delay5ms(20);          
    
    soutdata((uint8_t *) "AT+CPIN?\r\n$");    
    delay5ms(20);  
   
    soutdata((uint8_t *) "AT+CGMR\r\n$");    
    delay5ms(20);
    
    soutdata((uint8_t *) "AT+CIMI\r\n$");    
    delay5ms(20);
    
    soutdata((uint8_t *) "ATI1\r\n$");    
    delay5ms(20);
    
    soutdata((uint8_t *) "ATI4\r\n$");    
    delay5ms(20);    
    
    soutdata((uint8_t *) "AT+CSQ\r\n$");    
    delay5ms(20);    
}


uint8_t start_send_alarm()
{
	uint8_t cnt,rsp,temp;
    md_resp_t ota_status = OTA_UNKNOW;
	
    if (ready_for_sending)
    {
        //LED_flash_type = LED_INTERNET;
        //update_led_state(SENDING);
        rsp = check_emc_stack();
        if( rsp=='U' )
        {
            MD_POWER = POWER_OFF;

			// TODO: what's this for???
			delayseconds(30);

            //goto module_start;
			return false;
        }
        
        // TODO: Comment out OTA function for now since we don't have OTA working at all.
        if( OTA_flag==1 )
        {
            rsp = check_OTA();
            
            if( rsp=='F' )
            {
                // Setup for next OTA in 24 hours
                respond_day = 1;
            }
            //if( rsp=='K' )
            else
            {
                OTA_flag = 2;
                
                // 1000 = 50 sec
                while (ota_status == OTA_UNKNOW)
                {
                    ota_status = wait_ota_status(1000);
                }
                
                switch (ota_status)
                {
                    case OTA_BOOT_SEL:
                        soutdata((uint8_t *) "set_boot_sel_output\r\n$");    
                        // d. PIC18 strobes BOOT_SEL = 0 for 500us, then BOOT_SEL = 1;
                        // Tell OTA PIC18 is ready for FW update.
                        set_boot_sel_output();
                        BOOT_SEL_O = 0;
                        __delay_us(500);
                        BOOT_SEL_O = 1;

                        myState = OTA_BOOT;
                        respond_day = page0_eeprom.map.TEST_FREQ;
                        
                        while (1)
                            ;
                        
                        break;
                        
                    case MD_ERROR:
                    case MD_NO_CARRIER:
                    case OTA_UNKNOW:
                    default:
                        // Setup for next OTA in 24 hours
                        respond_day = 1;
                        break;
                }

            }
        }
    }
    check_ip_setting();	
	
	return true;
}

uint8_t start_sms()
{
	uint8_t cnt,rsp,temp;
    uint8_t sms_time;
		
    // This bit indicating if we are in button 5-1 or not.
    if( listen_sms_state==1 )    
    {
        sms_time = page0_eeprom.map.SMS_WAIT_TIME;
        set_sms_init();    
        do{
            cnt = 12;
            do{
                rsp = 16;
                do{
                    delayseconds(1);
                    if(!is_event_que_empty())     
                        deque_event(); 
                    
                    // Need bail out call start_send_alarm() to send data
                    if( stack_buffer[0].map.state != 0 && retry_count==0 && IP_type==1 )   
                    {                   
                       //goto alarm_start;
					   return false;
                    }
                }while(--rsp!=0);
                
              //  LED = 0;
                check_sms();
                
                // Check EEPROM to see if APN & IP1 have been set.
                if (check_apn_status())
                    update_led_state(APN_IP_ACCT_NOT_SET);                
                else                
                    update_led_state(STANDBY);
                
                delay5ms(100);
                if( check_csq()==0 )
                {
                    ///// STEP 4. - Comment Uart_initial_115200() out. Call Uart_initial() to set to 19200.)
                    //Uart_initial_115200();
                    //UART1_init(19200);
                    UART1_init(115200); 
                    //soutdata("AT+IPR=19200\r\n$");
                    //wait_ok_respond(40);
                    //UART_init();
                    set_sms_init();    
                }
              //  LED = 1;
            }while(--cnt!=0);      
            CLRWDT();
        }while(--sms_time!=0);
        listen_sms_state = 0;
    }
    
    if(!is_event_que_empty())    
        deque_event();
    
    // Need bail out call start_send_alarm() to send data
    if( stack_buffer[0].map.state !=0 && retry_count==0 && IP_type==1 )//LED_flash_type==LED_STANDBY )                     
        //goto alarm_start;
		return false;
		    
	return true;
}


bool check_apn_status()
{
    if ((read_ee(EE_PAGE0, APN_ADDR) == '#') && (read_ee(EE_PAGE0, IP1_ADDR) == '#'))
        return true;
    else
        return false; 
}

void powerup_modem(void)
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

void start_modem(void)
{
//    CLRWDT();
//    MD_POWER = POWER_ON;
//    //__delay_ms(25000);
//    delayseconds(25);
        
    update_led_state(POWERON);
       
    enable_UART();    
    powerup_modem();
    
    // AT&F Reset AT Command Settings to Factory Default Values
    //reset_at_command_default();
            
    // TODO: FOR DEBUGGING ONLY
    //send_test_AT_commands();
//    test_at();
   
    while (!modem_config_ok())
        restart_modem();
    
    poweroff_modem();
    //MD_POWER = POWER_OFF;
    
    myState = SLEEP;
    update_led_state(OFF);
    //md_started = true;
}

void reset_at_command_default(void)
{
    soutdata((uint8_t *) "AT&F1\r\n$");    
    delay5ms(20);
}

void restart_modem(void)
{
    CLRWDT();
    //MD_POWER = POWER_OFF;
    poweroff_modem();

    delayseconds(5);
   
    powerup_modem();
}

void poweroff_modem(void)
{
    MD_POWER = POWER_OFF;
}

void process_event_queue(void)
{
	// Event queue is not empty
    if(!is_event_que_empty())      
	{
		deque_event(); 
		retry_count = 0;        // new add on V104
	}
}

//uint8_t process_SMS_setup_state()
//{
//	if ( listen_sms_state==1 )
//	{                     
//		MD_POWER = POWER_OFF;
//
//		//goto module_start;
//		return false;
//	}	
//	
//	return true;
//}

void out_sbuf(uint8_t tmp)
{
    // TRMT: Transmit Shift Register Status bit
    while( TRMT1==0 )
        ;
    CLRWDT();
    TX1REG = tmp;
}

void soutdata(const uint8_t dat[])
{
    uint8_t temp,cnt;
    cnt = 0;
    do{
        temp = dat[cnt++];
        if( temp!='$' )
            out_sbuf(temp);
    } while (temp !='$' && temp != '\0');
    CLRWDT();
}

void soutdata_1(const uint8_t dat[], uint8_t len)
{
    uint8_t temp,cnt;
    
    cnt = 0;
    while (len--)
    {
        
        temp = dat[cnt++];
        out_sbuf(temp);
    };
    
    CLRWDT();
}

void out_sbuf2(uint8_t tmp)
{
    while( TRMT2==0 )
        ;
    CLRWDT();
    TX2REG = tmp;
}

//---------------------------------------------------
uint8_t wait_ok_respond(uint16_t count)
{
  	uint8_t temp;
    uint8_t buffer[50]={0}, buffer_p = 0;
    CREN1 = 0;
    RC1IE = 0;
    CREN1 = 1;
    CLRWDT();
  	do{
        T3CON = 0x71;
        TMR3H = 0x40;   //50ms
        TMR3L = 0;
        TMR3IF = 0;
     	do{
            // RC1IF: EUSART1 Receive Interrupt Flag bit
        	if( RC1IF==1)
        	{	   
                // RCREGx
		 	 	temp=RC1REG;
                buffer[buffer_p] = temp;
                if( ++buffer_p >= 50 )
                    buffer_p = 49;
                if (temp == LF)     // 0x0A, \n
                {
                    //if (strncmp((const char *)buffer, (const char *)"OK", 2) == 0)
                    if( buffer[0]=='O'&&buffer[1]=='K' )
                        temp = 'K';
                    else if (strncmp((const char *)buffer, (const char *)"ERROR", 5) == 0)
                        temp = 'E';
                    
                    if(temp=='K'||temp=='E')
                    {
                        RC1IE = 1;
                        CREN1 = 0;
                        //delay5ms(60);
                        return(temp);		    		
                    }
                    buffer_p = 0;
                }
        	}
            //check_receive_overrun();
            
     	}while(TMR3IF==0);  // TMR3IF: TMR3 Overflow Interrupt Flag bit
        CLRWDT();
        TMR3IF = 0;
        //LED = ~LED;
  	}while(--count!=0);
    
  	TMR3ON = 0;
    RC1IE = 1;
  	return('N');
}

uint8_t check_module_run(void)
{
    uint8_t at[]="AT\r\n$";
  	uint8_t temp,cnt;
	uint8_t buffer[32];
	uint8_t buf_cnt=0;
 
    CREN1 = 0;
	soutdata(at);
    T3CON = 0x71;
    TMR3H = 0x40;   //50ms
    TMR3L = 0;
    TMR3IF = 0;    
    cnt=20;         //50ms*20=1sec
    RCIE = 0;
    CREN1 = 1;
    CLRWDT();
	do{ 
        T3CON = 0x71;
        TMR3H = 0x40;   //50ms
        TMR3L = 0;
        do{
			if( RC1IF==1 )
			{                  
				temp = RC1REG;
				buffer[buf_cnt] = temp;
				if( temp=='K' )
                    return('K');
			}
            
            //check_receive_overrun();
		}while(TMR3IF==0 );
        CLRWDT();
		TMR3IF = 0;
	}while(--cnt!=0);
	TMR3ON = 0;
    RC1IE = 1;
	return('E');
}

uint8_t check_module_version(uint8_t type)
{
    const uint8_t at4[]="ATI4\r\n$";
    const uint8_t at3[]="ATI1\r\n$";
  	uint8_t temp,cnt;
	uint8_t buffer[32];
	uint8_t buf_cnt=0;
 
    CREN1 = 0;
    if( type%2==0 )
        soutdata(at4);
    else 
        soutdata(at3);
    
    //soutdata(at4);

    T3CON = 0x71;
    TMR3H = 0x40;   //50ms
    TMR3L = 0;
    TMR3IF = 0;    
    cnt = 20;         //50ms*20=1sec
    RCIE = 0;
    CREN1 = 1;
    buf_cnt = 0;
    CLRWDT();
	do{ 
        T3CON = 0x71;
        TMR3H = 0x40;   //50ms
        TMR3L = 0;
        do{
			if( RC1IF==1 )
			{                  
            //    LED = ~LED;				
				temp = RC1REG;
				//RCIF = 0;
				buffer[buf_cnt] = temp;
				if( ++buf_cnt >=32 )
					buf_cnt = 31;
                
				if( temp == LF )
				{		  
                    if (strncmp((const char *)buffer, (const char *)"EMS31", 5) == 0)
					{
                        Module_type = EMS31;
                        return('K');
                    }
                    else if (strncmp((const char *)buffer, (const char *)"PLS8", 4) == 0)
					{
                        Module_type = PLS8;
                        return('K');
                    }
                    else if (strncmp((const char *)buffer, (const char *)"EHS5", 4) == 0)
					{
                        Module_type = EHS5;
                        return('K');
                    }
                    else if ( (strncmp((const char *)buffer, (const char *)"LE910", 5) == 0) || (strncmp((const char *)buffer, (const char *)"UE910", 5) == 0) )
					{
                        Module_type = LE910;
                        return('K');
                    }
                    else if ((strncmp((const char *)buffer, (const char *)"LE866", 5) == 0) || (strncmp((const char *)buffer, (const char *)"UE866", 5) == 0))
					{
                        Module_type = LE866;
                        return('K');
                    }
					buf_cnt = 0;
				}
			}
            
            check_receive_overrun();
		}while(TMR3IF==0 );
        CLRWDT();
		TMR3IF = 0;
	}while(--cnt!=0);
	TMR3ON = 0;
    RC1IE = 1;
	return('E');
}

uint8_t check_sim_card(void)
{
    const uint8_t cpin[]="AT+CPIN?\r\n$";
    const uint8_t cpin_s[]="+CPIN: READY$";
  	uint8_t temp,cnt;
  	uint8_t receive=0;
	uint8_t buffer[32];
	uint8_t buf_cnt=0;
 
    CREN1 = 0;
	soutdata(cpin);
    T3CON = 0x71;
    TMR3H = 0x40;   //50ms
    TMR3L = 0;
    TMR3IF = 0;    
    cnt=20;         //50ms*20=1sec
    RCIE = 0;
    CREN1 = 1;
    CLRWDT();
	do{ 
        T3CON = 0x71;
        TMR3H = 0x40;   //50ms
        TMR3L = 0;
        do{
			if( RC1IF==1 )
			{                  
            //    LED = ~LED;
				receive=1;
				temp = RC1REG;
				//RCIF = 0;
				buffer[buf_cnt] = temp;
				if( ++buf_cnt >=32 )
					buf_cnt = 31;
				if( temp==0x0a )
				{		
                  //  LED = ~LED;
					temp=0;
					do{
			    	  	if ( buffer[temp] == cpin_s[temp] )
			  			{
 			    			temp++;
							if ( cpin_s[temp]== '$' )
							{
                                temp = RC1REG;	
                                temp = RC1REG;	
                                temp = RC1REG;	
                                RC1IE = 1;
				  				TMR3ON = 0;
				  				return('K');
							}
			 	 		}else
			  				temp=0;
					}while(temp!=0);
					buf_cnt = 0;
				}
			}
		}while(TMR3IF==0 );
        CLRWDT();
        //LED = ~LED;
		TMR3IF = 0;
	}while(--cnt!=0);
	TMR3ON = 0;
    RC1IE = 1;
	if(receive==0)
    	return('N');
	return('E');
}

uint8_t check_register(uint8_t type)
{
    const uint8_t creg[]="AT+CREG?\r\n$";
  	const uint8_t creg_s[]="+CREG: $";
    const uint8_t cereg[]="AT+CEREG?\r\n$";
  	const uint8_t cereg_s[]="+CEREG: $";
  	uint8_t temp,cnt;
	uint8_t buffer[32];
	uint8_t buf_cnt=0;

    CREN1 = 0;
    if( type==0 )
        soutdata(creg);
    else 
        soutdata(cereg);
    
    // TXCON: TIMER1/3/5 CONTROL REGISTER
    T3CON = 0x71;   // 0111 0001
    TMR3H = 0x40;   //50ms
    TMR3L = 0;
    TMR3IF = 0;    
    cnt=20;         //50ms*20=1sec    
    RC1IE = 0;
    CREN1 = 1;
    CLRWDT();
	do{
        T3CON = 0x71;
        TMR3H = 0x40;   //50ms
        TMR3L = 0;
		do{
			if( RC1IF==1 )
			{           
				temp = RC1REG;				
				buffer[buf_cnt] = temp;
				if( ++buf_cnt >=32 )
					buf_cnt = 31;
				if( temp==0x0a )
				{		
                  //  LED = ~LED;
					temp=0;
					do{
                        if( type==0 )
                        {
                          	if ( buffer[temp] == creg_s[temp] )
                            {
                                temp++;
                                if ( creg_s[temp]== '$' )
                                {
                                    if( buffer[temp+2]=='1'|| buffer[temp+2]=='5' )
                                    {
                                        TMR3ON = 0;
                                        RC1IE = 1;
                                        return('K');	
                                    }
                                }                        
                            }else
			  				temp=0;
                        }else
                        {
                            if ( buffer[temp] == cereg_s[temp] )
                            {
                                temp++;
                                if ( cereg_s[temp]== '$' )
                                {
                                    if( buffer[temp+2]=='1'|| buffer[temp+2]=='5' )
                                    {
                                        TMR3ON = 0;
                                        RC1IE = 1;
                                        return('K');	
                                    }
                                }                        
                            }else
			  				temp=0;
                        }
					}while(temp!=0);
					buf_cnt = 0;
				}
			}
            
            check_receive_overrun();
		}while(TMR3IF==0 );
        CLRWDT();
		TMR3IF = 0;
	}while(--cnt!=0);
	TMR3ON = 0;
    RC1IE = 1;
	return('E');
}

//---------------------------------------------------
uint8_t check_csq(void)
{  	
  	const uint8_t csq[]="AT+CSQ\r\n$";
    uint8_t count,temp,temp1;
    uint8_t buffer_p,buffer[250];
    CREN1 = 0;
  	//soutdata(csq);
    soutdata((uint8_t *) "AT+CSQ\r\n$");
  	count=20;
  	buffer_p=0;
    CREN1 = 1;
    CLRWDT();
	do{ 
        T3CON = 0x71;
        TMR3H = 0x40;   //50ms
        TMR3L = 0;
        do{
			if( RC1IF==1 )
			{                  
				temp = RC1REG;
          		buffer[buffer_p]=temp;
		  		if(++buffer_p>=31)
		    		buffer_p=30;
		  		if(temp==0x0a)
		  		{
            		if(buffer[0]=='+'&&buffer[2]=='S'&&buffer[4]==':')
					{
              			temp=buffer[6];
			  			temp1=buffer[7];
			  			if(temp1==',')			  			
			    			temp = (uint8_t) (temp & 0x0f);
			  			else			  			
							temp = (uint8_t)((temp & 0x0f))*10U + (uint8_t)(temp1&0x0f);			  			
						signal_val = temp;
                        TMR3ON=0;
			  			return(temp);
					}
					buffer_p=0;
		  		}
			}
     	}while(TMR3IF==0);
        CLRWDT();
	 	TMR3IF=0;
  	}while(--count!=0);
  	TMR3IF=0;
  	TMR3ON=0;
	return(0);
} 
