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
 * FUNCTION PROTOTYPES
 ****************************************************/
void    check_OTA_status();
uint8_t get_ota_ip_addr();
uint8_t get_ip1_addr();

extern void delayseconds(uint16_t secs);
extern void poweroff_modem();
extern void set_boot_sel_input();
extern void set_boot_sel_output();

/*****************************************************
 * VARIABLES
 ****************************************************/


#endif	/* OTA_H */

