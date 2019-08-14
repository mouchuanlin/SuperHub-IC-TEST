//
// telit.h
//

#ifndef TELIT_H
#define	TELIT_H

#ifdef	__cplusplus
extern "C" {
#endif
    
/*****************************************************
 * INCLUDES
 ****************************************************/    
#include "stdint.h"

/*****************************************************
 * FUNCTION PROTOTYPES
 ****************************************************/ 
void    TL_module_first_run(void);
uint8_t TL_internet_init(void);
uint8_t TL_connection_open(uint8_t type);
void    TL_connection_close(void);
void    TL_internet_close(void);
uint8_t TL_send_data_to_server(void);
uint8_t TL_receive_data_from_server(void);
uint8_t get_ip_addr(uint8_t type);

/*****************************************************
 * VARIABLES
 ****************************************************/
extern uint8_t wait_ok_respond(uint16_t);

#ifdef	__cplusplus
}
#endif

#endif	/* TELIT_H */

