//
// sms.c
//

#include "io.h"
#include "eeprom.h"
#include "sms.h"
#include "eeprom_setup.h"
#include "modem.h"
#include "timer.h"

#define SMS_BUF_LEN     160
uint8_t sms_p,sms_buffer[SMS_BUF_LEN]; 
uint8_t time[9];
uint8_t sms_phone[40];
uint8_t phone_cnt;
uint8_t access_code[7];
uint8_t x_cnt;

//---------------------------------------------------

void set_sms_init(void)
{
    const uint8_t cmgf[] = "AT+CMGF=1\r\n$";
    const uint8_t cpms[] = "AT+CPMS=\"ME\",\"ME\",\"ME\"\r\n$";
	const uint8_t cnmi[] = "AT+CNMI=1,1\r\n$";
    //sms
    CREN1 = 0;
    soutdata(cmgf);
    wait_ok_respond(40);
 //   soutdata(cpms);
 //   wait_ok_respond(40);
 //   soutdata(cnmi);
 //   wait_ok_respond(40);    
}

//---------------------------------------------------
uint8_t check_sms(void)
{
  	const uint8_t cmgl[]="AT+CMGL=\"ALL\"\r\n$";
  	uint8_t temp, a, b, c;
    uint8_t buffer[160];
    uint8_t buffer_p = 0;
    uint16_t count;
    
    CREN1 = 0;
	soutdata(cmgl);
    T3CON = 0x71;
    TMR3H = 0xa0;   //50ms
    TMR3L = 0;
    TMR3IF = 0;    
    
    // TODO: Doesn't this suppose to use SMS_WAIT_TIME???
    count = 1000;         //50ms*600=30sec
    a = b = c = 0;

    RCIE = 0;
    CREN1 = 1;
	do{ 
        // Timer 3 for 50 ms.
        reload_timer3_50ms();
       
        do{
            // RC1IF: EUSART1 Receive Interrupt Flag bit
			if( RC1IF==1 )
			{   
		  		temp = RC1REG;
          		buffer[buffer_p]=temp; 
          		if(++buffer_p==31)
            		buffer_p--;

          		if( temp == LF )	// 0x0A
          		{
          			// List Messages - +CMGL
          			// +CMGL: 1,"REC READ","+15039709528","","19/05/13,17:00:13-28"
		    		if((buffer[0]=='+') && (buffer[2]=='M') && (buffer[6]==' ') && (a==0) && (b==0) && (c==0)	)
					{
			  		//	TMR3ON = 0;
			  			a=buffer[7];
			  			b=buffer[8];
                        c=buffer[9];
						count = 100;
					}else if(buffer[0]=='O'&&buffer[1]=='K')		// OK response
					{
						if( a==0 && b==0 )
							return 0;					
						delay5ms(100);
              			temp = read_sms(a,b,c);
			  			delay5ms(100);
			  			delete_sms(a,b,c);			  		
						//if( temp=='W' )
						//	reset_system();
                        load_default();
                        check_ip_setting();
						return 1;
					}
                    count = 100;
					buffer_p=0;
          		}
        	}
            // Receive Overrun Error - clearing the CREN bit to clear the error.
			check_receive_overrun();
			
     	} while(TMR3IF==0 );

        CLRWDT();
		TMR3IF = 0;
  	}while(--count!=0);
    
  	TMR3ON = 0;
    load_default();
    RC1IE = 1;
    
	return 0;
}

void check_receive_overrun(void)
{
	// Receive Overrun Error - clearing the CREN bit to clear the error.
	if (OERR1 == 1)
	{
		CREN1 = 0;
		NOP();
		CREN1 = 1;
	}	
}

//void get_access_code(void)
//{
//    uint8_t temp = 0;
//            
//	// get access code from eeprom
//    do{
//        access_code[temp] = read_ee(EE_PAGE0, (uint8_t)(0xC0+temp));
//    }while(++temp<7);       
//}

