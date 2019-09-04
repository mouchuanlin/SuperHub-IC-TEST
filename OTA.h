//
// OTA.h
//

#ifndef OTA_H
#define	OTA_H

/*****************************************************
 * INCLUDES
 ****************************************************/
#include <stdint.h>

/*****************************************************
 * VARIABLES
 ****************************************************/
// Event definitions    
typedef enum ota_resp {
    OTA_ERROR,
    OTA_NO_CARRIER,
    OTA_BOOT_SEL,
    OTA_RED,
    OTA_CONNECT,
    OTA_OFA,
    OTA_UNKNOW,
            
    MD_OK,
    MD_ERROR
} ota_resp_t;

/*****************************************************
 * FUNCTION PROTOTYPES
 ****************************************************/
uint8_t get_ota_ip_addr();
uint8_t get_ip1_addr();
ota_resp_t wait_ota_status(uint16_t count);

extern void delayseconds(uint16_t secs);
extern void poweroff_modem();
extern void set_boot_sel_input();
extern void set_boot_sel_output();
extern void reload_timer3_50ms();
extern void check_receive_overrun();


#endif	/* OTA_H */

