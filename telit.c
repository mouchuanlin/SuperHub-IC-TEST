//
// telit.c
//

#include "io.h"
#include "eeprom.h"
#include "emc_library.h"
#include "telit.h"


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
    
    // AT+CGDCONT=3,"IP","c2.korem2m.com"
	soutdata(cgdcont);
	cnt = 0x10;
	do{
        // APN - 35# - "c2.korem2m.com"
		temp = read_ee(0x00,cnt);
		if( temp!='#' )
			out_sbuf(temp);
		cnt++;
	}while( temp!='#' );
	out_sbuf('"');
	out_sbuf(0x0d);
	out_sbuf(0x0a);
//	if( wait_ok_respond(200)=='N' )
		wait_ok_respond(200);
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
		  		if( temp==0x0a )	//Network opened
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
	const uint8_t netconnect[]="AT#SD=1,0,$";
    const uint8_t net_2[]="\",0,0,1\r\n$";
    //uint8_t const netconnect[]="AT#SD=1,0,2020,\"211.22.241.58\",0,0,1\r\n$";
    uint8_t buffer_p,buffer[32];
	uint8_t cnt,temp;
	uint16_t port;
    
    // Check IP address EEPROM. If # then there is no IP address has been stored.
    if(!is_ip_exists(type))
        return('E');
    
    CREN1 = 0;
    // AT#SD=1,0,2021,"72.197.171.234",0,0,1
	soutdata(netconnect);
    
    //------ port ------
    // Get 2 bytes of port number
    if( type==0x01 )
		cnt = 0xB0;
	else if( type==0x02 )
		cnt = 0xB2;
	else if( type==0x03 )
		cnt = 0xB4;
	else 
        cnt = 0xB6;

	port = (uint16_t) (read_ee(0,cnt)<<8U);
	port += read_ee(0,cnt+1U);
    

	cnt = 0;
	temp = port/10000;
	if( temp!=0 )
	{	
		cnt = 1;
		out_sbuf((uint8_t)(temp+0x30));
	}
	port %= 10000;
	temp = port/1000;
	if( temp!=0||cnt==1 )
	{
		cnt = 1;
		out_sbuf( (uint8_t) (temp+0x30) );
	}
	port %= 1000;
	temp = port/100;
	if( temp!=0||cnt==1 )
	{
		cnt = 1;
		out_sbuf( (uint8_t) (temp+0x30) );
	}
	port %= 100;
	temp = port/10;
	if( temp!=0||cnt==1 )
	{
		cnt = 1;
		out_sbuf( (uint8_t) (temp+0x30) );
	}
	temp = port%10;
	out_sbuf( (uint8_t) (temp+0x30) );
    out_sbuf(',');
    out_sbuf('"');
    
    //------ IP ------    
    if( type==0x01 )
		cnt = 0x30;
	else if( type==0x02 )
		cnt = 0x50;
	else if( type==0x03 )
		cnt = 0x70;
	else cnt = 0x90;
	do{
		temp = read_ee(0,cnt);
		if( temp!='#' )
			out_sbuf(temp);
		cnt++;
	}while( temp!='#' );     
    CLRWDT();
	soutdata(net_2);
	out_sbuf(0x0d);
	out_sbuf(0x0a);

	//Connect ok / fail
	buffer_p = 0;
	cnt = 200;
    RCIE = 0;
	CREN1 = 1;
	temp = wait_ok_respond(500);
    return(temp);
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
    if( encryption==0 )
        tp_cnt--;
    CREN1 = 0;
    soutdata(send);
    cnt = tp_cnt;
    // Convert to ASCII - 0x30 is 0
    out_sbuf((cnt/100U)+0x30);
    cnt %= 100;
    out_sbuf((cnt/10U)+0x30);
    out_sbuf((cnt%10U)+0x30);
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
		  		if( temp==0x0a && buffer[buffer_p-2U] == 0x0d )	//Network opened
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
    if( type==0x01 )
        temp = read_ee(EE_PAGE0, IP1_ADDR);
    else if( type==0x02 )
        temp = read_ee(EE_PAGE0, IP2_ADDR);
    else if( type==0x03 )
        temp = read_ee(EE_PAGE0, IP3_ADDR);
    else 
        temp = read_ee(EE_PAGE0, IP4_ADDR);    
    
    if( temp=='#' )
        return false;
    else
        return true;
}
