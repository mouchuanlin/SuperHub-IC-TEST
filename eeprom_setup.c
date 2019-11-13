//
// eeprom_setup.c
//

#include "eeprom_setup.h"
#include "eeprom.h"
#include "io.h"


// This function is called to perform ID# setup function from user SMS.
uint8_t sms_setup_functions(void)
{
	uint8_t poundH, poundL, response='E';
    uint8_t cmd, option;
    
    uint8_t index;
    bool    found = true;

    
	poundH = (uint8_t) (key[0] & 0x0F);
	poundL = (uint8_t) (key[1] & 0x0F);
	
	cmd = (uint8_t) (poundH*10 + poundL);
	
    // ID: 1,2,3,4,35,36 - IP address
	if ( ((cmd >= 1) && (cmd <= 4)) || (cmd == 35) || (cmd == 36) )
		option = 0;
    // ID: 5 access code
	else if ( cmd == 5 )
		option = 1;
	else if ( (cmd == 6) || (cmd == 14) )
		option = 2;
	else if ( cmd == 7 )
		option = 3;
	else if ( cmd == 8 )
		option = 4;
	else if ( (cmd == 9) || (cmd == 15) || (cmd == 16) )
		option = 5;
	else if ( cmd == 10 )
		option = 6;
	else if ( cmd == 11 )
		option = 7;
	else if ( cmd == 12 )
		option = 8;
	else if ( ((cmd >= 31) && (cmd <= 34)) || (cmd == 37) )
		option = 9;
	else if ( cmd == 98 )
		option = 10;
    else if( cmd >= ID_START && cmd <= ID_END )   
        option = 11;
    else
        found = false;
    
    if (found)
        response = (*func_ptr[option])(cmd);
    
    
//    uint8_t junk = sizeof(sms_setup_funs)/sizeof(sms_setup_fun_t);
//    for (index = 0 ; index < sizeof(sms_setup_funs)/sizeof(sms_setup_fun_t); index++)
//    {
////        response = sms_setup_funs[index].(*func_ptr())(cmd);
////        return response
//    }

	return response;
}

// This function is called to setup different IP address - IP1/2/3/4, APN, OTA.
//  EX: 
//      1#*#
//      1#198.17.112.128#
//      35#c2.korem2m.com#
//      36#12.12.201.84#
//
uint8_t set_n01_02_03_04_35_36(uint8_t cmd)
{
	uint8_t cnt,temp,dot,addr,page;
	
    page = EE_PAGE0;
    if( cmd == P_OTA )
    {
        page = EE_PAGE1;
        addr = 0xD0;
    }
    else if( cmd == P_APN )
		addr = 0x10;
	else 
        addr = (uint8_t) ((cmd <<4 )*2+0x10);

    // Delete device ID. Ex: 01#*#
	if( key_p == 5 && key[3] == '*' && cmd != P_APN )
	{
		write_ee(EE_PAGE0, addr,'#');
		return('K');
	}

	cnt = 0x03;
	dot = 0;
	do{
		temp = key[cnt++];
	}while(temp != '#' && cnt<36);

	if( cnt>=36 )
		return('E');

	cnt = 0x03;
	do{
		temp = key[cnt++];
		if( cmd != 32 )
		{
			if( temp=='*' )
				temp = '.';
	  	}
		write_ee(page,addr,temp);
		addr++;
	}while(temp!='#');
	return('K');
}

// This function is called to setup Access Code.
//  EX:
//      05#1111#
//
// Access code - 4-6 digits/Char
//  EX: 05#123456#  - 10
//      05#1234#    - 8
uint8_t set_n05(uint8_t cmd)
{
	uint8_t index,temp,addr;
	
	index = 0x03;
	while (temp != '#' && index < 0x09)
    {
		temp = key[index++];		 
	};
	
	if( temp!='#' || index<0x07 )
		return('E');
    
//    strncpy((char *)page0_eeprom.map.ACCESS_CODE, (const char *)key[3], index - 3);
    update_eeprom_page0();
    
// 	index = 0x03;
//    addr = 0xC0;
//	do{
//		temp = key[index++];
// 	    write_ee(EE_PAGE0,addr,temp);
//		addr++;
//	}while(temp!='#');
    
    
    CLRWDT();
	return('K');
}