//---------------------------------------------------
uint8_t read_sms(uint8_t a,uint8_t b,uint8_t c)
{
	//+CMGR: "REC READ","+886937608095",,"11/08/26,15:51:59+32"
	const uint8_t cmgr[]="AT+CMGR=$";
  	uint8_t temp,count,t_p;
  	uint8_t crt=0;
	uint16_t sp_tp,in_tp;
	uint8_t start_a,ct;
	uint8_t enter_cnt=0;
    
    CREN1 = 0;
	memset( sms_buffer,0xff,sizeof(sms_buffer) );
	memset( time,'0',sizeof(time) );
    temp = 0;
	
    // get access code
    //get_access_code();
	
  	soutdata(cmgr);
  	out_sbuf(a);
  	if(b!=',')
    	out_sbuf(b);
  	out_sbuf('\r');
    out_sbuf('\n');
  	count=100;
    CREN1 = 1;
  	sms_p=0;
  	do{
    	T3CON = 0x71;
        TMR3H = 0x40;   //50ms
        TMR3L = 0;
        do{
			if( RC1IF==1 )
			{ 
				// Get phone #
		  		temp=RC1REG;
          		if(crt==0&&temp==',')
		    		crt=1;
		  		else if(crt==1&&temp=='"')
		  		{
		    		crt=2;
					sms_phone[0]=temp;
					phone_cnt=1;
		  		}
		  		else if(crt==2)
		  		{
		    		if(temp==',')
					{
			  			crt=3;
			  			sms_p=0;
					}else
					{
						// store phone#
			  			sms_phone[phone_cnt]=temp;
			  			phone_cnt++;
					}
		  		}else if(crt==3)
				{
					if( temp==0x0a )
						crt = 6;
					else if( temp==',' )
					{
						t_p=0;
		    			crt=4;
					}
			   	}
				else if(crt==4)
				{
					// store time - actually not really use
					time[t_p]=temp;
					if( ++t_p>=9 )
					{
						crt=5;
						for( t_p=0;t_p<8;t_p++)
							time[t_p] = time[t_p+1];
						time[8]='"';
					}
				}
				else if(crt==5&&temp==0x0a)
		    		crt=6;
   	      		else if(crt==6)
		  		{
					// real SMS message data
					sms_buffer[sms_p] = temp;
					if( temp=='^' )
						enter_cnt = '^';
					if(++sms_p>=160 )
			  			sms_p=159;
					if(temp == LF)
					{
                        // TODO: set for length 4 now. Should be up to 7.
                        if (strncmp((const char *)&sms_buffer[0], (const char *)page0_eeprom.map.ACCESS_CODE, 4) != 0)										
							break;
						
			  			sms_buffer[sms_p-1] = 0xcc;
						
			 	 		crt=check_remote();
			  			if(crt=='K')				//setup function
			  			{						
							// do the setting
			    			temp = remote_setting();
							crt = temp;
							if( temp!='L'&&temp!='X' )
								temp = 0;
							send_respond(temp);
							return(crt);
			  			}
			  			return(1);
					}
		  		}
			}
     	}while(TMR3IF==0 );
        CLRWDT();
		TMR3IF = 0;
  	}while(--count!=0);
  	TMR3ON = 0;
    RC1IE = 1;
	return(0);
}

//---------------------------------------------------
void delete_sms(uint8_t a,uint8_t b,uint8_t c)
{
  	const uint8_t cmgd[]="AT+CMGD=$";
  	uint8_t temp,count;
    CREN1 = 0;
  	soutdata(cmgd);
	
#ifndef del_ok
		out_sbuf('*');
#endif
  	out_sbuf(a);
  	if(b!=',')
    {
    	out_sbuf(b);
        if( c!=',' )
            out_sbuf(c);
    }
 	out_sbuf(0x0d);
    out_sbuf(0x0a);
  	count=100;
    CREN1 = 1;
  	do{
        T3CON = 0x71;
        TMR3H = 0x40;   //50ms
        TMR3L = 0;
        do{
			if( RC1IF==1 )
			{        	
		  		temp=RC1REG;
		  		if(temp=='E'||temp=='K')
                {
                    TMR3IF=0;
                    TMR3IE=0;
		    		return;
                }
			}
     	}while(TMR3IF==0);
        CLRWDT();
	 	TMR3IF=0;
  	}while(--count!=0);
  	TMR3IF=0;
  	TMR3ON=0;
}

