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
#include "inc.h"
    
/*****************************************************
 * FUNCTION PROTOTYPES
 ****************************************************/  
bool    alarm_out(uint8_t type,uint8_t zone_ext);
void    add_event(uint8_t event, uint8_t zone);
void    check_event(void);
void    load_emc_number(void);
uint8_t check_emc_stack(void);
uint8_t chk_data_type(uint8_t *buffer, uint8_t buffer_p);

/*****************************************************
 * VARIABLES
 ****************************************************/ 

//-------------------- 
// Event definitions    
typedef enum event_enum{
    SMOKE_ALARM_T   = 0,
    TEST_CYCLE_S    = 1,
    LOW_BATTERY_T   = 2,   
    TAMPER_OPEN_T   = 3,
    TAMPER_CLOSE_T  = 4,    
    TEST_PIN_T      = 5,
    LOW_BATTERY_S   = 6,     
    EOL_T           = 7,
    FLOOD_T         = 8,
    GLASS_T         = 9,
    SUPERVISORY_T   = 10,
    CARBON_T        = 11,
    MOTION_T        = 12,
    DOOR_T          = 13,
    PANIC_T         = 14,
    HVAC_T          = 15,
    APPLIANCE_T     = 16,
    RESERVE1_T      = 17,
    RESERVE2_T      = 18,
    RESERVE3_T      = 19,
    RESERVE4_T      = 20,
    RESERVE5_T      = 21,
    RESERVE6_T      = 22,
    RESERVE7_T      = 23,   
    GO_SMS_T        = 24              
} EVENT_ENUM;

#define NUM_OF_STR      25
#define MAX_STR_SIZE    20
uint8_t alarm_string[NUM_OF_STR][MAX_STR_SIZE] = {
    "18,111100$",       //002$";
    "18,E60200$",       //035$";
    "18,130200$",       //002$";
    "18,138300$",       //002$";
    "18,338300$",       //002$";
    "18,E60100$",       //002";
    "18,138600$",       //002$";
#ifdef DEBUG
    "18,130211$",       //002$";
#else
    "18,138400$",       //002$";
#endif
    "18,115400$",       //002$";
    "18,114000$",       //002$";   
    "18,E38100$",       //002$";
    "18,116200$",       //002$";
    "18,113200$",       //002$";
    "18,1134700$",      //002$";
    "18,110100$",       //002$";
    "18,116100$",       //002$";
    "18,115200$",       //002$";
    "18,115500$",       //002$";
    "18,118000$",       //002$";
    "18,118100$",       //002$";
    "18,118200$",       //002$";
    "18,118300$",       //002$";
    "18,118400$",       //002$";
    "18,118500$",       //002$";
    "18,E30600$"        //002$";
};

#define SMOKE_ALARM_T   0    
#define TEST_CYCLE_S    1
#define LOW_BATTERY_T   2    
#define TAMPER_OPEN_T   3
#define TAMPER_CLOSE_T  4    
#define TEST_PIN_T      5
#define LOW_BATTERY_S   6      
#define EOL_T           7
#define FLOOD_T         8
#define GLASS_T         9
#define SUPERVISORY_T   10
#define CARBON_T        11
#define MOTION_T        12
#define DOOR_T          13
#define PANIC_T         14
#define HVAC_T          15
#define APPLIANCE_T     16
#define RESERVE1_T      17
#define RESERVE2_T      18
#define RESERVE3_T      19
#define RESERVE4_T      20
#define RESERVE5_T      21
#define RESERVE6_T      22
#define RESERVE7_T      23   
#define GO_SMS_T        24  

#ifdef	__cplusplus
}
#endif

#endif	/* EMC_LIBRARY_H */

