//
// sms.c
//

#include "io.h"
#include "eeprom.h"
#include "sms.h"
#include "eeprom_setup.h"
#include "modem.h"
#include "timer.h"

uint8_t sms_p,sms_buffer[160]; 
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

void get_access_code()
{
    uint8_t temp = 0;
            
	// get access code from eeprom
    do{
        access_code[temp] = read_ee(0x00, (uint8_t)(0xC0+temp));
    }while(++temp<7);       
}

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
    get_access_code();
	
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
							time[t_p] = time[t_p+1U];
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
					if(temp==0x0d)
					{
						if( (sms_buffer[0]==access_code[0])&&(sms_buffer[1]==access_code[1])&&(sms_buffer[2]==access_code[2])&&(sms_buffer[3]==access_code[3]) )							
							NOP();
						else										
							break;
						
			  			sms_buffer[sms_p-1U] = 0xcc;
						
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
/*	uint8_t code ARMED[]="ARMED$";
	uint8_t code DISARMED[]="DISARMED$";
	uint8_t code F_ARMED[]="FAIL TO ARMED$";
	uint8_t code F_DISARMED[]="FAIL TO DISARMED$";*/

	uint8_t temp,temp1,temp2,respond,cnt;
	uint8_t addr_tp,off_set,off_tp;
	uint16_t count,data_int;

	addr_tp = x_cnt; 					// record first address

	//----- function code
	respond = 0x00;
	if( sms_buffer[x_cnt]=='2' )		// retrieve system config
	{
		x_cnt++;
        // 20#
		if( (sms_buffer[x_cnt]=='0') && (sms_buffer[x_cnt+1U]=='#') )
		{
			x_cnt += 2;
			respond = 'X';
			// 1111##20sensor# - query sensor info
			if( (toupper(sms_buffer[x_cnt]) == 'S') && (toupper(sms_buffer[x_cnt+1U]) == 'E') && 
				(toupper(sms_buffer[x_cnt+2U]) == 'N') && (toupper(sms_buffer[x_cnt+3U]) == 'S') && 
				(toupper(sms_buffer[x_cnt+4U]) == 'O') && (toupper(sms_buffer[x_cnt+5U]) == 'R') &&  
                (sms_buffer[x_cnt+6U]=='#')   )  //SEMSOR
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
                        temp = read_ee(1,(temp1*8U)+temp2);
                        if( temp2==0&&temp==0 )
                            break;
                        rsp_buffer[off_set++] = temp;
                    }                    
                    rsp_buffer[off_set++] = ',';
                }     
                rsp_buffer[off_set-1U] = 0x0d;
            }/*else if(  (sms_buffer[x_cnt]=='S')||(sms_buffer[x_cnt]=='s')&&(sms_buffer[x_cnt+1]=='M')||(sms_buffer[x_cnt+1]=='m')&&(sms_buffer[x_cnt+2]=='O')||(sms_buffer[x_cnt+2]=='o')&&
                       (sms_buffer[x_cnt+3]=='K')||(sms_buffer[x_cnt+3]=='k')&&(sms_buffer[x_cnt+4]=='E')||(sms_buffer[x_cnt+4]=='e')&&(sms_buffer[x_cnt+5]=='#')   )  //SMOKE
            {        
                off_set = 0;
                for( temp1=16;temp1<28;temp++ )
                {
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
            }*/
			// 1111#20#all# - query all setting info
			//else if( (sms_buffer[x_cnt]=='A')||(sms_buffer[x_cnt]=='a') )
			else if( toupper(sms_buffer[x_cnt]) == 'A') 
			{
				x_cnt++;
				if( toupper(sms_buffer[x_cnt]) == 'L' )
				{
					x_cnt++;
					if( (toupper(sms_buffer[x_cnt]) == 'L') && (sms_buffer[x_cnt+1U] == '#') )
					{
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
							temp1 = temp/10U;
							temp2 = temp%10U;
							temp1 = (uint8_t)((temp1<<4U)+temp2);
							if( (temp1>=0x1&&temp1<=0x12)||(temp1>=0x14&&temp1<=0x16)||((temp1>=0x31)&&(temp1<=0x37)) )
								off_set = respond_setting(temp1,off_set);
						}while(++temp<0x38);						
						rsp_buffer[off_set-1U]=0x0d;
				 	}else respond='E';
				}else respond='E';
			}else
			{
				// Otherwise ERROR
				off_set = 0;
				do{
					temp = sms_buffer[x_cnt++];
					temp1 = sms_buffer[x_cnt++];
					if( !is_digit(temp) || !is_digit(temp1) )
                    {
                        rsp_buffer[off_set++] = 'E';
                        rsp_buffer[off_set++] = 'r';
                        rsp_buffer[off_set++] = 'r';
                        rsp_buffer[off_set++] = 'o';
                        rsp_buffer[off_set++] = 'r';
                        rsp_buffer[off_set++] = ' ';
						break;
                    }
					temp = (uint8_t) (((temp&0x0f)<<4U) + (temp1&0x0f));
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
					rsp_buffer[off_set-1U]=0x0d;
				respond = 'X';
			}
	  	}else x_cnt--;
	}
    
	// call functaion_all 
	if( respond!='R'&&respond!='E'&&respond!='L'&&respond!='X'&&respond!='W' )			// setting function
	{
		do{
			key_p = 0;
			respond = 'E';
			do{
				temp = sms_buffer[x_cnt++];
				key[key_p++] = temp;
				if( temp==','||temp==0xcc )
				{
					key[key_p-1U] = '#';
					if( key_p<5 )
						respond = 'E';
					else if(key[2]=='#'||key[3]=='#' )
					{					
						respond = 'K';
					}else respond = 'E';
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
				respond = function_code();
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
			rsp_buffer[temp1++] = 'E';
			rsp_buffer[temp1++] = 'r';
			rsp_buffer[temp1++] = 'r';
			rsp_buffer[temp1++] = 'o';
			rsp_buffer[temp1++] = 'r';
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
						rsp_buffer[a-1U] = '-';
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
						rsp_buffer[a-1U] = 'N';
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
		   	else rsp_buffer[a-1U] = 0x0d;
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

uint8_t respond_setting(uint8_t type,uint8_t off_set)
{
	uint8_t addr,temp,off_tp,page,cnt;
	uint16_t data_int;
    
	off_tp = off_set;
	page = 0;
	if( (type>=0x01&&type<=0x05)||(type==0x10)||(type==0x11)||(type==0x35)||(type==0x36) )			//01~05,10,11,35,36
	{	
        page = 0x00;
		if( type==0x01 )
			addr = 0x30;
		else if( type==0x02 )
			addr = 0x50;
	  	else if( type==0x03 )
			addr = 0x70;
	   	else if( type==0x04 )
			addr = 0x90;
	 	else if( type==0x05 )
			addr = 0xC0;
		else if( type==0x10 )
			addr = 0xCA;
        else if( type==0x11 )
			addr = 0xD0;
        else if( type==0x35 )
			addr = 0x10;
	   	else if( type==0x36 )
        {
            page = 1;
			addr = 0xD0;
        }
		cnt = 0;
		do{
			temp = read_ee(page, (uint8_t) (addr+cnt));
			if( temp=='#'||temp==0x0c )
				break;
		   	rsp_buffer[off_set++] = temp;
		}while( ++cnt<32 );
        CLRWDT();
		if( off_set==off_tp )
			rsp_buffer[off_set++] = 'N';
		rsp_buffer[off_set++] = ',';
	}else if( (type==0x06)||(type==0x14) )		//06,14
	{
        page = 0x00;
        if( type==0x06 )
            addr = 0xC7;
        else addr = 0xBB;
		temp = read_ee(page,addr);
        if( temp==0x00 )
            temp = '0';
        else temp = '1';
        rsp_buffer[off_set++] = temp;
		rsp_buffer[off_set++] = ',';	
	}else if( type==0x12 )      //||(type==0x13) )		//12,13
	{
        page = 0x00;
        if( type==0x12 )
            addr = 0xB9;
      //  else addr = 0xBA;   
        temp = read_ee(page,addr);     
     //   if( temp >=10 )
      //      rsp_buffer[off_set++] = ((temp/10)+0x30);
        rsp_buffer[off_set++] = temp;//((temp%10)+0x30);
		rsp_buffer[off_set++] = ',';	
	}else if( (type==0x31)||(type==0x32)||(type==0x33)||(type==0x34)||(type==0x37) )	//31,32,33,34,37
	{
		if( type==0x31 )
		{
			data_int = (uint8_t) (read_ee(0x00,0xB0)<<8U);
			data_int += read_ee(0x00,0xB1);
		}else if( type==0x32)
		{
			data_int = (uint8_t) (read_ee(0x00,0xB2)<<8U);
			data_int += read_ee(0x00,0xB3);
		}else if( type==0x33)
		{
			data_int = (uint8_t) (read_ee(0x00,0xB4)<<8U);
			data_int += read_ee(0x00,0xB5);
		}else if( type==0x34 )
		{
			data_int = (uint8_t) (read_ee(0x00,0xB6)<<8U);
			data_int += read_ee(0x00,0xB7);
		}else if( type==0x37 )
        {
            data_int = (uint8_t) (read_ee(0x01,0xF0)<<8U);
			data_int += read_ee(0x01,0xF1);
        }else data_int = 0;
		cnt = 0;
		temp = data_int/10000U;
		if( temp!=0 )
		{	
			cnt = 1;
			rsp_buffer[off_set++] = (uint8_t) (temp+0x30);
		}
		data_int %= 10000;
		temp = data_int/1000;
		if( temp!=0||cnt==1 )
		{
			cnt = 1;
			rsp_buffer[off_set++] = (uint8_t) (temp+0x30);
		}
		data_int %= 1000;
		temp = data_int/100;
		if( temp!=0||cnt==1 )
		{
			cnt = 1;
			rsp_buffer[off_set++] = (uint8_t) (temp+0x30);
		}
		data_int %= 100;
		temp = data_int/10;
		if( temp!=0||cnt==1 )
		{
			cnt = 1;
			rsp_buffer[off_set++] = (uint8_t) (temp+0x30);
		}
		temp = data_int%10;
		rsp_buffer[off_set++] = (uint8_t) (temp+0x30);
		rsp_buffer[off_set++] = ',';
	}else if( (type==0x07)||(type==0x08)||(type==0x09)||(type==0x15)||(type==0x16) ) //07,08,09,15,16
	{	
        page = 0x00;
		if( type==0x07 )
			addr = 0xC8;
	   	else if( type==0x08 ) 
            addr = 0xC9;
        else if( type==0x09 ) 
            addr = 0xB8;        
        else if( type==0x15 ) 
            addr = 0xBC; 
        else //if( type==0x16 ) 
            addr = 0xBD; 
		off_tp = off_set;
		temp = read_ee(page,addr);
		addr = temp/100U;
		if( addr!=0 )
			rsp_buffer[off_set++] = (uint8_t) (addr+0x30);
	  	temp %= 100;
		addr = temp/10U;
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

