//
// emc_library.c
//

#include "emc_library.h"
#include "io.h"
#include "eeprom.h"
#include "telit.h"
#include "timer.h"
#include "led.h"

void update_mmcnt(void)
{
	uint8_t mm_cnt;
    
	mm_cnt = page0_eeprom.map.MM_COUNT;

	if( ++mm_cnt>=END_MM_COUNT )
		mm_cnt = START_MM_COUNT;
    
    page0_eeprom.map.MM_COUNT = mm_cnt;
	write_ee(EE_PAGE0, MM_COUNT_ADDR, mm_cnt);
}

uint8_t save_stack_header(void)
{
	uint8_t tp_cnt,temp,cnt1,cnt2;

	tp_cnt = 0;
	do{
		if ( stack_buffer[tp_cnt].map.state == 0x00 )
		{
			stack_buffer[tp_cnt].map.state = 'P';
			stack_buffer[tp_cnt].map.dial_type = 1;				//dial type
            
			/* new add 12/09 */
            // TODO: What's this for??? Page 1 0x20 is device ID location. WHY???
			if( read_ee(0x01, 0x20)==0x0C )				//retry count if line2 empty,line1 retry <5>
				temp += RETRY_NUMBER1;	
			else temp += RETRY_NUMBER2;	
            
			stack_buffer[tp_cnt].map.retry_count = temp;

            // TODO: What's EEPROM 0x59 for??? There is NO 0x59 defined.
			temp = read_ee(EE_PAGE0, 0x59);
            
			stack_buffer[tp_cnt].map.ack_time = temp;			 	//ack time
			stack_buffer[tp_cnt].map.ack_count = temp;				//ack count
            stack_buffer[tp_cnt].map.reboot_time[0] = 3;                // reboot times
            stack_buffer[tp_cnt].map.reboot_time[1] = 3;
			
			update_mmcnt();	
			return(tp_cnt);
		}
	} while (++tp_cnt < BUFFER_STACK);
    
    CLRWDT();
	return(tp_cnt);
}

uint8_t stack_data_header(uint8_t tp_cnt,uint8_t mm_cnt, uint8_t cnt)
{
    // This function store data into stack_buffer[tp_cnt].
    //                             0    1    2    3    4    5    6    7        20   21    22   23   24   25   26   27   28   29   30 
    //  Ex: stack_buffer[0][] = 0x50 0x01 0xF2 0xFF 0xFF 0x03 0x03 0x00 .... 0x30 0x30 0x037 0x35 0x34 0x38 0x2C 0x34 0x30 0x30 0x37
    //                             P                                                       (7548)                  (4007)
	uint8_t 		addr, temp;
	const uint8_t 	hex[16]="0123456789ABCDEF";
    
	//temp = (mm_cnt/10)+0x30;			//MM
	temp = hex[(uint8_t)(mm_cnt>>4)];
	stack_buffer[tp_cnt].map.mm_count[0] = temp;
	//temp = (mm_cnt%10)+0x30;
	temp = hex[((uint8_t) (mm_cnt&0x0f))];
	stack_buffer[tp_cnt].map.mm_count[1] = temp;
    
    // Read Line Card (ex. 7548) from EEPROM one byte each time and store in stack_buffer
    //  Max 6 bytes (account no + #)
//	addr = LINE_CARD_ADDR;	
//	do {
//		temp = read_ee(EE_PAGE0, addr);
//		
//		if( temp!='#'&& temp!=0x0c)
//			stack_buffer[tp_cnt][cnt++] = temp;
//		else 
//			temp='#';
//	} while( (++addr < 0xD5) && (temp != '#') );
    
    // TODO: use 4 as the line card length. Need figure out if range 4-8
    strncpy((char *)stack_buffer[tp_cnt].map.line_card, (const char *)page0_eeprom.map.LINE_CARD, 4);
	
	stack_buffer[tp_cnt].map.comma = ',';
    
    // Read Unit Account (ex. 4007) from EEPROM one byte each time and store in stack_buffer
    //  Max 5 bytes (line card + #)
//	addr = UNIT_ACCT_ADDR;
//	do { 	
//		temp = read_ee(EE_PAGE0, addr);		
//		if( temp!='#' )
//			stack_buffer[tp_cnt][cnt++] = temp;
//	} while ( (++addr < 0xD0) && (temp != '#') );
    
    // TODO: use 4 as the unit account length. Need figure out if range 4-8
    strncpy((char *)stack_buffer[tp_cnt].map.unit_account, (const char *)page0_eeprom.map.UNIT_ACCNT, 4);
    
    CLRWDT();
	return(cnt);
}

