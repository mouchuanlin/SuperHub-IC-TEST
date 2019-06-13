
#include <pic18f26k22.h>
#include <xc.h>

#include "io.h"

uint8_t read_ee(uint8_t page,uint8_t addr)
{
    uint8_t p_data;
    GIE = 0;
    EEADRH = page;
    EEADR = addr;
	CFGS = 0;
	EEPGD = 0;
	RD = 1;
    p_data = EEDATA;
    GIE = 1;
    CLRWDT();
	return(p_data);
}

void write_ee(uint8_t page,uint8_t addr,uint8_t data_p)
{
    GIE = 0;
    EEADRH = page;
    EEADR = addr;
	EEDATA = data_p;
	CFGS = 0;
	EEPGD = 0;
	WREN = 1;
	GIE = 0;
    // Write 55h to EECON2. Write 0AAh to EECON2.
	EECON2 = 0x55;
	EECON2 = 0xaa;
	WR = 1;
	GIE = 1;
	WREN = 0;
	while (WR == 1)
        ;
    CLRWDT();
    GIE = 1;
}
