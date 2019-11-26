//
// file:    eeprom_setup.c
//

#include "eeprom_setup.h"
#include "eeprom.h"
#include "io.h"


// This function is called to perform ID# setup function from user SMS.
uint8_t sms_setup_functions(void)
{
	uint8_t cmdH, cmdL, response = 'E';
    uint8_t cmd, index;
    
	cmdH = (uint8_t) (key[0] & 0x0F);
	cmdL = (uint8_t) (key[1] & 0x0F);
	cmd = (uint8_t) (cmdH*10 + cmdL);
    
    for (index = 0 ; index < sizeof(sms_setup_funs)/sizeof(sms_setup_fun_t); index++)
    {
        if (cmd == sms_setup_funs[index].cmd)
        {
            return sms_setup_funs[index].func_ptr(cmd);
        }
    }

	return response;
}

// This function is called to setup different IP address - IP1/2/3/4, APN, OTA.
//  EX: 
//      35#c2.korem2m.com#
//      35#11583.mcs#
//
uint8_t set_n35(uint8_t cmd)
{
    uint8_t len;
    uint8_t apn[32];

    
    len = strlen((const char *)key);
    if (key[len-1] != '#')
        return 'E';
    else
        key[len-1] = '\0';
    
    strcpy((char *)apn, (const char *)&key[3]);
    
    // TODO: WHY this function changed the value of cmd???
    if (!is_valid_apn((char *)apn))
        return 'E';
    
    // 35# - APN
    strcpy((char *)page0_eeprom.map.APN, (const char *)&key[3]);
    
    update_eeprom();
    
	return 'K';
}

// This function is called to setup different IP address - IP1/2/3/4, APN, OTA.
//  EX: 
//      01#*#
//      01#198.17.112.128#
//      35#c2.korem2m.com#
//      36#12.12.201.84#
//
uint8_t set_n01_02_03_04_36(uint8_t cmd)
{
    uint8_t len;
    uint8_t ip[32];

    
    len = strlen((const char *)key);
    if (key[len-1] != '#')
        return 'E';
    else
        key[len-1] = '\0';
    
    strcpy((char *)ip, (const char *)&key[3]);

    if (!is_valid_ip((char *)ip))
        return 'E';
    
    if (cmd == P_OTA)
        strcpy((char *)page1_eeprom.map.IP_OTA, (const char *)&key[3]);
    else if (cmd == P_IP1)
        strcpy((char *)page0_eeprom.map.IP1, (const char *)&key[3]);
    else if (cmd == P_IP2)
        strcpy((char *)page0_eeprom.map.IP2, (const char *)&key[3]);
    else if (cmd == P_IP3)
        strcpy((char *)page0_eeprom.map.IP3, (const char *)&key[3]);
    else if (cmd == P_IP4)
        strcpy((char *)page0_eeprom.map.IP4, (const char *)&key[3]);
        
    update_eeprom();
    
	return 'K';
}

// This function is called to setup Access Code.
//  EX:
//      05#1111#
//
// Access code - 4-6 digits/Char
//  EX: 05#123456#  - 10
//      05#1234#    - 8
//
uint8_t set_n05(uint8_t cmd)
{
    uint8_t len;
    
    len = strlen((const char *)key);
    if (len < 8 || len > 10)
        return 'E';
    
    if (key[len-1] != '#')
        return 'E';
    else
        key[len-1] = '\0';
    
    if (!valid_digit((char *)&key[3]))
        return 'E';
    
    strncpy((char *)page0_eeprom.map.ACCESS_CODE, (const char *)&key[3], len - 4);
    update_eeprom_page0();
     
    CLRWDT();
	return('K');
}

// This function is called to setup 06#/14#
//  EX:
//      06#01#
//      14#00#
//
uint8_t set_n06_14(uint8_t cmd)
{
    uint8_t len, num;
    
    len = strlen((const char *)key);
    if (len < 5 || len > 6)
        return 'E';
    
    if (key[len-1] != '#')
        return 'E';
    else
        key[len-1] = '\0';
    
    num = atoi((const char *)&key[3]);
    
    if ( cmd == P_PROGRAM_ACK )
        page0_eeprom.map.PROGRAM_ACK = num;
    else if ( cmd == P_TP_PIN )
        page0_eeprom.map.TP_PIN = num;
    
    update_eeprom_page0();
    
	return('K');
}

