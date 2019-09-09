//
// eeprom_setup.h
//

#ifndef EEPROM_SETUP_H
#define	EEPROM_SETUP_H

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
uint8_t function_code(void);
uint8_t set_n01_02_03_04_35_36(uint8_t);
uint8_t set_n05(void);
uint8_t set_n06_14(uint8_t);
uint8_t set_n07(void);
uint8_t set_n08(void);
uint8_t set_n09_15_16(uint8_t);
uint8_t set_n10(void);
uint8_t set_n11(void);
uint8_t set_n12_13(uint8_t);
uint8_t set_n14(void);
uint8_t set_n31_32_33_34_37(uint8_t);
uint8_t set_n41_to_68(uint8_t);
uint8_t back_door_function(uint8_t);
uint8_t set_n98(void);

bool is_digit(uint8_t digit);

extern void write_ee(uint8_t page, uint8_t addr, uint8_t data_p);

/*****************************************************
 * VARIABLES
 ****************************************************/


#ifdef	__cplusplus
}
#endif

#endif	/* EEPROM_SETUP_H */