//------------------------------//
void move_stack_buffer(void)
{
	uint8_t cnt,temp,cnt2;
    
    // TODO: What's this for??? - temporary use union data to swap.
    //      Swap stack_buffer[0] - [1]/ [2] - [3]/ 3-4/ 5-6???
    for( cnt = 0; cnt < (BUFFER_STACK-1); cnt++ )
    {
        cnt2 = 0;
        
        while (++cnt2 < LOG_MAX_T)
        {
            temp = stack_buffer[cnt+1].data[cnt2];
            stack_buffer[cnt+1].data[cnt2] = 0;
            stack_buffer[cnt].data[cnt2] = temp;
        }
        
        CLRWDT();
    }                
}

//------------------------------//
void load_emc_number(void)
{
	uint8_t temp,stack_buf_index,resp_buf_index,temp1,random,swap;
	uint8_t check_sum;
    
	// Encrypt or copy stack_buffer_data[20~159] to rsp_buffer. stack_buffer_data[0-20] NOT USE.
	stack_buf_index = 20;
	resp_buf_index = 0;
	
    ////////////// random number //////////////
	if ( page0_eeprom.map.ENCRYPTION == 1 )
	{
		random = (uint8_t) ((rand() >> 8) ^ rand() ^ random_rx);
        // TODO: The first byte is random from here. In this case is 0. How can server know???
		rsp_buffer[resp_buf_index++] = random;
		check_sum = random;
		random = (uint8_t)(((random>>4)+(random&0x0f)))%16;
	}

    ////////////// encryption //////////////
    // Encrypt [20] to end of array (CR).
	while (resp_buf_index < (LOG_MAX_T-20))
    {
		temp = stack_buffer[0].data[stack_buf_index++];
		if( page0_eeprom.map.ENCRYPTION == 1 )
		{
			if(temp == CR)
				break;
            
			/*------------encryption ---------------*/
            encrypt_byte(&temp, &random);
                        
			rsp_buffer[resp_buf_index++] = temp;
			if( ++random >= 16 )
				random = 0;
			/*-------------encryption --------------*/
			check_sum ^= temp;
	  	}
        else
		{
            // Just copy, no encryption
			rsp_buffer[resp_buf_index++] = temp;
			if( temp == CR )
				break;
		}
	}
    
    CLRWDT();
    // Check sum at the end for encrypt data.
	if( page0_eeprom.map.ENCRYPTION == 1 )		
		rsp_buffer[resp_buf_index++] = check_sum;
	
    // This is the length of resp_buffer
	enc_cnt = resp_buf_index;
}

void encrypt_byte(uint8_t *temp, uint8_t *random)
{
    uint8_t temp1,swap;
        
    temp1 = encryption_code[*random];		
    *temp ^= temp1;					//xor
    swap = (uint8_t) (*temp<<4);					//4bit swap
    *temp >>= 4;
    *temp += swap;	
    swap = (uint8_t) (*temp >>6);				//left shift 2 bit
    *temp <<= 2;
    *temp += swap;
    *temp ^= temp1;					//xor
}

