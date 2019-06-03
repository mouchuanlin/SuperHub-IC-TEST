/* 
 * File:   EE_library.h
 * Author: YEN
 *
 * Created on 2017年4月12日, 下午 2:14
 */

#ifndef EE_LIBRARY_H
#define	EE_LIBRARY_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "stdint.h"
#include "stdbool.h"

uint8_t read_ee(uint8_t, uint8_t );
void write_ee(uint8_t, uint8_t, uint8_t );

#ifdef	__cplusplus
}
#endif

#endif	/* EE_LIBRARY_H */

