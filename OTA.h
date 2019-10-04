//
// OTA.h
//

#ifndef OTA_H
#define	OTA_H

/*****************************************************
 * INCLUDES
 ****************************************************/
#include "inc.h"

/*****************************************************
 * VARIABLES
 ****************************************************/    
// Event definitions    
typedef enum md_resp {
    MD_OK,
    MD_ERROR,
    MD_NO_CARRIER,
            
    OTA_BOOT_SEL,
    OTA_RED,
    OTA_CONNECT,
    OTA_OFA,
    OTA_UNKNOW,
            
} md_resp_t;

/*****************************************************
 * FUNCTION PROTOTYPES
 ****************************************************/
uint8_t     get_ota_ip_addr(void);
uint8_t     get_ip1_addr(void);
md_resp_t   wait_ota_status(uint16_t count);
uint8_t     check_OTA(void);

//extern void delayseconds(uint16_t secs);
//extern void poweroff_modem(void);
//extern void set_boot_sel_input(void);
//extern void set_boot_sel_output(void);
//extern void reload_timer3_50ms(void);
//extern void check_receive_overrun(void);


#endif	/* OTA_H */

