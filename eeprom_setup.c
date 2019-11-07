//
// eeprom_setup.c
//

#include "eeprom_setup.h"
#include "eeprom.h"
#include "io.h"


//-----------------------------------//
uint8_t function_code(void)
{
	uint8_t temp1,temp2,respond='E';
	
	temp1 = (uint8_t) (key[0]&0x0f);
	temp2 = (uint8_t) (key[1]&0x0f);
	
	temp1 = (uint8_t) (temp1*10+temp2);
	
	switch(temp1)
	{
		// 01#, 02# ...
		case(1):	
	   	case(2):
       	case(3):	
	   	case(4):
		case(35):
        case(36):
					respond = set_n01_02_03_04_35_36(temp1);		//IP:APN
					break;
		case(5):	respond = set_n05();
					break;
	   	case(6):	
        case(14):
                    respond = set_n06_14(temp1);
					break;					
	   	case(7):	respond = set_n07();
					break;
	   	case(8):	respond = set_n08();
					break;
	   	case(9):	
        case(15):
        case(16):
                    respond = set_n09_15_16(temp1);
					break;
        case(10):	respond = set_n10();
					break;
	   	case(11):	respond = set_n11();
					break;
       	case(12):	
       // case(13):	
                    respond = set_n12_13(temp1);
					break;
		case(31):	
        case(32):	
        case(33):	
		case(34):
        case(37):   respond = set_n31_32_33_34_37(temp1);
					break;
                  
        // TODO; comment this out since there is a compiler warning and we are no longer use back door at all.
		case(95):	//back_door = 1;	
		case(96):
		case(97):
					if( back_door==0 )
						return('E');
					//--=-=-=-=-=-=-=-=-=-=-=-=-=-=-
					respond = back_door_function(temp1);
					break;
	   	case(98):	respond = set_n98();
					break;
	 	//======================================//
		case(80)://	respond = set_n80();	//CSQ
					break;
                    
		default:	
                // 41 ~ 56
                if( temp1 >= ID_START && temp1 <= ID_END )            
                {
                    // sensor device ID - 41 - 68
                    respond = set_n41_to_56(temp1);                        
                }else
                    return('E');
	}
	return(respond);
}


//-----------------------------------//
uint8_t set_n01_02_03_04_35_36(uint8_t type)
{
	uint8_t cnt,temp,dot,addr,page;
	
    page = 0;
    if( type==36 )
    {
        page = 1;
        addr = 0xD0;
    }else if( type==35 )
		addr = 0x10;
	else addr = (uint8_t) ((type<<4)*2+0x10);

	if( key_p==5&&key[3]=='*'&&type!=35 )
	{
		write_ee(EE_PAGE0,addr,'#');
		return('K');
	}

	cnt = 0x03;
	dot = 0;
	do{
		temp = key[cnt++];
	}while(temp!='#'&&cnt<36);

	if( cnt>=36 )
		return('E');

	cnt = 0x03;
	do{
		temp = key[cnt++];
		if( type!=32 )
		{
			if( temp=='*' )
				temp = '.';
	  	}
		write_ee(page,addr,temp);
		addr++;
	}while(temp!='#');
	return('K');
}

//-----------------------------------//
uint8_t set_n05(void)
{
	uint8_t cnt,temp,addr;
	cnt = 0x03;
	do{							 
		temp = key[cnt++];		 
	}while(temp!='#'&&cnt<0x09);
	
	if( temp!='#' || cnt<0x07 )
		return('E');
 	cnt = 0x03;
    addr = 0xC0;
	do{
		temp = key[cnt++];
 	    write_ee(EE_PAGE0,addr,temp);
		addr++;
	}while(temp!='#');
    CLRWDT();
	return('K');
}

//-----------------------------------//
uint8_t set_n06_14(uint8_t type)
{
    uint8_t temp,addr;
    temp = key[3];
	if( key_p==5 && (temp=='1'||temp=='0') )
	{
        if( type==0x06 )
            addr = 0xC7;
        else addr = 0xBB;
        if( temp=='1' )
            write_ee(EE_PAGE0,addr,0x01);
        else write_ee(EE_PAGE0,addr,0x00);
		return('K');
	}
	return('E');
}

//-----------------------------------//
uint8_t set_n07(void)
{
	uint8_t cnt, temp, addr;

	cnt = 0x03;
	addr = 0;
	do{
		temp = key[cnt++];
        
		if( temp=='#' )
		{
			if( cnt==0x04 || addr>30 )
				return('E');
			write_ee(EE_PAGE0,0xC8,addr);
			return('K');
		}else if(is_digit(temp))
		{
			temp &=0x0f;
			addr = (uint8_t) (addr*10 + temp);
		}else return('E');
	}while(cnt<0x06);
    CLRWDT();
	return('E');
}

