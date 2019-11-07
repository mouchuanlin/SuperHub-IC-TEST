//
// file: eeprom.c
//

#include <string.h>
#include "eeprom.h"
#include "io.h"
#include "timer.h"
#include "led.h"
#include "modem.h"
#include "eeprom_setup.h"

// Read EEPROM for len (mainly use for 256) bytes
uint8_t *read_eeprom(uint8_t page, uint8_t addr, uint8_t *ptr, uint16_t len)
{
    uint8_t p_data;
    
    GIE = 0;
    EEADRH = page;
    
    while (len--)
    {
        EEADR = addr++;  
        CFGS = 0;
        EEPGD = 0;
        RD = 1;        
        p_data = EEDATA;      
        *ptr++ = p_data;
    }
    GIE = 1;
    CLRWDT();
    
	return(ptr-len);
}

// Write EEPROM for len (mainly use for 256) bytes
void write_eeprom(uint8_t page, uint8_t addr, uint8_t *data_p, uint16_t len)
{
    uint8_t temp;
    
    GIE = 0;
    EEADRH = page;
    
    while (len--)
    {
        EEADR = addr++;
        temp = *data_p++;
        EEDATA = temp;
        CFGS = 0;
        EEPGD = 0;
        WREN = 1;
        GIE = 0;        
        
        EECON2 = 0x55;
        EECON2 = 0xAA;
        WR = 1;
        GIE = 1;
        WREN = 0;
           
        while (WR == 1)
            ;
    }
    CLRWDT();
    GIE = 1;
}

// Write EEPROM for len (mainly use for 256) bytes for data_p value
void set_eeprom_value(uint8_t page, uint8_t addr, uint8_t data, uint16_t len)
{    
    GIE = 0;
    EEADRH = page;
    
    while (len--)
    {
        EEADR = addr++;
        EEDATA = data;
        CFGS = 0;
        EEPGD = 0;
        WREN = 1;
        GIE = 0;           
        
        EECON2 = 0x55;
        EECON2 = 0xAA;
        WR = 1;
        GIE = 1;
        WREN = 0;
           
        while (WR == 1)
            ;
    }
    CLRWDT();
    GIE = 1;
}

uint8_t read_ee(uint8_t page, uint8_t addr)
{
    uint8_t p_data;
    
    GIE = 0;
    EEADRH = page;
    EEADR = addr;
	CFGS = 0;
	EEPGD = 0;
	RD = 1;
    p_data = EEDATA;
    GIE = 1;
    CLRWDT();
    
	return(p_data);
}

void write_ee(uint8_t page, uint8_t addr, uint8_t data_p)
{
    GIE = 0;
    EEADRH = page;
    EEADR = addr;
	EEDATA = data_p;
	CFGS = 0;
	EEPGD = 0;
	WREN = 1;
	GIE = 0;
    // Write 55h to EECON2. Write 0AAh to EECON2.
	EECON2 = 0x55;
	EECON2 = 0xAA;
	WR = 1;
	GIE = 1;
	WREN = 0;
	while (WR == 1)
        ;
    CLRWDT();
    GIE = 1;
}

// TODO: for some reason, FIRST_RUN doesn't work. Keep here for further investigation.
bool is_first_run()
{   
    // TODO: for some reason, FIRST_RUN doesn't work. Keep here for further investigation.
    //return (read_ee(EE_PAGE0, FIRST_RUN_ADDR) != FIRST_RUN) ? true : false;
    
    // This make sure we only run first time.
    if((read_ee(EE_PAGE0,  VER_ADDR0) == VERSION[0]) && 
		(read_ee(EE_PAGE0, VER_ADDR1) == VERSION[1]) && 
        (read_ee(EE_PAGE0, VER_ADDR2) == VERSION[2]) &&
		(read_ee(EE_PAGE0, VER_ADDR2) == VERSION[3]))
        return false;
    
    return true;
    
}

