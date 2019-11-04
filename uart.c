//
// uart.c
//

#include "uart.h"
#include "emc_library.h"
#include "io.h"
#include "eeprom.h"
#include "modem.h"

void UART_init(void)
{
    enable_UART();
            
    UART1_init(115200);
    //UART1_init(19200);
    UART2_init();
}

void UART1_init(uint32_t baudrate)
{
    // TxSTAx: TRANSMIT STATUS AND CONTROL REGISTER
    TX1STA = 0x26;       // 0010 0110 - 8-bit, 
    // RCSTAx: RECEIVE STATUS AND CONTROL REGISTER
    RC1STA = 0x90;       //1001 0000  
    // BAUDCONx: BAUD RATE CONTROL REGISTER
    BAUDCON1 = 0xc8;     //11001000
    
    // baudrate
    if (baudrate ==19200)
        SPBRG1 = 103;       // 19200 bps
    else
        SPBRG1 = 16;        // 115200

    SPBRGH1 = 0;    
}

//---------------------------------------------------
void UART2_init(void)
{   
    TX2STA = 0x26;       //00100110
    RC2STA = 0x90;       //10010000
    BAUDCON2 = 0xc8;     //11001000
    SPBRG2 = 207;       //9600bps
    SPBRGH2 = 0;
    WPUB5 = 1;
    RC2IE = 1;
}

////---------------------------------------------------
//void Uart_initial_115200(void)
//{
//    // TxSTAx: TRANSMIT STATUS AND CONTROL REGISTER
//    TX1STA = 0x26;       // 0010 0110 - 8-bit, 
//    // RCSTAx: RECEIVE STATUS AND CONTROL REGISTER
//    RC1STA = 0x90;       //1001 0000  
//    // BAUDCONx: BAUD RATE CONTROL REGISTER
//    BAUDCON1 = 0xc8;     //11001000
//    // mlin 
//    SPBRG1 = 16;        // 115200 bps
////    SPBRG1 = 103;       // 19200 bps
//    SPBRGH1 = 0;
//}

//---------------------------------------------------
//void disable_UART(void)
//{
//    RC1STA = 0x00;       //10010000
//    MD_RX = 0;
//    MD_RX_TRIS = OUTPUT;
//    MD_TX = 1;
//    MD_TX_TRIS = INPUT;
//    RC1IE = 0;
//    
//    RC2STA = 0x00;       //10010000
//   // WIFI_RX = 0;
//  //  WIFI_RX_TRIS = OUTPUT;
//   // WIFI_TX = 0;
//   // WIFI_TX_TRIS = OUTPUT;
//    RC2IE = 0;
//}

//void disable_UART1()
//{
//    RC1STA = 0x00;       //10010000
//    MD_RX = 0;
//    MD_RX_TRIS = OUTPUT;
//    MD_TX = 1;
//    MD_TX_TRIS = INPUT;
//    RC1IE = 0;
//}

//---------------------------------------------------
void enable_UART(void)
{
    MD_RX = 1;
    MD_RX_TRIS = OUTPUT;
    MD_TX = 1;
    MD_TX_TRIS = INPUT;
}

// UART1 (to OTA/modem) ISR
void UART1_ISR(void)
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
void UART2_ISR(void)
{
	uint8_t temp;
	
    // RC2IE: EUSART2 Receive Interrupt Enable bit
    if ((RC2IE == 1) && (RC2IF == 1))
    {
        while (RC2IF == 1)
        {            
            temp = RC2REG;
			
            // Store data in rx2_buf
            rx2_buf[rx2_cnt] = temp;
            
            // If exceed MAX size, save to the last spot.
            if( ++rx2_cnt >= MAX_RX2_BUF_SIZE )
                rx2_cnt = MAX_RX2_BUF_SIZE - 1;
            
            // 7 bytes RF data in HEX - $ + 3byte ID + 1byte status + <CR> + <LF>
            // TODO: This might be able to call process_RF_data() in infinite loop instead of processing in ISR.
			if( temp == LF )      // \n
				process_RF_data();          
        };
       // RC1IF = 0;
    }        	       	
}

