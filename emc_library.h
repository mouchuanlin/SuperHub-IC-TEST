/* 
 * File:   emc_library.h
 * Author: YEN
 *
 * Created on 2017年5月2日, 下午 2:10
 */

#ifndef EMC_LIBRARY_H
#define	EMC_LIBRARY_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "stdint.h"
#include "stdbool.h"
    
bit alarm_out(uint8_t type,uint8_t zone_ext)@0xA000;
void add_event(uint8_t even, uint8_t zone);
void check_event(void);
void load_emc_number(void);
uint8_t check_emc_stack(void);
uint8_t chk_data_type(uint8_t *buffer, uint8_t buffer_p);

#ifdef	__cplusplus
}
#endif

#endif	/* EMC_LIBRARY_H */