void init_pic18_eeprom(void)
{           
    uint16_t i;
    
    if (!is_first_run())
        return;
    
	// Init to 0x00
	set_eeprom_value(EE_PAGE0, EE_START_ADDR, 0x00, EE_PAGE_SIZE);
	set_eeprom_value(EE_PAGE1, EE_START_ADDR, 0x00, EE_PAGE_SIZE);
    
    page0_eeprom.map.FIRST_RUN = FIRST_RUN;
	strncpy((char *)page0_eeprom.map.VERSION, (const char *)VERSION, (size_t)sizeof(VERSION));		
	strncpy((char *)page0_eeprom.map.APN, (const char *)APN, (size_t)sizeof(APN));
	strncpy((char *)page0_eeprom.map.IP1, (const char *)IP1, (size_t)sizeof(IP1));
	strncpy((char *)page0_eeprom.map.IP2, (const char *)IP2, (size_t)sizeof(IP2));
	strncpy((char *)page0_eeprom.map.IP3, (const char *)IP3, (size_t)sizeof(IP3));
	strncpy((char *)page0_eeprom.map.IP4, (const char *)IP4, (size_t)sizeof(IP4));
				
	page0_eeprom.map.PORT1 = PORT1;
	page0_eeprom.map.PORT2 = PORT2;
	page0_eeprom.map.PORT3 = PORT3;
	page0_eeprom.map.PORT4 = PORT4;
    
	
	strncpy((char *)page0_eeprom.map.ACCESS_CODE, (const char *)ACCESS_CODE, (size_t)sizeof(ACCESS_CODE));
	
    page0_eeprom.map.PROGRAM_ACK = PROGRAM_ACK;
    page0_eeprom.map.TEST_FREQ = TEST_FREQ;
    page0_eeprom.map.SERVER_ACK_TIME = SERVER_ACK_TIME;
    page0_eeprom.map.SMS_WAIT_TIME = SMS_WAIT_TIME;

	strncpy((char *)page0_eeprom.map.UNIT_ACCNT, (const char *)UNIT_ACCNT, (size_t)sizeof(UNIT_ACCNT));
	strncpy((char *)page0_eeprom.map.LINE_CARD, (const char *)LINE_CARD, (size_t)sizeof(LINE_CARD));
    
    page0_eeprom.map.ZONE1 = ZONE1;    

	page0_eeprom.map.TP_PIN = TP_PIN;   
	page0_eeprom.map.CYCLE = CYCLE;   
	page0_eeprom.map.RETRY = RETRY;   
	page0_eeprom.map.ENCRYPTION = ENCRYPTION;   
	page0_eeprom.map.MM_COUNT = MM_COUNT;

  
    load_device_id_table();
	
	// Page 1 EEPROM  
	strncpy((char *)page1_eeprom.map.IP_OTA, (const char *)IP_OTA, (size_t)sizeof(IP_OTA));
	page1_eeprom.map.PORT_OTA = PORT_OTA;
    

    // Programming EEPROM to default values.
    write_eeprom(EE_PAGE0, EE_START_ADDR, page0_eeprom.data, EE_PAGE_SIZE);
    write_eeprom(EE_PAGE1, EE_START_ADDR, page1_eeprom.data, EE_PAGE_SIZE);
    
    // TODO: FOR DEBUGGING ONLY
    ////////////////////////////////
    write_test_device_id();
    ////////////////////////////////  
    
    load_default();
}

void write_test_device_id()
{
    key_p = 10;

    strncpy((char *)key, (const char *)"41#627275#", 10);
    set_n41_to_56(41);
    strncpy((char *)key, (const char *)"42#892C31#", 10);
    set_n41_to_56(42);
    strncpy((char *)key, (const char *)"43#333435#", 10);
    set_n41_to_56(43);
    key_p = 0;    
}

void load_default(void)
{
    //encryption = read_ee(EE_PAGE0, ENCRYPTION_ADDR);
    // How often we want report low 
    respond_day = page0_eeprom.map.TEST_FREQ;
    test_enable = page0_eeprom.map.TP_PIN;
}

