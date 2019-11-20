//
// telit.c
//
#include <stdio.h>

#include "io.h"
#include "eeprom.h"
#include "emc_library.h"
#include "telit.h"
#include "modem.h"
#include "timer.h"


void TL_module_first_run(void)
{    
    const uint8_t led_910[] = "AT#GPIO=1,0,2\r\n$";   //910
    const uint8_t led_866[] = "AT#GPIO=7,0,2\r\n$";   //866
	const uint8_t led1[] = "AT#SLED=4\r\n$";

    //led
    soutdata((uint8_t *) "ATE1\r\n$");
    wait_ok_respond(20);
    if( Module_type==LE910)
        soutdata(led_910);
    else 
        soutdata(led_866);
    
    wait_ok_respond(40);
    soutdata(led1);
    wait_ok_respond(40);    
    //time
}

//---------------------------------------------------
uint8_t TL_internet_init(void)
{
    // Socket Configuration - #SCFG
    const uint8_t scfg[]="AT#SCFG=1,3,300,90,200,50\r\n$";
    // GPRS Attach Or Detach - +CGATT
	const uint8_t cgatt[]="AT+CGATT=1\r\n$";
    // Define PDP context- +CGDCONT
	const uint8_t cgdcont[]="AT+CGDCONT=3,\"IP\",\"$";	
    //uint8_t const cgdcont[]="AT+CGDCONT=1,\"IP\",\"internet\"\r\n$";	
    const uint8_t sgact[]="AT#SGACT=3,1\r\n$";    
	uint8_t cnt,temp,count;
    uint8_t buffer_p,buffer[32];
    uint8_t out_cmd[50];
    
    CREN1 = 0;
    delay5ms(100);
    // AT#SCFG=1,3,300,90,200,50
	soutdata(scfg);
	delay5ms(100);
    // AT+CGATT=1
	soutdata(cgatt);
//	if( wait_ok_respond(200)=='N' )
		wait_ok_respond(200);
    CREN1 = 0;
   	delay5ms(100);
        
    //send_at_command("AT+CGDCONT=3,\"IP\",\"11583.mcs\"\r\n$");     
    sprintf((char *)out_cmd, (char *)"AT+CGDCONT=3,\"IP\",\"%s\"\r\n$", page0_eeprom.map.APN);
    send_at_command(out_cmd);
    
//	if( wait_ok_respond(200)=='N' )
		wait_ok_respond(40);
   	delay5ms(100);
    
    CREN1 = 0;
    // AT#SGACT=3,1
	soutdata(sgact);
	count = 0;
    buffer_p = 0;
    RCIE = 0;
    CREN1 = 1;
	do{ 
        T3CON = 0x71;
        TMR3H = 0x40;   //50ms
        TMR3L = 0;
        do{
			if( RC1IF==1 )
			{                  
				temp = RC1REG;
				buffer[buffer_p] = temp;
				if( ++buffer_p>=32 )				
					buffer_p = 31;			
		  		if( temp==LF )	//Network opened
				{
                    // #SGACT: 10.133.224.83
					//if( buffer[0]=='#'&&buffer[1]=='S'&&buffer[2]=='G'&&buffer[3]=='A'&&buffer[4]=='C'&&buffer[5]=='T'&&buffer[6]==':' )
                    if (strncmp((const char *)buffer, (const char *)"#SGACT:", 7) == 0)
                    {
                        TMR3ON = 0;
                        CREN1 = 0;
                        return('K');
                    }
					buffer_p = 0;
			 	}
        	}
     	}while(TMR3IF==0);
        CLRWDT();
        TMR3IF = 0;
  	}while(--count!=0);
    
    TMR3IF = 0;
  	TMR3ON = 0;
    CREN1 = 0;
	delay5ms(100);
    return('E');
}

//---------------------------------------------------
uint8_t TL_connection_open(uint8_t type)
{
	uint8_t port[5];
    uint8_t out_cmd[50];

    // Check IP address EEPROM. If # then there is no IP address has been stored.
    if(!is_ip_exists(type))
        return('E');
    
    strncpy((char *)port, (const char *)page0_eeprom.map.PORT1, 4);
    port[4] = '\0';
    
    //send_at_command("AT#SD=1,0,2020,\"12.12.201.82\",0,0,1\r\n$");  
    sprintf((char *)out_cmd, (const char *)"AT#SD=1,0,%s,\"%s\",0,0,1\r\n$", port, page0_eeprom.map.IP1);
    send_at_command(out_cmd);
  
	//Connect ok / fail
//	buffer_p = 0;
//	cnt = 200;
    RCIE = 0;
	CREN1 = 1;

    return wait_ok_respond(40);
}

