//
// OTA.c
//

#include "OTA.h"
#include "io.h"
#include "eeprom.h"
#include "emc_library.h"
#include "telit.h"
#include "timer.h"
#include "modem.h"
#include "sms.h"

extern uint8_t rsp_buffer[250];

// OTA operation -
/*
1. BOOT_SEL line is idle HIGH (1). Your proposed solution could work. So:
    a. PIC16 gets final data from server (connection still open);
    b. PIC16 strobes BOOT_SEL = 0 for 500us, then BOOT_SEL = 1;
    c. PIC18 detects this BOOT_SEL change and closes the port, closes connection, turns off MD_PWR;
    d. PIC18 strobes BOOT_SEL = 0 for 500us, then BOOT_SEL = 1;
        [DL] WHY DO WE NEED THIS???
    e. Then OTA controller takes over and programs the PIC18.

2. PIC18 won't have to do anything after step (d) above.
 */ 

md_resp_t wait_ota_status(uint16_t count)
{
  	uint8_t temp;
    uint8_t buffer[20],buffer_p;
    md_resp_t resp = OTA_UNKNOW;
    
    CREN1 = 0;
    RC1IE = 0;
    CREN1 = 1;
    buffer_p = 0;
    CLRWDT();
    
    // BOOT_SEL input from PIC16
    set_boot_sel_input();
    
  	do{
        reload_timer3_50ms();
     	do{
            // b. PIC16 strobes BOOT_SEL = 0 for 500us, then BOOT_SEL = 1;
            if (BOOT_SEL_I == 0)
            {
                soutdata((uint8_t *) "OTA_BOOT_SEL\r\n$");    
                
                TL_connection_close();
                delayseconds(1);
                TL_internet_close();

                poweroff_modem();
                
                return OTA_BOOT_SEL;
            }
            
        	if( RC1IF==1)
        	{	   
		 	 	temp=RC1REG;
                buffer[buffer_p] = temp;

                if( ++buffer_p>=20 )
                    buffer_p = 19;
                if( temp == LF )
                {
                    // CONNECT response from modem due to online mode.
                    if (strncmp((const char *)buffer, (const char *)"CONNECT", 7) == 0)
                        resp = OTA_CONNECT;
                    // RED from server
                    if (strncmp((const char *)buffer, (const char *)"RED", 3) == 0)
                        resp = OTA_RED;
                    // NO CARRIER
                    else if (strncmp((const char *)buffer, (const char *)"NO CARRIER", 10) == 0)
                        resp = MD_NO_CARRIER;  
                    // ERROR
                    else if (strncmp((const char *)buffer, (const char *)"ERROR", 5) == 0)
                        resp = MD_ERROR;
                    // OFA
                    else if (strncmp((const char *)buffer, (const char *)"OFA", 3) == 0)
                        resp = OTA_OFA;
                    
                    if(resp != OTA_UNKNOW)
                    {
                        RC1IE = 1;
                        CREN1 = 0;
                        //delay5ms(60);
                        return(resp);		    		
                    }
                    buffer_p = 0;
                }
        	}
            //check_receive_overrun();
     	} while(TMR3IF==0);
        
        CLRWDT();
        TMR3IF = 0;
        //LED = ~LED;
  	}while(--count!=0);
    
  	TMR3ON = 0;
    RC1IE = 1;
  	return OTA_UNKNOW;
}