// Check if the APN & IP1 has been set. In this case, will try to send data.
void check_ip_setting()
{
    if((read_ee(EE_PAGE0, APN_ADDR)=='#') || (read_ee(EE_PAGE0, IP1_ADDR)=='#'))    
    {
        ready_for_sending = false;
        IP_type = 0;
    }
    else
    {
        ready_for_sending = true;
        IP_type = 1;
    }
}

//---------------new add 2017/11/23
void load_device_id_table()
{
    uint8_t cnt1,cnt2,addr;
	
    for( cnt1=0;cnt1<28;cnt1++ )
    {
        addr =cnt1*8U;
        for( cnt2=0;cnt2<8;cnt2++ )
            device_id_table[cnt1][cnt2] = read_ee(1, (uint8_t)(addr+cnt2) );
        
        device_id_table[cnt1][cnt2] = 0;
    }
    CLRWDT();
}

uint8_t get_zone_number(uint8_t id[])
{
    uint8_t row , column, temp;
    
    // Check if input ID match to any entry in device_id_table.
    for( row = 0; row < ID_TABLE_ROW; row++ )
    {
        for( column = 0; column < ID_LEN; column++ )
        {
            temp = device_id_table[row][column];
            if( temp != id[column] )
            {
                //out_sbuf2(ptr[cnt2]);
                break;             
            }            
        }
        //out_sbuf2(',');
        if( column == ID_LEN)
        {
            //ut_sbuf2('S');
            // TODO: Why zone# is the cntl+3???
            return(row+3U);   //respond Zone number 3~30
        }
    }
    CLRWDT();
  //  out_sbuf2('N');
    return(0);   
}

uint8_t add_ID(uint8_t *ptr)
{
    uint8_t cnt1,cnt2,temp,addr;
    GIE = 0;
    for( cnt1=0;cnt1<16;cnt1++ )    //28
    {
        temp = device_id_table[cnt1][0];
        if( temp==0x00 )
        {            
            addr =cnt1*8U;
            for( cnt2=0;cnt2<6;cnt2++)
            {
                temp = ptr[cnt2];
                device_id_table[cnt1][cnt2]=temp;
                write_ee(1,(uint8_t)(addr+cnt2),temp);
            }
            device_id_table[cnt1][6]=0;
            write_ee(1,addr+6U,0);
            device_id_table[cnt1][7]=0;
            write_ee(1,addr+7U,0);
            device_id_table[cnt1][8]=0;
            GIE = 1;
            return('K');
        }             
    }
    CLRWDT();
    GIE = 1;
    return('E');
}

uint8_t del_ID(uint8_t id)
{
    uint8_t addr,cnt1;
    id -=3;
    addr =id*8U;
    for( cnt1=0;cnt1<8;cnt1++ )
    {
        device_id_table[id][cnt1] = 0x00;
        write_ee(1, (uint8_t) (addr+cnt1), 0x00);
        return('K');
    }
    CLRWDT();
    return('E');
}



/*uint8_t send_trigger_to_RF(uint8_t type)
{
    if( HL_freq==0 )
    {
        RF_wait_count = 10;
        OSCCON = HIGH_FREQ_OSCCON;	// 4MHz
        T0CON = HIGH_FREQ_T0CON;             //1*4000 = 50,000us
        HL_freq = 1;
        UART2_init();
        CREN1 = 0;          
        CREN1 = 1;
        RC2IE = 1;
    }
    WIFI_INT1 = 0;
    WIFI_INT1_TRIS = OUTPUT;
    WIFI_INT1 = 1;
    delay5ms(200);   
    WIFI_INT1 = 0;
    out_sbuf2('$');
    if( type==0 )
        out_sbuf2('T');
    else out_sbuf2('A');
    out_sbuf2(0x0d);
    out_sbuf2(0x0a);
    WIFI_INT1_TRIS = INPUT;
    WIFI_INT1 = 1;
}*/