void process_RF_data(void)
{
	//uint8_t temp;
    uint8_t zone,cnt;
    uint8_t id[6];
		
    // We are expecting to receive 7 bytes of HEX data from RX receiver module.
    // In case we receive more than 7 byte of data.
	if( rx2_cnt>=7 )
	{
		cnt = 0;
		if( (rx2_buf[rx2_cnt-7U]=='$') && (rx2_buf[rx2_cnt-2U]== CR) )                    
		{
			for(zone=rx2_cnt-7U; zone<rx2_cnt; zone++ )
			{
				rx2_buf[cnt++] = rx2_buf[zone];
			}
			rx2_cnt = 7;
		}
	}
	
	// 7 bytes RF data in HEX - $ + 3byte ID + 1byte status + <CR> + <LF>
    // Cell configuration data saved in EEPROM is in ASCII. Need convert 3 byte ID to ASCII.
	// TODO: Can add CRC check after status byte.
	if( (rx2_buf[0]=='$') && (rx2_buf[5] == CR) && (rx2_buf[6] == LF) )                    
	{                
		led_count = 10;
		LED_RX_IN = 0;  // green ON - active LOW
		
		// Decode 6 bytes device ID
        decode_device_id(id);
        
        // TODO: don't need this???
		//temp = rx2_buf[4];

        // Get zone# from device_id_table
		zone = get_zone_number(id);       //respond zone number(3~30),error=0
	   
		if( zone!=0 )
		{
			device_id_table[zone-3U][7] = 0;     //clear supervisory count
			LED_RX_OUT = 0;                     // Yellow ON - active LOW
		}
        // Real sensor device alarms.
		if( learning_mode==KEY_NONE )
		{
			send_sensor_alarm(zone, id);
		}
        else
        {
            // Button 5-2/5-3 case.
			if( (learning_mode==KEY_ADD_ID) && (zone==0) )
				zone = add_ID(id); 
            
            // TODO: Why del_ID() input zone instead of id???
            else if( (learning_mode==KEY_DEL_ID) && (zone!=0) )
				zone = del_ID(zone); 
		}    
        
		// Send response to RF receiver. Same data as we received.
        ACK_to_RF_receiver();
	}
	rx2_cnt = 0;
	CREN2 = 0;
	NOP();
	CREN2 = 1;   
}

void decode_device_id(uint8_t id[])
{
    uint8_t temp;
        
	// 7 bytes RF data in HEX - $ + 3byte ID + 1byte status + <CR> + <LF>
    // Convert 3 bytes ID from HEX to ASCII
    rx2_cnt = 1;                                             
    do{
        // Convert from HEX to ASCII
        temp = (uint8_t) ((rx2_buf[rx2_cnt] >> 4) & 0x0f);
        if( temp>=10 )
        {
            temp += 0x41;
            temp -= 10;
        }else temp += 0x30;
        
        id[(rx2_cnt-1)*2] = temp;
    //    out_sbuf2(temp);////
        temp = (uint8_t) (rx2_buf[rx2_cnt] & 0x0f);
        if( temp>=10 )
        {
            temp += 0x41;
            temp -= 10;
        }else temp += 0x30;
        id[(rx2_cnt-1)*2+1] = temp;
    //    out_sbuf2(temp);//////
    }while(++rx2_cnt<4);     
}

void ACK_to_RF_receiver(void)
{
    for( uint8_t i=0; i<7; i++)
        out_sbuf2(rx2_buf[i]);    
}

void send_sensor_alarm(uint8_t zone, uint8_t id[])
{
	if( zone==0 )
	{
	/*    out_sbuf2('$');
		out_sbuf2('A');
		out_sbuf2('N');
		out_sbuf2(0x0d);
		out_sbuf2(0x0a);*/
	}
    else
    {   
        //Supervisory,nc,nc,low BT,nc,test,tamper,trigger/alarm                                                                                           
		if( (rx2_buf[4]&0x01)!=0 )   //alarm
		{             
			// ID starts with 8 - Smoke detector 
			// ID starts with 6 - flood sensor
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
        //test
		if( (rx2_buf[4]&0x04)!=0 )
		{
			add_event(TEST_PIN_T,zone);                            
		}    
         //Tamper open
		if( ((rx2_buf[4]&0x02)!=0) )
		{
			if( device_id_table[zone-3U][8]==0 )
			{
				add_event(TAMPER_OPEN_T,zone);     
				device_id_table[zone-3U][8]=1; 
			}
		}
        else    //Tamper close
		{
			if( device_id_table[zone-3U][8]==1 )
			{
				add_event(TAMPER_CLOSE_T,zone);     
				device_id_table[zone-3U][8]=0; 
			}
		}
        //Low Battery
		if( (rx2_buf[4]&0x10)!=0 )
		{
			add_event(LOW_BATTERY_T,zone);                            
		}     
        //Supervisory
		if( (rx2_buf[4]&0x80)!=0 )
		{
			add_event(SUPERVISORY_T,zone);                            
		}     
		/*out_sbuf2('$');
		out_sbuf2('A');
		out_sbuf2('S');
		out_sbuf2(0x0d);
		out_sbuf2(0x0a);*/
	}	
}