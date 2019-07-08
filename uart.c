//
// uart.c
//

#include <pic18f26k22.h>
#include <xc.h>

#include "uart.h"
#include "emc_library.h"
#include "io.h"

void UART_init()
{
    Enable_Uart_IO();
            
    UART1_init(115200);
    UART2_init();
}

void UART1_init(uint16_t baudrate)
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
void UART2_init()
{   
    TX2STA = 0x26;       //00100110
    RC2STA = 0x90;       //10010000
    BAUDCON2 = 0xc8;     //11001000
    SPBRG2 = 207;       //9600bps
    SPBRGH2 = 0;
    WPUB5 = 1;
    RC2IE = 1;
}////---------------------------------------------------
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
void Uart_disable(void)
{
    RC1STA = 0x00;       //10010000
    MD_RX = 0;
    MD_RX_TRIS = OUTPUT;
    MD_TX = 1;
    MD_TX_TRIS = INPUT;
    RC1IE = 0;
    
    RC2STA = 0x00;       //10010000
   // WIFI_RX = 0;
  //  WIFI_RX_TRIS = OUTPUT;
   // WIFI_TX = 0;
   // WIFI_TX_TRIS = OUTPUT;
    RC2IE = 0;
}

//---------------------------------------------------
void Enable_Uart_IO(void)
{
    MD_RX = 1;
    MD_RX_TRIS = OUTPUT;
    MD_TX = 1;
    MD_TX_TRIS = INPUT;
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
                    LED_RX_IN = 0;  // green ON - active LOW
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
                        device_id_table[zone-3][7] = 0;     //clear supervisory count
                        LED_RX_OUT = 0; // Yellow ON - active LOW
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
                                // ID starts with 8 - Smoke detector 
                                // ID starts with 6 - flood sensor
                                // 
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
                                if( device_id_table[zone-3][8]==0 )
                                {
                                    add_event(TAMPER_OPEN_T,zone);     
                                    device_id_table[zone-3][8]=1; 
                                }
                            }else                           //Tamper close
                            {
                                if( device_id_table[zone-3][8]==1 )
                                {
                                    add_event(TAMPER_CLOSE_T,zone);     
                                    device_id_table[zone-3][8]=0; 
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