bool alarm_out(uint8_t type, uint8_t zone_ext)
{
	uint8_t tp_cnt,mm_cnt,cnt,temp;
    uint8_t *sp;

    if (type > NUM_OF_STR)
        return 0;
    else
        sp = alarm_string[type];
	
    // Looking for the buffer first char is 'P'???
    for( cnt=0;cnt<BUFFER_STACK;cnt++ )
    {
        if( stack_buffer[cnt].map.state == 'P')
            break;
    }
    
    if( cnt != BUFFER_STACK )
    {    
        // TODO: What's this for??? 20-220???
        for( tp_cnt=20; tp_cnt<LOG_MAX_T; tp_cnt++)
        {
            if( stack_buffer[cnt].data[tp_cnt]==0x0d )
            {
                if( tp_cnt<160 )
                {
                    mm_cnt = 2;
                    do{
                        temp = sp[mm_cnt++];
                        stack_buffer[cnt].data[tp_cnt++] = temp;
                    }while(temp!='$');
                    stack_buffer[cnt].data[tp_cnt-1] = read_ee(EE_PAGE0, 0xB9);        
                    stack_buffer[cnt].data[tp_cnt++] = (uint8_t) ((zone_ext/10)+0x30);
                    stack_buffer[cnt].data[tp_cnt++] = (uint8_t) ((zone_ext%10)+0x30);
                    stack_buffer[cnt].data[tp_cnt++] = CR;
                    return(1);
                }
            }
        }       
    }
    
    // TODO: What's this for???
    mm_cnt = read_ee(EE_PAGE0, MM_COUNT_ADDR);
    tp_cnt = save_stack_header();
    
    if( type==TEST_PIN_T || type==TEST_CYCLE_S   )
    {
        stack_buffer[tp_cnt].map.state = 'T';
        stack_buffer[tp_cnt].map.reboot_time[0] = 1;
        stack_buffer[tp_cnt].map.reboot_time[1] = 1;
    }
    if( tp_cnt < BUFFER_STACK )
    {
        stack_buffer[tp_cnt].map.dial_type = 1; //usb line1~4
        //MML,ACCT18,E60200002
        cnt = 20;
        cnt = stack_data_header(tp_cnt, mm_cnt, cnt);
        mm_cnt = 0;
        
		// This function store data into stack_buffer[tp_cnt].
		//                             0    1    2    3    4    5    6    7        20   21    22   23   24   25   26   27   28   29   30 
		//  Ex: stack_buffer[0][] = 0x50 0x01 0xF2 0xFF 0xFF 0x03 0x03 0x00 .... 0x30 0x30 0x037 0x35 0x34 0x38 0x2C 0x34 0x30 0x30 0x37
		//                             P                                                       (7548)                  (4007)
        // Added alarm_string to stack_buffer - Ex. "18,115400$"
        //  stack_buffer[31]~[40] = "18,115400$"
//        do{
//            temp = sp[mm_cnt++];
//            stack_buffer[tp_cnt][cnt++] = temp;
//        }while(temp!='$');

        strncpy((char *)stack_buffer[tp_cnt].map.alarm_string, (const char *)sp, MAX_STR_SIZE-1);
        
        // stack_buffer[0][40] = ZONE1 from EEPROM
        // stack_buffer[0][41] = zone
        // stack_buffer[0][42] = zone
        // stack_buffer[0][43] = CR
        stack_buffer[tp_cnt].map.zone_1 = page0_eeprom.map.ZONE1;
        stack_buffer[tp_cnt].map.zone[0]= (zone_ext/10)+0x30;
        stack_buffer[tp_cnt].map.zone[1] = (zone_ext%10)+0x30;

        // Carriage return
        stack_buffer[tp_cnt].map.cr = CR; 
    }
    else 
        return(0);   
    
	return(1);
}

uint8_t chk_data_type(uint8_t *buffer,uint8_t buffer_p)
{
	uint8_t cnt,temp1,temp2,random,swap;
	uint8_t i_cnt;
    
    buffer_p -= 2;    

	for( i_cnt=0;i_cnt<buffer_p;i_cnt++ )
	{
		temp1 = buffer[i_cnt];
		if( temp1==','||temp1==' ' )
		{
			if( buffer[0]>='0'&&buffer[0]<='9'&&buffer[1]>='0'&&buffer[1]<='9' )
			{							
				break;
			}
	 	}
	}	
	//decrypt
	if( i_cnt==buffer_p )		
	{		        
		/*---decrypt code---*/
		random = buffer[0];
		random = (uint8_t)(((random>>4)+(random&0x0f)))%16;
		//random = 0;
		for( cnt=1;cnt<(buffer_p-1);cnt++ )
		{
			temp2 = buffer[cnt];
			temp1 = encryption_code[random];	
			temp2 ^= temp1;					//xor		
			swap = (uint8_t) (temp2 <<6);				//left shift 2 bit
			temp2 >>= 2;
			temp2 += swap;
			swap = (uint8_t) (temp2<<4);					//4bit swap
			temp2 >>=4;
			temp2 += swap;	
			temp2 ^= temp1;					//xor	
			buffer[cnt-1] = temp2;
			if( ++random>=16 )
				random = 0;			
		}
		buffer[cnt-1] = 0xcc;
		buffer_p = cnt;
	  	/*------------------*/
	}else 
    {
        if( buffer[buffer_p-1]==0x0d )
            buffer[buffer_p-1] = 0xcc;
        else buffer[buffer_p++] = 0xcc;                    
    }         
	if( buffer[2]==' '&&buffer[3]=='N'&&buffer[4]=='A'&&buffer[5]=='C')//&&buffer[6]=='K' )
	{
		if( (buffer[0] == stack_buffer[0].map.mm_count[0]) && (buffer[1]==stack_buffer[0].map.mm_count[1]) )
		{
			stack_buffer[0].map.ack_count = 1;
			temp1 = (uint8_t) (stack_buffer[0].map.retry_count & 0xf0);
			stack_buffer[0].map.retry_count = temp1;
			return('N');
	   	}
	}else
	{	
        // TODO: WHAT'S THIS FOR???
		cnt = 0;
		do{
            // TODO: temporary use .data.
			temp1 = stack_buffer[0].data[cnt+20];
			temp2 = buffer[cnt];	

			if( temp1!=temp2 )
			{
				if( temp1==0x0d&&temp2==0xcc )
				{			
					move_stack_buffer();
					return('K');
				}
				return('E');
			}
		}while(++cnt<=buffer_p);
	}
	return('E');
}