uint8_t wait_connect_respond(uint16_t count)
{
  	uint8_t temp;
    uint8_t buffer[20],buffer_p;
    uint8_t bad[3];
    
    CREN1 = 0;
    RC1IE = 0;
    CREN1 = 1;
    buffer_p = 0;
    CLRWDT();
    
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
                bad[0] = bad[1];
                bad[1] = bad[2];
                bad[2] = temp;
                if( ++buffer_p>=20 )
                    buffer_p = 19;
                if( temp==0x0a )
                {
                    // CONNECT response from modem due to online mode.
                    if (strncmp((const char *)buffer, (const char *)"CONNECT", 7) == 0)
                        temp = 'C';
                    // RED from server
                    else if (strncmp((const char *)buffer, (const char *)"RED", 3) == 0)
                        temp = 'R';
                    // NO CARRIER
                    else if (strncmp((const char *)buffer, (const char *)"NO CARRIER", 10) == 0)
                        temp = 'E';  
                    // ERROR
                    else if (strncmp((const char *)buffer, (const char *)"ERROR", 5) == 0)
                    //else if( buffer[0]=='E'&&buffer[1]=='R'&&buffer[2]=='R'&&buffer[3]=='O'&&buffer[4]=='R' )
                        temp = 'E';  
                    if(temp=='C'|| temp=='R'||temp=='E')
                    {
                        RC1IE = 1;
                        CREN1 = 0;
                        //delay5ms(60);
                        return(temp);		    		
                    }
                    buffer_p = 0;
                }
        	}
            check_receive_overrun();
     	}while(TMR3IF==0);
        CLRWDT();
        TMR3IF = 0;
        //LED = ~LED;
  	}while(--count!=0);
  	TMR3ON = 0;
    RC1IE = 1;
  	return('N');
}


//uint8_t CRC_16(uint8_t tp_cnt)
//{
//
//    uint8_t cnt,loop; 
//    uint16_t reg_crc=0xFFFF; 
//    for( cnt=0;cnt<tp_cnt;cnt++) 
//    { 
//        reg_crc ^= rsp_buffer[cnt]; 
//        for(loop=0;loop<8;loop++) 
//        { 
//            if(reg_crc & 0x01)
//                reg_crc=(reg_crc>>1) ^ 0xA001; 
//            else 
//                reg_crc=reg_crc >>1; 
//        } 
//    } 
//    rsp_buffer[tp_cnt++] = reg_crc&0x00ff;
//    rsp_buffer[tp_cnt++] = reg_crc>>8;
//    return tp_cnt; 
//}

//uint8_t decryption_data(uint8_t buffer_p,uint8_t *buffer )
//{
//   	uint8_t cnt,temp1,temp2,random,swap;
//    /*---decrypt code---*/
//	random = buffer[0];
//	random = ((random>>4)+(random&0x0f))%16;
//	//random = 0;
//	for( cnt=1;cnt<(buffer_p-1);cnt++ )
//	{
//		temp2 = buffer[cnt];
//		temp1 = encryption_code[random];	
//		temp2 ^= temp1;					//xor		
//		swap = temp2 <<6;				//left shift 2 bit
//		temp2 >>= 2;
//		temp2 += swap;
//		swap = temp2<<4;					//4bit swap
//		temp2 >>=4;
//		temp2 += swap;	
//		temp2 ^= temp1;					//xor	
//		buffer[cnt-1] = temp2;
//		if( ++random>=16 )
//			random = 0;			
//	}
//	return(cnt-1);       
//}

//uint8_t encryption_data(uint8_t tp_cnt )
//{
//    uint8_t temp,cnt1,cnt2,temp1,random,swap,type,page;
//	uint8_t check_sum,random_log;
//	/*------------random number---------------*/
//	random = (rand()>>8)^rand()^random_rx;
//	random_log = random;
//	check_sum = random;
//	random = ((random>>4)+(random&0x0f))%16;	
//	/*------------encryption ---------------*/
//    cnt1 = 0;
//	do{
//		temp = rsp_buffer[cnt1];
//		/*------------encryption ---------------*/
//		temp1 = encryption_code[random];		
//		temp ^= temp1;					//xor
//		swap = temp<<4;					//4bit swap
//		temp >>=4;
//		temp += swap;	
//		swap = temp >>6;				//left shift 2 bit
//		temp <<= 2;
//		temp += swap;
//		temp ^= temp1;					//xor
//		rsp_buffer[cnt1] = temp;
//		if( ++random>=16 )
//			random = 0;
//		/*-------------encryption --------------*/
//		check_sum ^= temp;	  	
//	}while(++cnt1<tp_cnt);
//    CLRWDT();
//    for( cnt1=tp_cnt;cnt1--;cnt1!=0 )
//    {
//        rsp_buffer[cnt1] = rsp_buffer[cnt1-1];        
//    }
//    rsp_buffer[0] = random_log;
//    tp_cnt++;
//	rsp_buffer[tp_cnt++] = check_sum;
//    return(tp_cnt);
//}

