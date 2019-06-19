//
// eeprom.c
//

#include <pic18f26k22.h>
#include <xc.h>

#include "eeprom.h"
#include "io.h"

uint8_t read_ee(uint8_t page,uint8_t addr)
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

void write_ee(uint8_t page,uint8_t addr,uint8_t data_p)
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
	EECON2 = 0xaa;
	WR = 1;
	GIE = 1;
	WREN = 0;
	while (WR == 1)
        ;
    CLRWDT();
    GIE = 1;
}

// TODO: Check spec
bool is_first_run()
{
    return (bool)(ee_read(0, FIRST_RUN_CHECK) != 0x57);
}

void save_first_run()
{
    ee_write(0, FIRST_RUN_CHECK, 0x57);
}

void init_eeprom()
{   
    //---------Check Version-----------
    // This make sure we only run first time.
    if((read_ee(EE_PAGE0,  VER_ADDR0) == VERSION[0]) && 
		(read_ee(EE_PAGE0, VER_ADDR1) == VERSION[1]) && 
		(read_ee(EE_PAGE0, VER_ADDR2) == VERSION[2]))
        return;

    //---------APN-----------
    write_EE_setting(EE_PAGE0, APN_ADDR, APN);
    //---------IP1-----------
    write_EE_setting(EE_PAGE0, IP1_ADDR, IP1);
    //---------IP2-----------
    write_EE_setting(EE_PAGE0, IP2_ADDR, IP2);
    //---------IP3-----------
    write_EE_setting(EE_PAGE0, IP3_ADDR, IP3);
    //---------IP4-----------
    write_EE_setting(EE_PAGE0, IP4_ADDR, IP4);
    //---------PORT1---------
    write_ee(EE_PAGE0, PORT1_ADDR, (PORT1 >> 8));
    write_ee(EE_PAGE0, (PORT1_ADDR+1), (PORT1 & 0x00ff));
    //---------PORT2---------
    write_ee(EE_PAGE0, PORT2_ADDR, (PORT2 >> 8));
    write_ee(EE_PAGE0, (PORT2_ADDR+1), (PORT2 & 0x00ff));
    //---------PORT3---------
    write_ee(EE_PAGE0, PORT3_ADDR, (PORT3 >> 8));
    write_ee(EE_PAGE0, (PORT3_ADDR+1),(PORT3 & 0x00ff));
    //---------PORT4---------
    write_ee(EE_PAGE0, PORT4_ADDR, (PORT4 >> 8));
    write_ee(EE_PAGE0, (PORT4_ADDR+1), (PORT4 & 0x00ff));
    //---------ACCESS_CODE-----------
    write_EE_setting(EE_PAGE0, ACCESS_CODE_ADDR, ACCESS_CODE);
    //---------PROGRAM_ACK-----------
    write_ee(EE_PAGE0, PROGRAM_ACK_ADDR, PROGRAM_ACK);                //06#
    //---------TEST_FREQ-----------
    write_ee(EE_PAGE0, TESTING_FREQ_ADDR, TEST_FREQ);                  //07#
    //---------SERVER_ACK_TIME-----------
    write_ee(EE_PAGE0, SERVER_ACK_TIME_ADDR, SERVER_ACK_TIME);         //08#
    //---------SMS_WAIT_TIME-----------
    write_ee(EE_PAGE0, SMS_WAIT_TIME_ADDR, SMS_WAIT_TIME);             //09#
    //---------UNIT_ACCNT-----------
    write_EE_setting(EE_PAGE0, UNIT_ACCT_ADDR, UNIT_ACCNT);
    //---------LINE_CARD-----------                   
    write_EE_setting(EE_PAGE0, LINE_CARD_ADDR, LINE_CARD);    
    //-----------ZONE---------------                
    write_ee(EE_PAGE0, ZONE1_ADDR, ZONE1);				//12#
 //   write_ee(0,0xBA,ZONE2);       					//13#
    write_ee(EE_PAGE0, TP_PIN_ADDR, TP_PIN);            //14#
    write_ee(EE_PAGE0, CYCLE_ADDR, CYCLE);              //15#
    write_ee(EE_PAGE0, RETRY_TIMES_ADDR, RETRY);        //16# 
    //---------ENCRYPTION-----------
    write_ee(EE_PAGE0, ENCRYPTION_ADDR, ENCRYPTION);    //95#
        
	// TODO: What's this???
    write_ee(EE_PAGE0, 0x0f, 0x00);   //MM
    
    // Device ID - 41#~56#
    for(uint8_t i = 0; i <(16*8); i++ )      		//28
        write_ee(EE_PAGE1, i, 0);    
    
    load_RF_devID_table();
    
    //---------IP OTA---------      		//36
    write_EE_setting(EE_PAGE1, IP_OTA_ADDR, IP_OTA); 
    //---------PORT OTA---------    		//37
    write_ee(EE_PAGE1, PORT_OTA_ADDR, (PORT_OTA >> 8));
    write_ee(EE_PAGE1, (PORT_OTA_ADDR+1), (PORT_OTA & 0x00ff));
    
    //------------------------------
    write_ee(EE_PAGE0, VER_ADDR0, VERSION[0]);
    write_ee(EE_PAGE0, VER_ADDR1, VERSION[1]);
    write_ee(EE_PAGE0, VER_ADDR2, VERSION[2]);
    
    load_default();
}

