#include "initial.h"
#include "EE_library.h"
#include "System_Library.h"
#include <pic18f26k22.h>
#include <xc.h>


void out_sbuf(uint8_t tmp)
{
    // TRMT: Transmit Shift Register Status bit
    while( TRMT1==0 )
        ;
    CLRWDT();
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
    uint8_t buffer[20]={0}, buffer_p = 0;
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
                if( ++buffer_p >= 20 )
                    buffer_p = 19;
                if (temp == LF)     // 0x0A, \n
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
        soutdata(&at4);
    else soutdata(&at3);
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
				if( temp==0x0a )
				{		  
					if( buffer[0]=='E'&&buffer[1]=='M'&&buffer[2]=='S'&&buffer[3]=='3'&&buffer[4]=='1' )
					{
                        Module_type = EMS31;
                        return('K');
                    }else if( buffer[0]=='P'&&buffer[1]=='L'&&buffer[2]=='S'&&buffer[3]=='8' )
					{
                        Module_type = PLS8;
                        return('K');
                    }else if( buffer[0]=='E'&&buffer[1]=='H'&&buffer[2]=='S'&&buffer[3]=='5' )
					{
                        Module_type = EHS5;
                        return('K');
                    }else if( (buffer[0]=='L'||buffer[0]=='U')&&buffer[1]=='E'&&buffer[2]=='9'&&buffer[3]=='1'&&buffer[4]=='0' )
					{
                        Module_type = LE910;
                        return('K');
                    }else if( (buffer[0]=='L'||buffer[0]=='U')&&buffer[1]=='E'&&buffer[2]=='8'&&buffer[3]=='6'&&buffer[4]=='6' )
					{
                        Module_type = LE866;
                        return('K');
                    }
					buf_cnt = 0;
				}
			}
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
	soutdata(&cpin);
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
        soutdata(&creg);
    else soutdata(&cereg);
    
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
  	uint8_t const csq[]="AT+CSQ\r\n$";
    uint8_t count,temp,temp1;
    uint8_t buffer_p,buffer[250];
    CREN1 = 0;
  	soutdata(&csq);
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
        CLRWDT();
	 	TMR3IF=0;
  	}while(--count!=0);
  	TMR3IF=0;
  	TMR3ON=0;
	return(0);
} 