// This function is called to setup 06#/14#
//  EX:
//
uint8_t set_n06_14(uint8_t cmd)
{
    uint8_t temp,addr;
	
    temp = key[3];
	if( key_p==5 && (temp=='1'||temp=='0') )
	{
        if( cmd == 0x06 )
            addr = 0xC7;
        else addr = 0xBB;
        if( temp=='1' )
            write_ee(EE_PAGE0,addr,0x01);
        else write_ee(EE_PAGE0,addr,0x00);
		return('K');
	}
	return('E');
}

// This function is called to setup 07#
//  EX:
//      07#10#
uint8_t set_n07(uint8_t cmd)
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

// This function is called to setup 08#
uint8_t set_n08(uint8_t cmd)
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

// This function is called to setup 09#/15#/16#.
//  EX:
//
uint8_t set_n09_15_16(uint8_t cmd)
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
            if( cmd==9 )
                write_ee(EE_PAGE0,0xB8,addr);
            else if( cmd==15 )     //17/11/08
                write_ee(EE_PAGE0,0xBC,addr);
            else if( cmd==16 )     //17/11/08
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

// This function is called to setup 10# Unit Account.
//  EX:
//      10#4007#
//
uint8_t set_n10(uint8_t cmd)
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
uint8_t set_n11(uint8_t cmd)
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
uint8_t set_n12_13(uint8_t cmd)
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

// This function is called to set PORT1/2/3/4 and OTA_Port.
//  EX:
//      31#2020#
//      37#2021#
//
uint8_t set_n31_32_33_34_37(uint8_t cmd)
{
	uint8_t cnt,temp;
	uint16_t addr;
    
    
    port_no = (port_stru_t *)&key[0];

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
			if( cmd==P_PORT1 )
			{
                // PORT1_ADDR
				write_ee(EE_PAGE0, 0xB0, (addr>>8));
				write_ee(EE_PAGE0, 0xB1, (addr&0x00ff));
			}else if( cmd==P_PORT1 )
			{
                // PORT2_ADDR
				write_ee(EE_PAGE0, 0xB2, (addr>>8));
				write_ee(EE_PAGE0, 0xB3, (addr&0x00ff));
			}else if( cmd==P_PORT1 )
			{
                // PORT3_ADDR
				write_ee(EE_PAGE0, 0xB4, (addr>>8));
				write_ee(EE_PAGE0, 0xB5, (addr&0x00ff));
			}else if( cmd==P_PORT1)
			{
                // PORT4_ADDR
				write_ee(EE_PAGE0, 0xB6, (addr>>8));
				write_ee(EE_PAGE0, 0xB7, (addr&0x00ff));
			}else if( cmd==P_PORT_OTA)
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

// This function is called to setup RF device ID in EEPROM. RF device ID from 41~56 - totally 16 entries.
//  EX:
//      41#627275#
//      42#892C31#
//      43#333435#
//
uint8_t set_n41_to_56(uint8_t cmd)
{
    uint8_t cnt,temp,val;
    
    // The parameter cmd is the EEPROM ID#. It starts from 41 while we want save to array[0]
    // RF slave device ID store in EEPROM Page 1 address 0 - 8 bytes for each device.
    cnt = 0x03;
    cmd -= 41;
    val = cmd;
    cmd *= 8;
    // Delete ID 41 - 1111#41#*#, key[] hold SMS without access code.
    if( key[3]=='*'&&key[4]=='#' )
    {        
        for( cnt=0;cnt<8;cnt++ )
        {            
            write_ee(EE_PAGE1, (uint8_t) (cmd+cnt), 0);
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
            temp = key[cnt+3];
            write_ee(EE_PAGE1, (uint8_t) (cmd+cnt), temp);
            device_id_table[val][cnt] = temp;
        }
        
        // TODO: Why write to [6]???
        write_ee(EE_PAGE1, cmd+6, 0);
        device_id_table[val][6] = 0;
        
        // TODO: Why write to [7]???
        write_ee(EE_PAGE1, cmd+7, 0);
        device_id_table[val][7] = 0;
        return('K');
    }
    return('E');
}

// This function is called to re-init EEPROM
//  EX:
//      98#*#
uint8_t set_n98(uint8_t cmd)
{
	if( key[3]=='*'&&key[4]=='*'&&key[5]=='#' )
	{
        write_ee(EE_PAGE0,0x00,0x00);
		init_pic18_eeprom();
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