void load_default(void)
{
    encryption = read_ee(EE_PAGE0, ENCRYPTION_ADDR);
    respond_day = read_ee(EE_PAGE0, TESTING_FREQ_ADDR);
    test_enable = read_ee(EE_PAGE0, TP_PIN_ADDR);
}

void write_EE_setting(uint8_t page, uint8_t addr, uint8_t const setting[])
{
    uint8_t temp, cnt = 0;

    do {
        temp = setting[cnt];
        write_ee(page, addr+cnt, temp);
        cnt++;
    } while (temp != '#');    
}

// Check if the APN & IP1 has been set. In this case, will try to send data.
void check_led_type(void)
{
    if((read_ee(EE_PAGE0, APN_ADDR)=='#') || (read_ee(EE_PAGE0, IP1_ADDR)=='#'))    
    {
        LED_flash_type = LED_NO_SET;
        IP_type = 0;
    }
    else
    {
        LED_flash_type = LED_STANDBY;
        IP_type = 1;
    }
}

//---------------new add 2017/11/23
void load_RF_devID_table()
{
    uint8_t cnt1,cnt2,addr;
    for( cnt1=0;cnt1<28;cnt1++ )
    {
        addr =cnt1*8;
        for( cnt2=0;cnt2<8;cnt2++ )
            RF_devID_table[cnt1][cnt2] = read_ee(1,addr+cnt2 );
        
        RF_devID_table[cnt1][cnt2] = 0;
    }
    CLRWDT();
}

uint8_t check_ID(uint8_t *ptr)
{
    uint8_t cnt1,cnt2,temp;
    for( cnt1=0;cnt1<16;cnt1++ ) //28
    {
        for( cnt2=0;cnt2<6;cnt2++ )
        {
            temp = RF_devID_table[cnt1][cnt2];
            if( temp != ptr[cnt2] )
            {
         //       out_sbuf2(ptr[cnt2]);
                break;             
            }
            
        }
      //  out_sbuf2(',');
        if( cnt2==6 )
        {
    //        out_sbuf2('S');
            return(cnt1+3);   //respond Zone number 3~30
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
        temp = RF_devID_table[cnt1][0];
        if( temp==0x00 )
        {            
            addr =cnt1*8;
            for( cnt2=0;cnt2<6;cnt2++)
            {
                temp = ptr[cnt2];
                RF_devID_table[cnt1][cnt2]=temp;
                write_ee(1,(addr+cnt2),temp);
            }
            RF_devID_table[cnt1][6]=0;
            write_ee(1,addr+6,0);
            RF_devID_table[cnt1][7]=0;
            write_ee(1,addr+7,0);
            RF_devID_table[cnt1][8]=0;
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
    addr =id*8;
    for( cnt1=0;cnt1<8;cnt1++ )
    {
        RF_devID_table[id][cnt1] = 0x00;
        write_ee(1,addr+cnt1,0x00);
        return('K');
    }
    CLRWDT();
    return('E');
}

uint8_t check_supervisory(void)
{
    uint8_t cnt1,cnt2,temp;
    for( cnt1=0;cnt1<16;cnt1++ )    //28
    {
        if( RF_devID_table[cnt1][0]!=0 )
        {
            if( RF_devID_table[cnt1][7] < 84 )         // 12 = 1Day  84 = 7Day
            {
                if( ++RF_devID_table[cnt1][7] >= 84 )
                {            
                    add_event(SUPERVISORY_T,cnt1+3);
                    RF_devID_table[cnt1][7] = 'S';
                }
            }
        }
        CLRWDT();
    }    
    return(0);   
}

void rsp_SUP_LBT(void)
{
    uint8_t cnt1,cnt2,temp;
    for( cnt1=0;cnt1<16;cnt1++ )    //28
    {
        if( RF_devID_table[cnt1][0]!=0 )
        {
            if( RF_devID_table[cnt1][6]=='B' )                  
                add_event(LOW_BATTERY_T,cnt1+3);       
            if( RF_devID_table[cnt1][7]=='S' )                  
                add_event(SUPERVISORY_T,cnt1+3);            
        }
    }
    CLRWDT();
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
