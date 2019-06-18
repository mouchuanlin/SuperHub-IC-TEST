//
// Module_LB_Telit.h
//

#ifndef MODULE_LB_TELIT_H
#define	MODULE_LB_TELIT_H

#ifdef	__cplusplus
extern "C" {
#endif
    
#include "stdint.h"


void    TL_module_first_run(void);
uint8_t TL_internet_init(void);
uint8_t TL_connection_open(uint8_t type);
void    TL_connection_close(void);
void    TL_internet_close(void);
uint8_t TL_send_data_to_server(void);
uint8_t TL_receive_data_from_server(void);


extern uint8_t wait_ok_respond(uint16_t);

#ifdef	__cplusplus
}
#endif

#endif	/* MODULE_LB_TELIT_H */

