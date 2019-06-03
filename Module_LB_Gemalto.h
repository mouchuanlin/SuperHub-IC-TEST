/* 
 * File:   Module_LB_Gemalto.h
 * Author: YEN
 *
 * Created on 2018年1月15日, 下午 4:22
 */

#ifndef MODULE_LB_GEMALTO_H
#define	MODULE_LB_GEMALTO_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "stdint.h"
#include "stdbool.h"

void GM_module_first_run(void);
uint8_t GM_internet_init_EMS31(void);
uint8_t GM_connection_open(uint8_t type);
void GM_connection_close(void);
void GM_internet_close(void);
uint8_t GM_send_data_to_server(void);
uint8_t GM_receive_data_from_server(void);



#ifdef	__cplusplus
}
#endif

#endif	/* MODULE_LB_GEMALTO_H */