uint8_t OTA_send_data_to_server(void)
{
    const uint8_t send[]="AT#SSENDEXT=1,$";
    uint8_t buffer_p;
	uint8_t cnt,temp,as;
    uint8_t tp_cnt;
    
    // NEJ2040!< 
    tp_cnt = 0;
    rsp_buffer[tp_cnt++] = 'N';
    rsp_buffer[tp_cnt++] = 'E';
    rsp_buffer[tp_cnt++] = 'J';
    rsp_buffer[tp_cnt++] = VERSION[0];
    rsp_buffer[tp_cnt++] = VERSION[1];
    rsp_buffer[tp_cnt++] = VERSION[2];
    rsp_buffer[tp_cnt++] = VERSION[3];
    // CRC
    //tp_cnt = CRC_16(tp_cnt);
    //encryption
    //tp_cnt = encryption_data(tp_cnt);
    
    CREN1 = 0;
    soutdata(send);
    cnt = tp_cnt;
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

uint8_t OTA_receive_data_from_server(void)
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
		  		if( (temp==0x0a) && (buffer[buffer_p-2]==0x0d) )	//Network opened
				{
                    //if( buffer[0]=='E'&&buffer[1]=='R'&&buffer[2]=='R'&&buffer[3]=='O'&&buffer[4]=='R' )   
                    if (strncmp((const char *)buffer, (const char *)"ERROR", 5) == 0)
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
                        //buffer_p = decryption_data(buffer_p,&buffer[0]);
                        if( buffer[0]=='N'&&buffer[1]=='E'&&buffer[2]=='J'&&buffer[3]==VERSION[0]&&buffer[4]==VERSION[1]&&buffer[5]==VERSION[2]&&buffer[6]==VERSION[3] )
                        {
                            if( buffer[9]==0x0d )
                                temp = 'K';
                            else if( buffer[7]=='R'&&buffer[8]=='F'&&buffer[9]=='Q' )                            
                                temp = 'U';                            
                            else temp = 'R';
                        }else temp = 'E';
                        return(temp);
                    }
                    buffer_p = 0;
			 	}
        	}
            // Receive Overrun Error - clearing the CREN bit to clear the error.
            if (OERR1 == 1)
            {
                CREN1 = 0;
                NOP();
                CREN1 = 1;
            }
     	}while(TMR3IF==0);
        CLRWDT();
        TMR3IF = 0;
  	}while(--cnt!=0);
    TMR3IF = 0;
  	TMR3ON = 0;
    return('E');
}

