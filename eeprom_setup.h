//
// Setup_mode.h
//

#ifndef SETUP_MODE_H
#define	SETUP_MODE_H

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
uint8_t function_code(void);
uint8_t set_n01_02_03_04_35_36(uint8_t);
uint8_t set_n05();
uint8_t set_n06_14(uint8_t);
uint8_t set_n07();
uint8_t set_n08();
uint8_t set_n09_15_16(uint8_t);
uint8_t set_n10();
uint8_t set_n11();
uint8_t set_n12_13(uint8_t);
uint8_t set_n14();
uint8_t set_n31_32_33_34_37(uint8_t);
uint8_t set_n41_to_68(uint8_t);
uint8_t back_door_function(uint8_t);
uint8_t set_n98(void);

/*****************************************************
 * VARIABLES
 ****************************************************/


#ifdef	__cplusplus
}
#endif

#endif	/* SETUP_MODE_H */