//---------------------------------------------------
void TL_connection_close(void)
{
	const uint8_t TCP_close[]="AT#SH=1\r\n$";

    CREN1 = 0;
	delay5ms(200);
	soutdata(TCP_close);
	delay5ms(200);
}

//---------------------------------------------------
void TL_internet_close(void)
{
	const uint8_t NT_close[]="AT#SGACT=3,0\r\n$";

    CREN1 = 0;
	delay5ms(200);
	soutdata(NT_close);
	delay5ms(200);
}

uint8_t TL_send_data_to_server(void)
{
    const uint8_t send[]="AT#SSENDEXT=1,$";
    uint8_t buffer_p;
	uint8_t cnt,temp,as;
    uint8_t tp_cnt;
    
    tp_cnt = enc_cnt;
    if( page0_eeprom.map.ENCRYPTION == 0 )
        tp_cnt--;
    CREN1 = 0;
    soutdata(send);
    cnt = tp_cnt;
    // Convert to ASCII - 0x30 is 0
    out_sbuf((cnt/100)+0x30);
    cnt %= 100;
    out_sbuf((cnt/10)+0x30);
    out_sbuf((cnt%10)+0x30);
    out_sbuf('\r');
    //out_sbuf('\n');
    RCIE = 0;
    CREN1 = 1;
    as = 200;
    do{ 
        cnt = 0;
        do{
            buffer_p = 0;
            do{
                if( RC1IF==1 )			              
                	temp = RC1REG;
            }while(++buffer_p!=0&&temp!='>');
            CLRWDT();
        }while(++cnt!=0&&temp!='>');     
    }while(++as!=0&&temp!='>');  
    CREN1 = 0;
    delay5ms(100);
   // soutdata(ass);
    cnt = 0;
    do{
        temp = rsp_buffer[cnt];
        out_sbuf(temp);
    }while(++cnt<tp_cnt);
    CLRWDT();
    RCIE = 0;
    CREN1 = 1;
	buffer_p = 0;
	cnt = 200;
    RCIE = 0;
	CREN1 = 1;
	temp = wait_ok_respond(200);        //200*50ms=10sec
    return(temp);
}

uint8_t TL_receive_data_from_server(void)
{
//      AT#SRECV=1,20
//      #SRECV: 1,9
//      +++123456
//
//      OK
    const uint8_t SRECV[]="AT#SRECV=1,200\r\n$";
    uint8_t buffer_p,buffer[250];
	uint8_t cnt,temp,type,err;
    
    CREN1 = 0;
    soutdata(SRECV);
    RCIE = 0;
    CREN1 = 1;
    cnt = 100;
    type = 0;
    buffer_p = 0;
    debug_p = 0;
    err = 0;
    //GIE = 0;
    //TMR0IE = 0;
     do{ 
        T3CON = 0x71;
        TMR3H = 0x40;   //50ms
        TMR3L = 0;
        do{
			if( RC1IF==1 )
			{                  
				temp = RC1REG;
				buffer[buffer_p] = temp;
                #ifdef DEBUG
                debug_buffer[debug_p++] = temp;
                #endif
				if( ++buffer_p>=250 )				
                 buffer_p = 249;			
		  		if( temp==0x0a && buffer[buffer_p-2] == 0x0d )	//Network opened
				{
                    if( buffer[0]=='E'&&buffer[1]=='R'&&buffer[2]=='R'&&buffer[3]=='O'&&buffer[4]=='R' )                     
                    {
                        TMR3ON = 0;
                        CREN1 = 0;
                        return('E');
                    }
                    if( type==0 )
                    {
                        if( buffer[0]=='#'&&buffer[1]=='S'&&buffer[2]=='R'&&buffer[3]=='E'&&buffer[4]=='C'&&buffer[5]=='V'&&buffer[6]==':' )
                            type = 1;
                    }else
                    {
                        temp = chk_data_type(&buffer[0],buffer_p);
                        if( temp=='E' )
                            temp = 'R';
                        return(temp);
                    }
                    buffer_p = 0;
			 	}
        	}
     	}while(TMR3IF==0);
        CLRWDT();
        TMR3IF = 0;
  	}while(--cnt!=0);
    TMR3IF = 0;
  	TMR3ON = 0;
    return('E');
}

bool is_ip_exists(uint8_t type)
{
    uint8_t temp;
    
    // Check IP address EEPROM. If # then there is no IP address has been stored.
    if (!page0_eeprom.map.IP1[0] && !page0_eeprom.map.IP2[0] && !page0_eeprom.map.IP3[0] && !page0_eeprom.map.IP4[0])
        return false;
    else
        return true;
}