// This function is called to setup 07#
//  EX:
//      07#10#
//
uint8_t set_n07(uint8_t cmd)
{
    uint8_t len, num;
    
    len = strlen((const char *)key);
    if (len < 5 || len > 6)
        return 'E';
    
    if (key[len-1] != '#')
        return 'E';
    else
        key[len-1] = '\0';
    
    num = atoi((const char *)&key[3]);
    
    page0_eeprom.map.TEST_FREQ = num;
    
    update_eeprom_page0();
    
    CLRWDT();
	return('K');
}

// This function is called to setup 08#
//  EX:
//      08#12#
//
uint8_t set_n08(uint8_t cmd)
{
    uint8_t len, num;
    
    len = strlen((const char *)key);
    if (len < 5 || len > 6)
        return 'E';
    
    if (key[len-1] != '#')
        return 'E';
    else
        key[len-1] = '\0';
    
    num = atoi((const char *)&key[3]);
    
    page0_eeprom.map.SERVER_ACK_TIME = num;
    
    update_eeprom_page0();
    
    CLRWDT();
	return('K');
}

// This function is called to setup 09#/15#/16#.
//  EX:
//      09#1#
//      09#12#
//
uint8_t set_n09_15_16(uint8_t cmd)
{
    uint8_t len, num;
    
    len = strlen((const char *)key);
    if (len < 5 || len > 6)
        return 'E';
    
    if (key[len-1] != '#')
        return 'E';
    else
        key[len-1] = '\0';
    
    num = atoi((const char *)&key[3]);
    
    if ( cmd == P_SMS_WAIT_TIME )
        page0_eeprom.map.SMS_WAIT_TIME = num;
    else if ( cmd == P_CYCLE )
        page0_eeprom.map.CYCLE = num;
    else if ( cmd == P_RETRY )
        page0_eeprom.map.RETRY = num;
    
    update_eeprom_page0();
        
    CLRWDT();
	return('K');
}

// This function is called to setup 10# Unit Account - 4~6 digits
//  EX:
//      10#4007#
//      10#123456#
//
uint8_t set_n10(uint8_t cmd)
{
    uint8_t len;
    
    len = strlen((const char *)key);
    if (len < 8 || len > 10)
        return 'E';
    
    if (key[len-1] != '#')
        return 'E';
    else
        key[len-1] = '\0';    

    if (!valid_digit((char *)&key[3]))
        return 'E';
     
    strncpy((char *)page0_eeprom.map.UNIT_ACCNT, (const char *)&key[3], len - 4);
    
    update_eeprom_page0();
   
    CLRWDT();
	return('K');
}

// This function is called to setup 11# Line Card - 4~6 digits
//  EX:
//      11#7548#
//      11#123456#
//
uint8_t set_n11(uint8_t cmd)
{
    uint8_t len;
    
    len = strlen((const char *)key);
    if (len < 8 || len > 10)
        return 'E';
    
    if (key[len-1] != '#')
        return 'E';
    else
        key[len-1] = '\0';    

    if (!valid_digit((char *)&key[3]))
        return 'E';
     
    strncpy((char *)page0_eeprom.map.LINE_CARD, (const char *)&key[3], len - 4);
    
    update_eeprom_page0();
    
    CLRWDT();
	return('K');
}

// This function is called to setup 12#/13#
//  EX:
//      12#12#
//
uint8_t set_n12_13(uint8_t cmd)
{
    uint8_t len, num;
    
    len = strlen((const char *)key);
    if (len < 8 || len > 10)
        return 'E';
    
    if (key[len-1] != '#')
        return 'E';
    else
        key[len-1] = '\0';    

    if (!valid_digit((char *)&key[3]))
        return 'E';
    
    num = atoi((const char *)&key[3]);
     
    if ( cmd == P_ZONE1 )
        page0_eeprom.map.ZONE1 = num;
    else if ( cmd == P_ZONE2 )
        page0_eeprom.map.ZONE2 = num;
    
    update_eeprom_page0();    

    return('K');
}

// This function is called to set PORT1/2/3/4 and OTA_Port.
//  EX:
//      31#2020#
//      37#2021#
//
uint8_t set_n31_32_33_34_37(uint8_t cmd)
{
    port_cmd_t *port_cmd = (port_cmd_t *)&key[0];

    if (port_cmd->pound != '#' || port_cmd->pound != '#')
        return('E');
    
    for (uint8_t i = 0; i < 4; i++)
    {
        if (!is_digit(port_cmd->port[i]))
             return('E');
    }
	
    if( cmd == P_PORT1 )
        strncpy((char *)page0_eeprom.map.PORT1, (const char *)port_cmd->port, 4);                 
    else if( cmd == P_PORT2 )
        strncpy((char *)page0_eeprom.map.PORT2, (const char *)port_cmd->port, 4);
    else if( cmd == P_PORT3 )
        strncpy((char *)page0_eeprom.map.PORT3, (const char *)port_cmd->port, 4);
    else if( cmd == P_PORT4)
        strncpy((char *)page0_eeprom.map.PORT4, (const char *)port_cmd->port, 4);        
    else if( cmd == P_PORT_OTA)
        strncpy((char *)page1_eeprom.map.PORT_OTA, (const char *)port_cmd->port, 4);    
    
    update_eeprom();

    CLRWDT();
	return('K');
}

