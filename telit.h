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
#include "inc.h"

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
bool is_ip_exists(uint8_t type);

/*****************************************************
 * VARIABLES
 ****************************************************/


#ifdef	__cplusplus
}
#endif

#endif	/* TELIT_H */

