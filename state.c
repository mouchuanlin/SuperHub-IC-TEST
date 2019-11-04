//
// state.c
//

#include "state.h"
#include "emc_library.h"
#include "io.h"
#include "modem.h"
#include "timer.h"
#include "uart.h"

extern state_t myState;

void check_state(void)
{
    check_RF_device();
    process_ADC();
    process_supervisory();
    
    if (hub_type == SUPER_HUB)
        check_alarm_tamper();  
    
    
    process_event_queue();
        
    if (process_restart())
    {
        myState = SEND_TEST;
    }
    
    switch (myState)
    {
        case POWER_UP:
            break;
        case SLEEP:
            break;            
        case LISTEN_SMS:
			//restart_modem();
			start_modem();
            break;
        case ADD_SENSOR:
            add_sensor();
            break;   
        case DEL_SENSOR:
            delete_sensor();
            break; 
        case SEND_TEST:
            start_modem();
            break;      
        case RF_INTERRUPT:
            break;
        case TEMPER_INT:
            break;
        case SUPERVISORY:
            break;
        case ADC:
            break;           
        case OPERATIONAL:
        case OTA_BOOT:
            break;
    }
}

bool process_restart()
{	
	if( (retry_count == 0 ) && (stack_buffer[0][0] != 0) && (IP_type == 1) )        
	{                   
		MD_POWER = POWER_OFF;
		delayseconds(1);
        MD_RESET = 0;

		//goto module_start;
		return true;
	}     	
	
	return false;
}

void check_alarm_tamper(void)
{
    // TAMPER_PIN == 1 - close
    if( tamper_status!=0&&TAMPER_PIN==1  )
    {
        if( ++tamper_status>2 )
        {
            if( first_tamper==0 )                    
                add_event(TAMPER_CLOSE_T,1);
            first_tamper = 0;
            tamper_status = 0;
            SPK = 0;
        }
    }
}

//
 // Primary loop that controls add_sensor 
//
void add_sensor(void)
{
    
}

//
// Primary loop that controls delete sensor
//
void delete_sensor(void)
{

}

void process_supervisory(void)
{
	if( chk_supervisory>=2 )       //2 hour
	{
		check_supervisory();
		chk_supervisory = 0;
	}
}

uint8_t check_supervisory()
{
    uint8_t cnt1,cnt2,temp;
    for( cnt1=0;cnt1<16;cnt1++ )    //28
    {
        if( device_id_table[cnt1][0]!=0 )
        {
            // Supervisory every 2 hours. 7 days = 84
            if( device_id_table[cnt1][7] < 84 )         // 12 = 1Day  84 = 7Day
            {
                if( ++device_id_table[cnt1][7] >= 84 )
                {            
                    add_event(SUPERVISORY_T,cnt1+3U);
                    device_id_table[cnt1][7] = 'S';
                }
            }
        }
        CLRWDT();
    }    
    return(0);   
}

void process_ADC(void)
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
		
        // ADRESH: ADC RESULT REGISTER HIGH (ADRESH) ADFM = 1
        // ADRESL: ADC RESULT REGISTER LOW (ADRESL) ADFM = 1
		ADC_data = (uint16_t) ((ADRESH<<8U)+ADRESL);
		ADC_data &= 0x03ff;
		ADON = 0;                               //748->2.80 V
		ADC_time = 0;                           //776->2.70 V
		if( ADC_data >776 )                     //806->2.60 V
		{                                       //838->2.50 V
			if( BT_L_respond==0 )
			{                    
				add_event(LOW_BATTERY_S,0);    
                // board battery - send out once a week
				BT_L_respond = BT_EOL_RESEND;
			}
		}else BT_L_respond = 0;
		FVREN = 0;
	}
}

//void process_RF_interrupt()
//{
//	if( WIFI_INT ==1 && RF_wait_count == 0 )
//	{
//		//LED_G = 0;
//		OSCCON = HIGH_FREQ_OSCCON;	// 4MHz
//		T0CON = HIGH_FREQ_T0CON;             //1*4000 = 50,000us
//		HL_freq = 1;
//		UART2_init();
//		CREN1 = 0;
//		RF_wait_count = 100;      
//		TMR0IE = 1;//
//		TMR0ON = 1;
//		CREN1 = 1;
//		RC2IE = 1;
//	}
//}

void check_RF_device(void)
{
    // RF interrupt
    if( WIFI_INT==1&&RF_wait_count==0 )
    {
        //LED_G = 0;
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