//---------------------------------------------------
uint8_t check_remote(void)
{
	uint8_t temp,addr;


	//----- password
	x_cnt = 0;
	addr = 0;		
	do{
		// 1111 
		temp = access_code[addr++];
		if( sms_buffer[x_cnt]==temp )
		{
			if( temp=='#' )
				break;
   		}
		else 
			return('E');
	}while(++x_cnt<0x07);
	
    CLRWDT();
	if( temp != '#' )
		return('E');
	x_cnt++;
	return('K');
}

//---------------------------------------------------
// query and set
uint8_t remote_setting(void)
{
	uint8_t temp,temp1,temp2,respond,cnt;
	uint8_t addr_tp,off_set,off_tp;
    uint8_t index = 0;

    for (uint8_t i = 0 ; i < SMS_BUF_LEN; i++)
        sms_buffer[i] = toupper(sms_buffer[i]);
    
    // Find the first command index in sms_buffer
    while (sms_buffer[index++] != '#')
        ;
        
	//addr_tp = index; 					// record first address
    addr_tp = x_cnt;

	//----- function code
	respond = 0x00;

    // 20#
    if (strstr((const char *)&sms_buffer[0], (const char *)"20#"))
    {
        x_cnt += 2;
        respond = 'X';
        // 1111#20sensor# - query sensor info
        if ( strstr((const char *)&sms_buffer[0], (const char *)"SENSOR#"))
        {
            off_set = 0;
            for( temp1=0;temp1<16;temp1++ )
            {
                if( temp1==0||temp1==4||temp1==9||temp1==14)
                {
                    rsp_buffer[off_set++] ='(';
                    if( temp1==0 )                        
                        rsp_buffer[off_set++] ='1';                        
                    else if( temp1==4 )
                        rsp_buffer[off_set++] ='5';
                    else if( temp1==9 )
                    {
                        rsp_buffer[off_set++] ='1';
                        rsp_buffer[off_set++] ='0';
                    }else if( temp1==14 )
                    {
                        rsp_buffer[off_set++] ='1';
                        rsp_buffer[off_set++] ='5';
                    }
                    rsp_buffer[off_set++] =')';
                }
                for( temp2=0;temp2<6;temp2++)
                {
                    temp = read_ee(1,(temp1*8)+temp2);
                    if( temp2==0&&temp==0 )
                        break;
                    rsp_buffer[off_set++] = temp;
                }                    
                rsp_buffer[off_set++] = ',';
            }     
            rsp_buffer[off_set-1] = 0x0d;
        }
        // 1111#20#all# - query all setting info
        else if(strstr((const char *)&sms_buffer[0], (const char *)"ALL#")) 
        {
            //sprintf((char *)&rsp_buffer[0], (char *)"V.%c.%c.%c-", VERSION[0], VERSION[1], VERSION[2]);
            rsp_buffer[0]='V';
            rsp_buffer[1]='.';
            rsp_buffer[2]= VERSION[0];
            rsp_buffer[3]='.';
            rsp_buffer[4]= VERSION[1];
            rsp_buffer[5]='.';
            rsp_buffer[6]= VERSION[2];
            rsp_buffer[7]='-';
            off_set = 8;
            temp= 1;
            do{
                if( (temp >= 1 && temp <= 12)||(temp>=14 && temp<=16)||((temp >= 31)&&(temp <= 37)) )
                    off_set = respond_setting(temp,off_set);
            } while (++temp<38);						
            rsp_buffer[off_set-1]=0x0d;
        }
        else
        {
            // Otherwise ERROR
            off_set = 0;
            do{
                temp = sms_buffer[x_cnt++];
                temp1 = sms_buffer[x_cnt++];
                if( !is_digit(temp) || !is_digit(temp1) )
                {
                    strncpy((char *)&rsp_buffer[off_set],(const char *)"Error ", 6);
                    off_set += 6;
                    break;
                }
                temp = (uint8_t) (((temp&0x0f)<<4) + (temp1&0x0f));
                temp1 = sms_buffer[x_cnt++];
                if( temp1=='#' )
                {
                    off_tp = respond_setting(temp,off_set);
                    if( off_tp == off_set )
                        break;
                    off_set = off_tp;
                }else break;
            }while(x_cnt<sms_p);			
            if( off_set!=0x00 )
                rsp_buffer[off_set-1]=0x0d;
            respond = 'X';
        }
    }

    
	// call function_all 
	if( respond!='R'&&respond!='E'&&respond!='L'&&respond!='X'&&respond!='W' )			// setting function
	{
		do{
            for (uint8_t i = 0; i < 100; i++)
                key[i] = 0x00;
            
			key_p = 0;
			respond = 'E';
			do{
				temp = sms_buffer[x_cnt++];
				key[key_p++] = temp;
				if( temp==','||temp==0xcc )
				{
					key[key_p-1] = '#';
					if( key_p<5 )
						respond = 'E';
					else if(key[2]=='#'||key[3]=='#' )
					{					
						respond = 'K';
					}
                    else 
                        respond = 'E';
                        break;
				}
				if( key_p>=36 )
				{
					respond = 'E';
					break;
			  	}
			}while( x_cnt<sms_p );

			if( respond=='K' )
			{
				// all setting 
				respond = sms_setup_functions();
			}
		}while( (x_cnt<sms_p)&&respond=='K' );
		

		// sms ending
		if( temp==0xcc )
			x_cnt-=1;
	}
	
	
	if( respond!='R'&&respond!='L'&&respond!='X'&&respond!='W'  )
	{
		temp1 = 0;
		for( temp=addr_tp;temp< x_cnt;temp++ )
		{
			rsp_buffer[temp1] = sms_buffer[temp];
			temp1++;
		}
		if( respond=='E' )
		{
			//temp1 += sprintf(rsp_buffer+temp1,"%s"," Error");
            strncpy((char *)&rsp_buffer[temp1],(const char *)"Error", 5);            
            temp1 += 5;
		}
		rsp_buffer[temp1] = 0x0d;
	}
	return(respond);
}

