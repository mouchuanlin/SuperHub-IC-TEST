//
// uart.c
//

#include "uart.h"
#include "emc_library.h"
#include "io.h"
#include "eeprom.h"
#include "modem.h"
#include "led.h"

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
    while (RC2IF == 1)
    {            
        temp = RC2REG;

        // Store data in rx2_buf
        rx2_buf.data[rx2_cnt] = temp;

        // If exceed MAX size, save to the last spot.
        if( ++rx2_cnt >= MAX_RX2_BUF_SIZE )
            rx2_cnt = MAX_RX2_BUF_SIZE - 1;

        // 7 bytes RF data in HEX - $ + 3byte ID + 1byte status + <CR> + <LF>
        // TODO: This might be able to call process_RF_data() in infinite loop instead of processing in ISR.
        if( temp == LF )      // \n
            process_RF_data();          
    };
     	       	
}

void process_RF_data(void)
{
	//uint8_t temp;
    uint8_t zone;
    uint8_t id[6];
		
	// 7 bytes RF data in HEX - $ + 3byte ID + 1byte status + <CR> + <LF>
    // Cell configuration data saved in EEPROM is in ASCII. Need convert 3 byte ID to ASCII.
	// TODO: Can add CRC check after status byte.
	if(is_valid_rf_data())                    
	{                
		led_count = 10;
		// LED5 green ON - active LOW
		LED_RF_RX_ON();
        
		// Decode 6 bytes device ID
        decode_device_id(id);
        
        // Get zone# from device_id_table
		zone = get_zone_number(id);       //respond zone number(3~30),error=0
	   
		if( zone!=0 )
		{
			device_id_table[zone-3][7] = 0;     //clear supervisory count
			//LED4 Yellow ON - active LOW
            LED_RF_ID_MATCH_ON();
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

bool is_valid_rf_data(void)
{
	if( (rx2_buf.map.dollar == '$') && (rx2_buf.map.cr == CR) && (rx2_buf.map.lf == LF) )   
        return true;
    else
        return false;
}

void decode_device_id(uint8_t id[])
{
    uint8_t temp, i;
        
	// 7 bytes RF data in HEX - $ + 3byte ID + 1byte status + <CR> + <LF>
    // Convert 3 bytes of RF device ID from HEX to ASCII and store in id[0-5]. Ex. 0x62 0x72 0x75 -> "627275"
    for (i = 0; i < 3; i++)
    {
        // Upper nibble
        temp = (uint8_t) ((rx2_buf.map.rf_id[i] >> 4) & 0x0F);
        id[i*2] = hex_to_ascii(temp);
        
        // Lower nibble
        temp = (uint8_t) (rx2_buf.map.rf_id[i] & 0x0F);
        id[i*2+1] = hex_to_ascii(temp);
    }
}

uint8_t hex_to_ascii(uint8_t hex)
{
    if( hex >= 10 )
    {
        hex += 0x41;
        hex -= 10;
    }
    else 
        hex += 0x30;

    return hex;
}

void ACK_to_RF_receiver(void)
{
    uint8_t i;
    
    for(i = 0; i < MAX_RX2_BUF_SIZE; i++)
        out_sbuf2(rx2_buf.data[i]);    
}

void send_sensor_alarm(uint8_t zone, uint8_t id[])
{
    if (zone != 0)
    {   
        //Supervisory,nc,nc,low BT,nc,test,tamper,trigger/alarm                                                                                           
		if( (rx2_buf.map.status & 0x01) != 0 )   //alarm
		{             
			// Ex. ID starts with 8 - Smoke detector, 6 - flood sensor
			if( id[0]=='8')
				enque_event(SMOKE_ALARM_T,zone);
			else if( id[0]=='6' ) 
				enque_event(FLOOD_T,zone); 
			else if( id[0]=='2' )
				enque_event(CARBON_T,zone);
			else if( id[0]=='C' )
				enque_event(GLASS_T,zone);
			else if( id[0]=='9' )
				enque_event(MOTION_T,zone);
			else if( id[0]=='3' )
				enque_event(DOOR_T,zone);
			else if( id[0]=='1' )
				enque_event(PANIC_T,zone);
			else if( id[0]=='B' )
				enque_event(HVAC_T,zone);
			else if( id[0]=='5' )
				enque_event(APPLIANCE_T,zone);
			else if( id[0]=='4' )
				enque_event(RESERVE1_T,zone);
			else if( id[0]=='7' )
				enque_event(RESERVE2_T,zone);
			else if( id[0]=='A' )
				enque_event(RESERVE3_T,zone);
			else if( id[0]=='D' )
				enque_event(RESERVE4_T,zone);
			else if( id[0]=='E' )
				enque_event(RESERVE5_T,zone);
			else if( id[0]=='0' )
				enque_event(RESERVE6_T,zone);
			else if( id[0]=='F' )
				enque_event(RESERVE7_T,zone);
		}
        //test
		if( (rx2_buf.map.status &0x04) != 0 )
		{
			enque_event(TEST_PIN_T,zone);                            
		}    
         //Tamper open
		if( ((rx2_buf.map.status & 0x02) != 0) )
		{
			if( device_id_table[zone-3][8]==0 )
			{
				enque_event(TAMPER_OPEN_T,zone);     
				device_id_table[zone-3][8]=1; 
			}
		}
        else    //Tamper close
		{
			if( device_id_table[zone-3][8]==1 )
			{
				enque_event(TAMPER_CLOSE_T,zone);     
				device_id_table[zone-3][8]=0; 
			}
		}
        //Low Battery
		if( (rx2_buf.map.status & 0x10) != 0 )
		{
			enque_event(LOW_BATTERY_T,zone);                            
		}     
        //Supervisory
		if( (rx2_buf.map.status & 0x80) != 0 )
		{
			enque_event(SUPERVISORY_T,zone);                            
		}     
	}	
}