//-----------------------------------//
uint8_t set_n08(void)
{
	uint8_t cnt,temp;
	uint16_t addr;

	cnt = 0x03;
	addr = 0;
	do{
		temp = key[cnt++];
		if( temp=='#' )
		{
			if( cnt==0x04 || addr>180 )
				return('E');
			write_ee(EE_PAGE0,0xC9,addr);
			return('K');
		}else if( is_digit(temp) )
		{
			temp &= 0x0f;
			addr = addr*10 + temp;
		}else return('E');
	}while(cnt<0x07);
    CLRWDT();
	return('E');
}

//-----------------------------------//
uint8_t set_n09_15_16(uint8_t type)
{
	uint8_t cnt,temp;
	uint16_t addr;

	cnt = 0x03;
	addr = 0;
	do{
		temp = key[cnt++];
		if( temp=='#' )
		{
			if( cnt==0x00 || addr>100 )
				return('E');
            if( type==9 )
                write_ee(EE_PAGE0,0xB8,addr);
            else if( type==15 )     //17/11/08
                write_ee(EE_PAGE0,0xBC,addr);
            else if( type==16 )     //17/11/08
                write_ee(EE_PAGE0,0xBD,addr);
            else return('E');
			return('K');
		}else if( is_digit(temp))
		{
			temp &= 0x0f;
			addr = addr*10 + temp;
		}else return('E');
	}while(cnt<0x07);
    CLRWDT();
	return('E');
}

//-----------------------------------//
uint8_t set_n10(void)
{
	uint8_t cnt,temp,addr,tp;

	cnt = 0x03;
	addr = 0;
	do{
		temp = key[cnt++];
		if( temp=='#' )
		{
			if( cnt==0x04 )
				return('E');
			cnt -= 4;	
			addr = 0xCA;	
			if( cnt<4 )
			{
				tp = (uint8_t) (4-cnt);
				do{
					write_ee(EE_PAGE0,addr,'0');
					addr++;
				}while(--tp!=0);
			}
			cnt = 0x03;
			do{
				temp = key[cnt++];
				write_ee(EE_PAGE0,addr,temp);
			}while(++addr<0xD0);
			return('K');
		}else if( !is_digit(temp) )
			return('E');
	}while(cnt<0x0a);
    CLRWDT();
	return('E');
}

//-----------------------------------//
uint8_t set_n11(void)
{
	uint8_t cnt,temp,addr;

	cnt = 0x03;
	addr = 0;
	do{
		temp = key[cnt++];
		if( temp=='#' )
		{
			if( cnt==0x04 )
				return('E');
			addr = 0xD0;
			cnt = 0x03;
			do{
				temp = key[cnt++];
				write_ee(EE_PAGE0,addr,temp);
				addr++;
			}while(temp!='#');
			return('K');
		}else if( !is_digit(temp) )
			return('E');
	}while(cnt<0x08);
    CLRWDT();
	return('E');
}

//-----------------------------------//
uint8_t set_n12_13(uint8_t type)
{
	uint8_t temp;

    temp = key[3];
    if( temp<'0'||temp>'9' )
        return('E');
    if( key[4]!='#' )
        return('E');
    write_ee(EE_PAGE0,0xB9,key[3]);  
	return('K');
}

//-----------------------------------//
uint8_t set_n31_32_33_34_37(uint8_t type)
{
	uint8_t cnt,temp;
	uint16_t addr;

	cnt = 0x03;
	addr = 0;
	do{
		temp = key[cnt++];
		if( temp=='#' )
		{
			if( cnt==0x04||addr<100 )
				return('E');
			if( cnt==9 )
			{
				if( key[3]=='6' )
				{
					if( key[4]=='5' )
					{
						if( key[5]=='5' )
						{
							if( key[6]=='3' )
							{
								if( key[7]>'5' )
									return('E');
							}else if( key[6]>'3' )
								return('E');			
						}else if( key[5]>'5' )
							return('E');
					}else if( key[4]>'5' )
						return('E');
				}else if( key[3]>'6' )
					return('E');
			}	
			if( type==31 )
			{
                // PORT1_ADDR
				write_ee(EE_PAGE0, 0xB0, (addr>>8));
				write_ee(EE_PAGE0, 0xB1, (addr&0x00ff));
			}else if( type==32 )
			{
                // PORT2_ADDR
				write_ee(EE_PAGE0, 0xB2, (addr>>8));
				write_ee(EE_PAGE0, 0xB3, (addr&0x00ff));
			}else if( type==33 )
			{
                // PORT3_ADDR
				write_ee(EE_PAGE0, 0xB4, (addr>>8));
				write_ee(EE_PAGE0, 0xB5, (addr&0x00ff));
			}else if( type==34)
			{
                // PORT4_ADDR
				write_ee(EE_PAGE0, 0xB6, (addr>>8));
				write_ee(EE_PAGE0, 0xB7, (addr&0x00ff));
			}else if( type==37)
			{
				write_ee(EE_PAGE1, 0xF0, (addr>>8));
				write_ee(EE_PAGE1, 0xF1, (addr&0x00ff));
			}
			return('K');
		}else if( is_digit(temp) )
		{
			temp &= 0x0f;
			addr = addr*10 + temp;
		}else return('E');
	}while(cnt<0x09);
    CLRWDT();
	return('E');
}