// This function is called to setup RF device ID in EEPROM. RF device ID from 41~56 - totally 16 entries.
//  EX:
//      41#*#
//      41#627275#
//      42#892C31#
//      43#333435#
//
uint8_t set_n41_to_56(uint8_t cmd)
{
    uint8_t len, id;
    
    id = cmd - 41;
    
    // Delete ID 41 - 1111#41#*#, key[] hold SMS without access code.
    if( key[3]=='*' && key[4]=='#' )
    {        
        strncpy((char *)page1_eeprom.map.device_id_table[id], (const char *) 0x00, 6);
        page1_eeprom.map.device_id_table[id][6] = 0;
        page1_eeprom.map.device_id_table[id][7] = 0;

        update_eeprom_page1();
        
        return('K');
    }
    
    // Only if the whole string is 10 byte and the last byte is #
    // Ex: 41#892C31#
    len = strlen((char *)key);
    if (len != 10)
        return 'E';
    
    if (key[len-1] != '#')
        return 'E';
    else
        key[len-1] = '\0';      
        
    if (!valid_alnum((char *)&key[3]))
        return 'E';
        
    // Write ID to EEPROM and device_id_table
    strncpy((char *)page1_eeprom.map.device_id_table[id], (const char *)&key[3], 6);
    page1_eeprom.map.device_id_table[id][6] = 0;
    page1_eeprom.map.device_id_table[id][7] = 0;

    update_eeprom_page1();
        
    return('K');
}

// This function is called to re-init EEPROM
//  EX:
//      98#**#
//
uint8_t set_n98(uint8_t cmd)
{
	if ( key[3]=='*' && key[4] == '*' && key[5]=='#' )
	{
        restore_eeprom_init_value();
		return('K');
	}
	else 
        return('E');
}

bool is_digit(uint8_t digit)
{
    if ((digit >= 0x30) && (digit <= 0x39))
        return true;
    else
        return false;
}

// return  true if string contain only digits, else return false.
bool valid_digit(char *ip_str) 
{ 
    while (*ip_str) { 
        if (*ip_str >= '0' && *ip_str <= '9') 
            ++ip_str; 
        else
            return false; 
    } 
    
    return true; 
} 

// return true if IP string is valid, else return false.
bool is_valid_ip(char *ip) 
{ 
    int     num, dots = 0; 
    char    *ip_str;
    char    *ptr; 
  
    ip_str = ip;
    
    if (ip_str == NULL) 
        return false; 
  
    // Splitting a string 
    ptr = strtok(ip_str, DELIM); 
  
    if (ptr == NULL) 
        return false; 
  
    while (ptr)
    { 
        // after parsing string, it must contain only digits.
        if (!valid_digit(ptr)) 
            return false; 
  
        num = atoi(ptr); 
  
        // check for valid IP.
        if (num >= 0 && num <= 255) { 
            // parse remaining string
            ptr = strtok(NULL, DELIM); 
            if (ptr != NULL) 
                ++dots; 
        } else
            return false; 
    } 
  
    // valid IP string must contain 3 dots
    if (dots != 3) 
        return false; 
    
    return true; 
} 

// return  true if string contain only alphabets and digits, else return false.
bool valid_alnum(char *apn_str) 
{ 
    while (*apn_str) { 
        if (isalnum(*apn_str)) 
            ++apn_str; 
        else
            return false; 
    } 
    
    return true; 
} 

// return true if APN string is valid, else return false.
bool is_valid_apn(char *apn_str) 
{ 
    int     dots = 0; 
    char    *ptr; 
    
    if (apn_str == NULL) 
        return false; 
  
    // Splitting a string 
    ptr = strtok(apn_str, DELIM); 
  
    if (ptr == NULL) 
        return false; 
  
    while (ptr)
    { 
        // after parsing string, it must contain only digits.
        if (!valid_alnum(ptr)) 
            return false; 
        else { 
            // parse remaining string
            ptr = strtok(NULL, DELIM); 
            if (ptr != NULL) 
                ++dots; 
        } 
    } 
  
    // valid APN string must contain at least 1 dot
    if (dots < 1) 
        return false; 
    
    return true; 
} 