//---------------------------------------------------
uint8_t OTA_connection_open(uint8_t type)   //0: command mode 1:online mode
{
	const uint8_t netconnect[]="AT#SD=1,0,$";
    const uint8_t net_2[]="\",0,0,1\r\n$"; 
    const uint8_t net_3[]="\",0,0,0\r\n$";
    //uint8_t const netconnect[]="AT#SD=1,0,2020,\"211.22.241.58\",0,0,1\r\n$";
    uint8_t buffer_p,buffer[32];
	uint8_t cnt,temp;
	uint16_t port;
    uint8_t ip_sel,page,addr;
    
    // IP_OTA_ADDR
    //temp = read_ee(0x01,0xd0);
    temp = get_ota_ip_addr();
    if( temp=='#')
    {
        // IP1_ADDR
        temp = get_ip1_addr();        
        ip_sel = 1;
    }else ip_sel = 0;
    if( temp=='#' )
        return('E');
    CREN1 = 0;
    // AT#SD=1,0,2020,"198.17.112.128",0,0,1
	soutdata(netconnect);
    //------ port ------
    if( ip_sel == 0)
    {
        // PORT_OTA_ADDR
        page = 1;
        addr = 0xf0;
    }else 
    {
        // PORT1_ADDR
        page = 0;
        addr = 0xB0;
    }

	port = (uint16_t) (read_ee(page,addr)<<8);
	port += read_ee(page,addr+1);
	cnt = 0;
	temp = port/10000;
	if( temp!=0 )
	{	
		cnt = 1;
		out_sbuf( (uint8_t) (temp+0x30) );
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
    if( ip_sel == 0)
    {
        // IP_OTA_ADDR
        page = 1;
        addr = 0xd0;
    }else 
    {
        // IP1_ADDR
        page = 0;
        addr = 0x30;
    }
	do{
		temp = read_ee(page,addr);
		if( temp!='#' )
			out_sbuf(temp);
		addr++;
	}while( temp!='#' );     
    CLRWDT();
    if( type==0 )        
        soutdata(net_2);
    else 
        soutdata(net_3);
	//Connect ok / fail
	buffer_p = 0;
	cnt = 200;
    RCIE = 0;
	CREN1 = 1;
    if( type==0 )
        temp = wait_ok_respond(500);
    else temp = wait_connect_respond(1500);
    return(temp);
}

uint8_t check_OTA(void)
{
    uint8_t rsp,cnt,cnt1;
    cnt = 3;
    do{
        // Connecting to APN
        rsp = TL_internet_init();
        if( rsp=='K' )
        {
            // Connect to OTA server IP - AT#SD command mode
            rsp = OTA_connection_open(0x00);
            if( rsp=='K' )
            {
                // Sending version# to server - NEJ2040
                rsp = OTA_send_data_to_server();
                cnt1 = 50;
                do{
                    // Receiving from server - NEJ2040RFQ
                    rsp = OTA_receive_data_from_server();     
                    delayseconds(1);
                }while(--cnt1!=0&&rsp=='E');
                //online mode
                // Update FW version
                if( rsp=='U' )
                {
                    delayseconds(1);
                    TL_connection_close();
                    delayseconds(1);
                    
                    // AT#SD=1,0,2021,"72.197.171.234",0,0,0 - AT#SD online mode
                    // Waiting CONNECT from modem
                    rsp = OTA_connection_open(0x01);
                    // TODO: AT#SD command mode
                    //rsp = OTA_connection_open(0x00);
                    delayseconds(1);
                    //delay5ms(50);
                    if( rsp=='C' )
                    //if( rsp=='K' )
                    {
                        // send to server - RFQ;
                        //soutdata((uint8_t *) "RFQ$");
                        out_sbuf('R');
                        out_sbuf('F');
                        out_sbuf('Q');
                        // Wait for RED from server
                        rsp = wait_connect_respond(1500);
                        // ESC
//                         soutdata((uinit8_t *) "+++$");
//                        delayseconds(3);
                        if( rsp=='E' )
                        {                        
                            TL_connection_close();
                            delayseconds(1);
                            TL_internet_close();
                            
                            poweroff_modem();
                            
                            return('F');
                        }
                        else
                        {
                            return 'K';
                        }
                    }
                }else if( rsp=='K' )
                {
                    TL_connection_close();
                    delayseconds(1);
                    TL_internet_close();
                    return('K');
                }else if( rsp=='R' )
                {                    
                    TL_connection_close();
                    delayseconds(1);
                    TL_internet_close();
                    return('E');
                }
            }
            TL_connection_close();
        }
        delayseconds(1);
        TL_internet_close();
        delayseconds(1);
    }while(--cnt!=0);

    return('E');
}

uint8_t get_ota_ip_addr()
{
    return read_ee(EE_PAGE1, IP_OTA_ADDR);
}

uint8_t get_ip1_addr()
{
    return read_ee(EE_PAGE0, IP1_ADDR);
}