// RF device ID from 41~56 - totally 16 entries.
uint8_t set_n41_to_56(uint8_t type)
{
    uint8_t cnt,temp,val;
    
    // The parameter type is the EEPROM ID#. It starts from 41 while we want save to array[0]
    // RF slave device ID store in EEPROM Page 1 address 0 - 8 bytes for each device.
    cnt = 0x03;
    type -= 41;
    val = type;
    type *= 8;
    if( key[3]=='*'&&key[4]=='#' )
    {        
        for( cnt=0;cnt<8;cnt++ )
        {            
            write_ee(EE_PAGE1, (uint8_t) (type+cnt), 0);
            device_id_table[val][cnt] = 0;
        }
        return('K');
    }
    // Only if the whole string is 10 byte and the last byte is #
    // Ex: 41#892C31#
    if( key_p==10&&key[9]=='#')
    {                
        cnt = 3;
        do{
            temp = key[cnt];
             // ID can ONLY be number (0-9) or letter (A-Z or a-z)
            if( (temp>='0'&&temp<='9')||(temp>='A'&&temp<='F') )
            {
                NOP();
            }else if( temp>='a'&&temp<='f' )
            {
                key[cnt] -= 0x20;
            }else return('E');            
        }while(++cnt<9);
        
        // Write ID to EEPROM and device_id_table
        //  Ex : device_id_table[0] = "892C31"
        for( cnt=0;cnt<6;cnt++ )
        {
            // ID starts from byte 3 - Ex 892C31
            temp = key[cnt+3U];
            write_ee(EE_PAGE1, (uint8_t) (type+cnt), temp);
            device_id_table[val][cnt] = temp;
        }
        
        // TODO: Why write to [6]???
        write_ee(EE_PAGE1, type+6U, 0);
        device_id_table[val][6] = 0;
        
        // TODO: Why write to [7]???
        write_ee(EE_PAGE1, type+7U, 0);
        device_id_table[val][7] = 0;
        return('K');
    }
    return('E');
}

//-----------------------------------//
uint8_t back_door_function(uint8_t p)
{
	uint8_t temp,addr,cnt;
    
	if( p==95 )
	{
		if( key[0x04] !='#' )
			return('E');
	
		temp = key[0x03];
		if( temp=='0' || temp=='1' )
		{
			write_ee(EE_PAGE0, 0x7e, (uint8_t) (temp&0x0f));
			return('K');
		}
	}else if( p==96 )
	{
		cnt = 0x03;
		addr = 0;
		do{
			temp = key[cnt++];
			if( temp=='#' )
			{
				if( cnt==0x04 || addr>30 )
					return('E');
				write_ee(EE_PAGE0,0x7f,addr);
				return('K');
			}else if( is_digit(temp) )
			{
				temp &=0x0f;
				addr = addr*10U + temp;
			}else return('E');
		}while(cnt<0x06);
		return('E');
	}else if( p==97 )
	{
		if( key[3]=='#' )
			return('E');

		if( key[4]=='#' )
		{
			if( key[3]=='*' )
			{
				write_ee(EE_PAGE0,0xb0,0x0c);
				return('K');
			}
			else return('E');
		}
		cnt = 0x03;
		do{
			temp = key[cnt++];
			if( temp!='#' )
			{
				if( !is_digit(temp) )
					return('E');
			}
		}while(temp!='#'&&cnt<0x12);
	
		if( temp!='#' )
			return('E');

		cnt = 0x03;
		addr = 0xb0;
		do{
			temp = key[cnt++];
			write_ee(EE_PAGE0,addr,temp);
			addr++;
		}while(temp!='#');
		return('K');
	}
	return('E');
}

//-----------------------------------//
uint8_t set_n98(void)
{
	if( key[3]=='*'&&key[4]=='*'&&key[5]=='#' )
	{
        write_ee(EE_PAGE0,0x00,0x00);
		init_eeprom();
		return('K');
	}
	else return('E');
}

bool is_digit(uint8_t digit)
{
    if ((digit >= 0x30) && (digit <= 0x39))
        return true;
    else
        return false;
}