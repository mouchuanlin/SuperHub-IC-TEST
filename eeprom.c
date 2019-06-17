//
// eeprom.c
//

#include <pic18f26k22.h>
#include <xc.h>

#include "io.h"


uint8_t read_int_ee(uint8_t col)
{
	EEADRL = col;
	CFGS = 0;
	EEPGD = 0;
	RD = 1;
	return(EEDATL);
}

void write_int_ee(uint8_t col,uint8_t mdata)
{
	EEADRL = col;
	EEDATL = mdata;
	CFGS = 0;
	EEPGD = 0;
	WREN = 1;
	GIE = 0;
	EECON2 = 0x55;
	EECON2 = 0xaa;
	WR = 1;
	GIE = 1;
	WREN = 0;
	
	while (WR == 1)
		;
}