//---------------------------------------------------
void send_respond(uint8_t type)
{
	uint8_t temp;
  	const uint8_t cmgs[]="AT+CMGS=\"$"; 
  	uint8_t a,b,c;
	uint8_t page,addr,ad_tp;
	uint8_t x_tp,x_vl,over_dp;
    
	page = 2;
	x_tp = 0;
	over_dp = 0;
	do{
        CLRWDT();
		ad_tp = 0;
		if( type=='L' )
		{
			do{
				a = ad_tp;
				temp = read_ee(page,0);
				if( temp>='0'&&temp<='9' )
				{
					addr = 0;
					if( a!=0 )
						rsp_buffer[a-1] = '-';
					do{
						temp = read_ee(page,addr);
						rsp_buffer[a] = temp;
						a++;
						addr++;
					}while(temp!=0x0d&&a<140&&addr<140);
					if( a<140 )
						ad_tp = a;
				   	else
					{
						a = ad_tp;
						rsp_buffer[a-1] = 'N';
						a++;
						break;
					}
				}
		 	}while(++page<8);
            CLRWDT();
			if( a==0 )
			{
                
				rsp_buffer[0] = 'N';
				rsp_buffer[1] = 'o';
				rsp_buffer[2] = ' ';
				rsp_buffer[3] = 'L';
				rsp_buffer[4] = 'o';
				rsp_buffer[5] = 'g';
				rsp_buffer[6] = 0x0d;
		  	}
		   	else rsp_buffer[a-1] = 0x0d;
		}else page=8;
        CREN1 = 0;
	  	soutdata(cmgs);
  		a=1;
	  	do{
			temp = sms_phone[a];
			//if( temp != '"' )
				out_sbuf(temp);
			a++;
		}while(temp!='"');
        CLRWDT();
		out_sbuf(CR);   // 0x0D
        out_sbuf(LF);     // 0x0A
        CREN1 = 1;
        
	  	// Wait for '>'
  		a=100;
	  	do{
    	 	T3CON = 0x71;
            TMR3H = 0xa0;   //50ms
            TMR3L = 0;
            do{
                if( RC1IF==1 )
                {        	
                    temp=RC1REG;
			  		if(temp=='>')
			    		a=1;
				}
	     	}while(TMR3IF==0);
            CLRWDT();
		 	TMR3IF=0;
	  	}while(--a!=0);
  		delay5ms(20);
        
	  	// Response data
		if( type=='X' )
			a = x_tp;
	   	else 
            a = 0;
        
		c = enc_cnt;
		x_vl = 0;
		do{
			b = rsp_buffer[a++];

            out_sbuf(b);
            if( rsp_buffer[a] == CR )
            {
                x_vl = 0x0d;
                break;
            }
		}while( (over_dp==0&&a<140)||(over_dp==1&&a<250) );
        
        CLRWDT();
		over_dp = 1;
		x_tp = a;
		delay5ms(60);
        
        // To write the message issue Ctrl-Z char (0x1A hex).
        // To exit without writing the message issue ESC char (0x1B hex).
#ifndef send_ok
			out_sbuf(ESC);
#else
			out_sbuf(CTRL_Z);
#endif
	   	//wait "OK"	<10sec>
  		a=200;		
 	 	do{
    	 	T3CON = 0x71;
            TMR3H = 0x40;   //50ms
            TMR3L = 0;
            do{
                if( RC1IF==1 )
                {        	
                    temp=RC1REG;
			  		if(temp=='K')
			    		a=1;
				}
                
                check_receive_overrun();
	     	}while(TMR3IF==0);
            CLRWDT();
		 	TMR3IF=0;
  		}while(--a!=0);
        TMR3ON = 0;
 	 	delay5ms(20);
		if( page<8 )
			delay5ms(200);
	}while( (type=='L'&&page<8)||(type=='X'&&x_vl!=0x0d) );
}		