//------------------------------//
uint8_t check_emc_stack(void)
{
    uint8_t temp,cnt,rsp,loop,count;
    
#ifdef DEBUG
    uint8_t const ascii[]="0123456789ABCDEF";
#endif
    
send_start:
    //lock_buffer = 0;
    temp = stack_buffer[0].map.state;
    if( temp == 'P' || temp == 'S' || temp== 'T' )     //connect to server
    {
      //  encryption = 0;
        GIE = 0;
        if( temp=='P' )
            stack_buffer[0].map.state = 'S';
        GIE = 1;
        
        // Encrypt or just copy to resp_buffer[].
        load_emc_number();				


        //TMR0IE = 0;
        delay5ms(200);

        TL_internet_init();       
        
        cnt = 1;
        do{
            if(stack_buffer[0].map.state=='T')
                loop = 1;
            else
                loop = page0_eeprom.map.CYCLE;
            do{
                CREN1 = 0;
                rsp = TL_connection_open(cnt);
                rsp = 'K';
                if( rsp=='K' )      //send data to server
                {
                    rsp = TL_send_data_to_server();     
                    if( rsp=='K' )     
                    {
                        count = page0_eeprom.map.SERVER_ACK_TIME;
                        do{                            
                            delayseconds(5);
                            rsp = TL_receive_data_from_server();         
                            delayseconds(2);
                        }while(--count!=0&&rsp=='E');
                    }
                }else 
                {
                    update_led_state(SEND_ERR);
                }
                delay5ms(100);
                TL_connection_close();
                delay5ms(200);
                if( stack_buffer[0].map.state == 'T' && cnt==2 )
                    break;
            }while(--loop!=0&&rsp!='K'); 
        }while(++cnt<5&&rsp!='K');  
        
        TL_internet_close();
        if( rsp=='K' )		
        {
            //move_stack_buffer();
            delay5ms(200);

            retry_count = 0;
            goto send_start;            
        }
        else 
            retry_count = read_ee(EE_PAGE0,0xBD)*600;
        
        stack_buffer[0].map.reboot_time[0]--;
        if( stack_buffer[0].map.reboot_time[0]==0 )
        {
            stack_buffer[0].map.reboot_time[0] = stack_buffer[0].map.reboot_time[1];
            return('E');
        }
        else return('U');
    }

    return('E');
}

void enque_event(uint8_t event, uint8_t zone)
{
    uint8_t front = event_que.front;

    event_que.data[front].event = event;
    event_que.data[front].zone = zone;
    
    if( ++event_que.front >= EVENT_MAX )
        event_que.front = 49;
}

void deque_event(void)
{
    uint8_t rear;
    
    while(event_que.rear != event_que.front)
    {
        rear = event_que.rear;
        
        alarm_out(event_que.data[rear].event, event_que.data[rear].zone);
        event_que.rear++;

        if (event_que.rear >= EVENT_MAX)
            event_que.rear = 0;
        
        // TODO: This should be wrap up to 0???
        if( event_que.rear == event_que.front )
        {
            event_que.front = 0;
            event_que.rear = 0;
        }
    };
}

bool is_event_que_empty(void)
{
    return (event_que.front == event_que.rear) ? true : false;        
}