#include "initial.h"
#include "EE_library.h"
#include "System_Library.h"
#include <pic18f26k22.h>


void out_sbuf(uint8_t tmp)
{
    while( TRMT1==0 );
    TX1REG = tmp;
}

void soutdata(uint8_t *dat)
{
    uint8_t temp,cnt;
    cnt = 0;
    do{
        temp = dat[cnt++];
        if( temp!='$' )
            out_sbuf(temp);
    }while(temp!='$');
}

void out_sbuf2(uint8_t tmp)
{
    while( TRMT2==0 );
    TX2REG = tmp;
}

//---------------------------------------------------
uint8_t wait_ok_respond(uint8_t count)
{
  	uint8_t temp;
    uint8_t buffer[20],buffer_p;
    CREN1 = 0;
    RC1IE = 0;
    CREN1 = 1;
    buffer_p = 0;
  	do{
        T3CON = 0x71;
        TMR3H = 0x40;   //50ms
        TMR3L = 0;
        TMR3IF = 0;
     	do{
        	if( RC1IF==1)
        	{	   
		 	 	temp=RC1REG;
                buffer[buffer_p] = temp;
                if( ++buffer_p>=20 )
                    buffer_p = 19;
                if( temp==0x0a )
                {
                    if( buffer[0]=='O'&&buffer[1]=='K' )
                        temp = 'K';
                    else if(buffer[0]=='E'&&buffer[1]=='R'&&buffer[2]=='R' )
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
     	}while(TMR3IF==0);
        TMR3IF = 0;
        //LED = ~LED;
  	}while(--count!=0);
  	TMR3ON = 0;
    RC1IE = 1;
  	return('N');
}

void module_first_run(void)
{    
#ifdef M_910
    uint8_t const led[] = "AT#GPIO=1,0,2\r\n$";
#else
    uint8_t const led[]="AT^SCFG=\"GPIO/mode/SYNC\",\"std\"\r\n$";
  	
   // uint8_t const led[] = "AT#GPIO=7,0,2\r\n$";
#endif
	//uint8_t const led1[] = "AT#SLED=4\r\n$";
    uint8_t const led1[]="AT^SLED=1\r\n$";      

    //led
    soutdata("ATI\r\n$");
    wait_ok_respond(20); 
    soutdata(&led);
    wait_ok_respond(40);
    soutdata(&led1);
    wait_ok_respond(40); 
  /*  soutdata("AT^SCFG=\"Sim/CCIN\",0\r\n$");
    wait_ok_respond(40);*/
    //time   
    soutdata("AT^SPOW=1,0,0\r\n$");
    wait_ok_respond(40);
}

uint8_t check_module_run(void)
{
    const uint8_t at[]="AT\r\n$";
  	uint8_t temp,cnt;
	uint8_t buffer[32];
	uint8_t buf_cnt=0;
 
    CREN1 = 0;
	soutdata(&at);
    T3CON = 0x71;
    TMR3H = 0x40;   //50ms
    TMR3L = 0;
    TMR3IF = 0;    
    cnt=20;         //50ms*20=1sec
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
				buffer[buf_cnt] = temp;
				if( temp=='K' )
                    return('K');
			}
		}while(TMR3IF==0 );
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
	soutdata(&cpin);
    T3CON = 0x71;
    TMR3H = 0x40;   //50ms
    TMR3L = 0;
    TMR3IF = 0;    
    cnt=20;         //50ms*20=1sec
    RCIE = 0;
    CREN1 = 1;
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
        soutdata(&creg);
    else soutdata(&cereg);
    T3CON = 0x71;
    TMR3H = 0x40;   //50ms
    TMR3L = 0;
    TMR3IF = 0;    
    cnt=20;         //50ms*20=1sec    
    RC1IE = 0;
    CREN1 = 1;
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
		}while(TMR3IF==0 );
		TMR3IF = 0;
	}while(--cnt!=0);
	TMR3ON = 0;
    RC1IE = 1;
	return('E');
}

//---------------------------------------------------
uint8_t internet_init(void)
{

    uint8_t const gprs3[]="AT^SISS=1,srvType,Socket\r\n$";
    uint8_t const gprs4[]="AT^SISS=1,conId,1\r\n$";
    
	uint8_t const cgatt[]="AT+CGATT=1\r\n$";
	uint8_t const cgdcont[]="AT+CGDCONT=1,\"IP\",\"$";	
    //uint8_t const cgdcont[]="AT+CGDCONT=1,\"IP\",\"internet\"\r\n$";	
    uint8_t const sgact[]="AT^SICA=1,1\r\n$";    
	uint8_t cnt,temp,count;
    uint8_t buffer_p,buffer[32];

    CREN1 = 0;
   // delay5ms(100);
	//soutdata(scfg);
	delay5ms(100);
	soutdata(cgatt);
//	if( wait_ok_respond(200)=='N' )
		wait_ok_respond(200);
    CREN1 = 0;
   	delay5ms(100);
	soutdata(cgdcont);
	cnt = 0x10;
	do{
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
	soutdata(&sgact);
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
					if( buffer[0]=='O'&&buffer[1]=='K' )
                    {
                                                 delay5ms(100);
                        soutdata(gprs3);
                         delay5ms(100);
                        soutdata(gprs4);
                        delay5ms(100);
                        TMR3ON = 0;
                        CREN1 = 0;
                        return('K');
                    }
					buffer_p = 0;
			 	}
        	}
     	}while(TMR3IF==0);
        TMR3IF = 0;
  	}while(--count!=0);
    TMR3IF = 0;
  	TMR3ON = 0;
    CREN1 = 0;
	delay5ms(100);
    return('E');
}