//---------------------------------------------------
/*void send_pdu_sms(uint8_t type)
{
	uint8_t temp;
  	const uint8_t code cmgs[]="AT+CMGS=$";
  	uint8_t a,b;
  	IEN0=0;
  	soutdata(cmgs);
	out_sbuf('"');
	type = 1;
  	a=1;
  	do{
		temp = sms_phone[a];
		if( temp != '"' )
		{
			out_sbuf('0');
			out_sbuf('0');
			out_sbuf(hex_chr[temp>>4]);
			out_sbuf(hex_chr[temp&0x0f]);
		}else out_sbuf(temp);
		a++;
	}while(temp!='"');
	at_end();
  	//wait ">"
  	a=100;
  	do{
     	TMOD=0x21;
     	TH0=(65536-50000)/256;
     	TL0=(65536-50000)&256;
     	TF0=0;
     	TR0=1;
     	do{
        	if(RI==1)
        	{
          		RI=0;
		  		b=S0BUF;
		  		if(b=='>')
		    		a=1;
			}
     	}while(TF0==0);
	 	TF0=0;
  	}while(--a!=0);
  	delay5ms(20);
  	//reply data
	for( a=0;a<enc_cnt;a++ )
	{
		b = rsp_buffer[a];
		out_sbuf(hex_chr[b>>4]);
		out_sbuf(hex_chr[b&0x0f]);
	//	out_sbuf(b);
	}
	delay5ms(20);
	if( send_ok==0 )
		out_sbuf(0x1b);
 	else
		out_sbuf(0x1a);
   	//wait "OK"	<10sec>
  	a=200;		
  	do{
     	TMOD=0x21;
     	TH0=(65536-50000)/256;
     	TL0=(65536-50000)&256;
     	TF0=0;
     	TR0=1;
     	do{
        	if(RI==1)
        	{
          		RI=0;
		  		b=S0BUF;
		  		if(b=='K')
		    		a=1;
			}
     	}while(TF0==0);
	 	TF0=0;
  	}while(--a!=0);
  	delay5ms(20);
}*/

