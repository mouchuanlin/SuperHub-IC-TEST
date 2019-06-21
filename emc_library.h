//
// emc_library.h
//

#ifndef EMC_LIBRARY_H
#define	EMC_LIBRARY_H

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
bit     alarm_out(uint8_t type,uint8_t zone_ext)@0xA000;
void    add_event(uint8_t event, uint8_t zone);
void    check_event(void);
void    load_emc_number(void);
uint8_t check_emc_stack(void);
uint8_t chk_data_type(uint8_t *buffer, uint8_t buffer_p);


extern void soutdata(uint8_t *dat);
extern void out_sbuf(uint8_t tmp);

/*****************************************************
 * VARIABLES
 ****************************************************/ 

#ifdef	__cplusplus
}
#endif

#endif	/* EMC_LIBRARY_H */