//---------------------------------------------------
uint8_t connection_open(uint8_t type)
{
    //at^siss=1,address,"socktcp://sohu.com:80"
	uint8_t const netconnect[]="AT^SISS=1,address,\"socktcp://$";  
    uint8_t const net_2[]="\"\r\n$";
    uint8_t const open[]="AT^SISO=1\r\n$";
    uint8_t buffer_p,buffer[32];
	uint8_t cnt,temp,count;
	uint16_t port;
    
    if( type==0x01 )
        temp = read_ee(0x00,0x30);
    else if( type==0x02 )
        temp = read_ee(0x00,0x50);
    else if( type==0x03 )
        temp = read_ee(0x00,0x70);
    else temp = read_ee(0x00,0x90);
    if( temp=='#' )
        return('E');
    CREN1 = 0;
	soutdata(&netconnect);
    //------ IP ------    
    if( type==0x01 )
		cnt = 0x30;
	else if( type==0x02 )
		cnt = 0x50;
	else if( type==0x03 )
		cnt = 0x70;
	else cnt = 0x90;
	do{
		temp = read_ee(0x00,cnt);
		if( temp!='#' )
			out_sbuf(temp);
		cnt++;
	}while( temp!='#' );    
    out_sbuf(':');
    //------ port ------
    if( type==0x01 )
		cnt = 0xB0;
	else if( type==0x02 )
		cnt = 0xB2;
	else if( type==0x03 )
		cnt = 0xB4;
	else cnt = 0xB6;

	port = read_ee(0x00,cnt)<<8;
	port += read_ee(0x00,cnt+1);
	cnt = 0;
	temp = port/10000;
	if( temp!=0 )
	{	
		cnt = 1;
		out_sbuf( temp+0x30 );
	}
	port %= 10000;
	temp = port/1000;
	if( temp!=0||cnt==1 )
	{
		cnt = 1;
		out_sbuf( temp+0x30 );
	}
	port %= 1000;
	temp = port/100;
	if( temp!=0||cnt==1 )
	{
		cnt = 1;
		out_sbuf( temp+0x30 );
	}
	port %= 100;
	temp = port/10;
	if( temp!=0||cnt==1 )
	{
		cnt = 1;
		out_sbuf( temp+0x30 );
	}
	temp = port%10;
	out_sbuf( temp+0x30 );
   //-----------------------------    
	soutdata(&net_2);
    wait_ok_respond(200);
    
	//Connect ok / fail
    CREN1 = 0;
    soutdata(&open);
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
		  		if( temp==0x0a )	//Network opened    ^SISW: 1, 1
				{
					if( buffer[0]=='^'&&buffer[1]=='S'&&buffer[2]=='I'&&buffer[3]=='S'&&buffer[4]=='W'&&buffer[5]==':' )
                    {
                        TMR3ON = 0;
                        CREN1 = 0;
                        return('K');
                    }
					buffer_p = 0;
			 	}
        	}
     	}while(TMR3IF==0);
        TMR3IF = 0;
  	}while(--count!=0);
    TMR3IF = 0;
  	TMR3ON = 0;
    CREN1 = 0;
	delay5ms(100);
    return('E');
}

//---------------------------------------------------
void connection_close(void)
{
	uint8_t const TCP_close[]="AT^SISC=1\r\n$";

    CREN1 = 0;
	delay5ms(200);
	soutdata(&TCP_close);
	delay5ms(200);
}

//---------------------------------------------------
void internet_close(void)
{
	//uint8_t const NT_close[]="AT#SGACT=1,0\r\n$";

    CREN1 = 0;
	delay5ms(200);
	//soutdata(&NT_close);
	//delay5ms(200);
}

//---------------------------------------------------
uint8_t check_csq(void)
{  	
  	uint8_t const csq[]="AT+CSQ\r\n$";
    uint8_t count,temp,temp1;
    uint8_t buffer_p,buffer[250];
    CREN1 = 0;
  	soutdata(&csq);
  	count=20;
  	buffer_p=0;
    CREN1 = 1;
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
			    			temp=temp&0x0f;
			  			else			  			
							temp=(temp&0x0f)*10+(temp1&0x0f);			  			
						signal_val = temp;
                        TMR3ON=0;
			  			return(temp);
					}
					buffer_p=0;
		  		}
			}
     	}while(TMR3IF==0);
	 	TMR3IF=0;
  	}while(--count!=0);
  	TMR3IF=0;
  	TMR3ON=0;
	return(0);
} 