uint8_t respond_setting(uint8_t type, uint8_t off_set)
{
	uint8_t addr,temp,off_tp,page,cnt;
	uint16_t data_int;
    
	off_tp = off_set;
	page = EE_PAGE0;
	if( (type>=1 && type <= 5) || (type==10) || (type==11) || (type==35) || (type==36) )			//01~05,10,11,35,36
	{	
        page = EE_PAGE0;
		if( type==P_IP1 )
			addr = IP1_ADDR;
		else if( type==P_IP2 )
			addr = IP2_ADDR;
	  	else if( type==P_IP3 )
			addr = IP3_ADDR;
	   	else if( type==IP4_ADDR )
			addr = 0x90;
	 	else if( type==P_ACCESS_CODE )
			addr = ACCESS_CODE_ADDR;
		else if( type==P_UNIT_ACCNT )
			addr = UNIT_ACCT_ADDR;
        else if( type==P_LINE_CARD )
			addr = LINE_CARD_ADDR;
        else if( type==P_APN )
			addr = APN_ADDR;
	   	else if( type==P_OTA )
        {
            page = EE_PAGE1;
			addr = IP_OTA_ADDR;
        }
		cnt = 0;
		do{
			temp = read_ee(page, (uint8_t) (addr+cnt));
			if( temp=='#'|| temp==0x0c || temp == '\0')
				break;
		   	rsp_buffer[off_set++] = temp;
		}while( ++cnt<32 );
        CLRWDT();
		if( off_set==off_tp )			
            rsp_buffer[off_set++] = 'N';
		rsp_buffer[off_set++] = ',';
	}
    else if( (type == P_PROGRAM_ACK) || (type == P_TP_PIN) )		//06,14
	{
        page = EE_PAGE0;
        if( type == P_PROGRAM_ACK )
            addr = PROGRAM_ACK_ADDR;
        else 
            addr = TP_PIN_ADDR;
        
		temp = read_ee(page,addr);
        if( temp==0x00 )
            temp = '0';
        else temp = '1';
        rsp_buffer[off_set++] = temp;
		rsp_buffer[off_set++] = ',';	
	}
    else if (type == P_ZONE1)      //||(type==0x13) )		//12,13
	{
        page = EE_PAGE0;
        if( type == P_ZONE1 )
            addr = ZONE1_ADDR;
      //  else addr = 0xBA;   
        temp = read_ee(page,addr);     
     //   if( temp >=10 )
      //      rsp_buffer[off_set++] = ((temp/10)+0x30);
        rsp_buffer[off_set++] = temp;//((temp%10)+0x30);
		rsp_buffer[off_set++] = ',';	
	}
    else if ( (type == P_PORT1) || (type == P_PORT2) || (type == P_PORT3) || (type == P_PORT4) || (type == P_PORT_OTA) )	//31,32,33,34,37
	{
		if ( type == P_PORT1 )
            strncpy((char *)&rsp_buffer[off_set], (const char *)page0_eeprom.map.PORT1, 4);
		else if ( type == P_PORT2 )
            strncpy((char *)&rsp_buffer[off_set], (const char *)page0_eeprom.map.PORT2, 4);
		else if ( type == P_PORT3 )
            strncpy((char *)&rsp_buffer[off_set], (const char *)page0_eeprom.map.PORT3, 4);
		else if ( type == P_PORT4 )
            strncpy((char *)&rsp_buffer[off_set], (const char *)page0_eeprom.map.PORT4, 4);        
		else if ( type == P_PORT_OTA )
            strncpy((char *)&rsp_buffer[off_set], (const char *)page1_eeprom.map.PORT_OTA, 4);         
        
        off_set += 4;
        
		rsp_buffer[off_set++] = ',';
	}
    else if( (type == P_TEST_FREQ) || (type == P_SERVER_ACK_TIME) || (type == P_SMS_WAIT_TIME) || (type == P_CYCLE) || (type == P_RETRY) ) //07,08,09,15,16
	{	
        page = 0x00;
		if( type == P_TEST_FREQ )
			addr = TESTING_FREQ_ADDR;
	   	else if( type == P_SERVER_ACK_TIME ) 
            addr = SERVER_ACK_TIME_ADDR;
        else if( type == P_SMS_WAIT_TIME ) 
            addr = SMS_WAIT_TIME_ADDR;        
        else if( type == P_CYCLE ) 
            addr = CYCLE_ADDR; 
        else //if( type==0x16 ) 
            addr = RETRY_TIMES_ADDR; 
        
		off_tp = off_set;
		temp = read_ee(page,addr);
		addr = temp/100;
		if( addr!=0 )
			rsp_buffer[off_set++] = (uint8_t) (addr+0x30);
	  	temp %= 100;
		addr = temp/10;
		if( addr!=0 || (off_tp!=off_set) )
			rsp_buffer[off_set++] = (uint8_t) (addr+0x30);
	  	temp %= 10;
		rsp_buffer[off_set++] = (uint8_t) (temp+0x30);
		rsp_buffer[off_set++] = ',';
	}else
	{
		rsp_buffer[off_set++] = 'N';
		rsp_buffer[off_set++] = ',';	
	}
	return(off_set);
}

