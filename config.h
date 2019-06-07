/* 
 * File:   config.h
 * Author: Dick
 *
 * Created on May 7, 2019, 9:52 AM
 */

#ifndef CONFIG_H
#define	CONFIG_H

// #include <stdio.h>
// #include <stdlib.h>
// #include <pic18f26k22.h>
// #include <xc.h>
// #include <stdint.h>
// #include <stdbool.h>

// #include "System_Library.h"
// #include "initial.h"
// #include "EE_library.h"
// #include "SMS_library.h"
// #include "Module_Library.h"
// #include "Module_LB_Gemalto.h"
// #include "Module_LB_Telit.h"
// #include "emc_library.h"

#include <stdint.h>
#include <stdbool.h>

#define TRUE 1
#define FALSE 0

// PIC18F26K22 Configuration Bit Settings
// 'C' source line config statements

// CONFIG1H
#pragma config FOSC = INTIO67   // Oscillator Selection bits (Internal oscillator block)
#pragma config PLLCFG = OFF     // 4X PLL Enable (Oscillator used directly)
#pragma config PRICLKEN = ON    // Primary clock enable bit (Primary clock enabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enable bit (Fail-Safe Clock Monitor disabled)
#pragma config IESO = OFF       // Internal/External Oscillator Switchover bit (Oscillator Switchover mode disabled)

// CONFIG2L
#pragma config PWRTEN = OFF     // Power-up Timer Enable bit (Power up timer disabled)
#pragma config BOREN = OFF  	// Brown-out Reset Enable bits (Brown-out Reset enabled in hardware only (SBOREN is disabled))
#pragma config BORV = 190       // Brown Out Reset Voltage bits (VBOR set to 1.90 V nominal)

// CONFIG2H
#pragma config WDTEN = SWON      // Watchdog Timer Enable bits (Watch dog timer is always disabled. SWDTEN has no effect.)
#pragma config WDTPS = 16    	// Watchdog Timer Postscale Select bits (1:32768)

// CONFIG3H
#pragma config CCP2MX = PORTC1  // CCP2 MUX bit (CCP2 input/output is multiplexed with RC1)
#pragma config PBADEN = OFF     // PORTB A/D Enable bit (PORTB<5:0> pins are configured as digital I/O on Reset)
#pragma config CCP3MX = PORTB5  // P3A/CCP3 Mux bit (P3A/CCP3 input/output is multiplexed with RB5)
#pragma config HFOFST = ON      // HFINTOSC Fast Start-up (HFINTOSC output and ready status are not delayed by the oscillator stable status)
#pragma config T3CMX = PORTC0   // Timer3 Clock input mux bit (T3CKI is on RC0)
#pragma config P2BMX = PORTB5   // ECCP2 B output mux bit (P2B is on RB5)
#pragma config MCLRE = INTMCLR  // MCLR Pin Enable bit (RE3 input pin enabled; MCLR disabled)

// CONFIG4L
#pragma config STVREN = ON      // Stack Full/Underflow Reset Enable bit (Stack full/underflow will cause Reset)
#pragma config LVP = OFF      	// Single-Supply ICSP Enable bit (Single-Supply ICSP enabled if MCLRE is also 1)
#pragma config XINST = OFF      // Extended Instruction Set Enable bit (Instruction set extension and Indexed Addressing mode disabled (Legacy mode))

// CONFIG5L
#pragma config CP0 = ON        // Code Protection Block 0 (Block 0 (000800-003FFFh) not code-protected)
#pragma config CP1 = ON        // Code Protection Block 1 (Block 1 (004000-007FFFh) not code-protected)
#pragma config CP2 = ON        // Code Protection Block 2 (Block 2 (008000-00BFFFh) not code-protected)
#pragma config CP3 = ON        // Code Protection Block 3 (Block 3 (00C000-00FFFFh) not code-protected)

// CONFIG5H
#pragma config CPB = ON        // Boot Block Code Protection bit (Boot block (000000-0007FFh) not code-protected)
#pragma config CPD = ON        // Data EEPROM Code Protection bit (Data EEPROM not code-protected)

// CONFIG6L
#pragma config WRT0 = OFF       // Write Protection Block 0 (Block 0 (000800-003FFFh) not write-protected)
#pragma config WRT1 = OFF       // Write Protection Block 1 (Block 1 (004000-007FFFh) not write-protected)
#pragma config WRT2 = OFF       // Write Protection Block 2 (Block 2 (008000-00BFFFh) not write-protected)
#pragma config WRT3 = OFF       // Write Protection Block 3 (Block 3 (00C000-00FFFFh) not write-protected)

// CONFIG6H
#pragma config WRTC = OFF       // Configuration Register Write Protection bit (Configuration registers (300000-3000FFh) not write-protected)
#pragma config WRTB = OFF       // Boot Block Write Protection bit (Boot Block (000000-0007FFh) not write-protected)
#pragma config WRTD = OFF       // Data EEPROM Write Protection bit (Data EEPROM not write-protected)

// CONFIG7L
#pragma config EBTR0 = OFF      // Table Read Protection Block 0 (Block 0 (000800-003FFFh) not protected from table reads executed in other blocks)
#pragma config EBTR1 = OFF      // Table Read Protection Block 1 (Block 1 (004000-007FFFh) not protected from table reads executed in other blocks)
#pragma config EBTR2 = OFF      // Table Read Protection Block 2 (Block 2 (008000-00BFFFh) not protected from table reads executed in other blocks)
#pragma config EBTR3 = OFF      // Table Read Protection Block 3 (Block 3 (00C000-00FFFFh) not protected from table reads executed in other blocks)

// CONFIG7H
#pragma config EBTRB = OFF      // Boot Block Table Read Protection bit (Boot Block (000000-0007FFh) not protected from table reads executed in other blocks)

// __delay_ms() need this define)
#define _XTAL_FREQ 8000000
// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

#define MODULE_OFF_TYPE   

#define _100milliseconds    780         // Timer0 interval for 8MHz clk
#define TMR0_CFG            0x87
    
    
// Forward declaration.
void 	buzzer_on(uint8_t count);
uint8_t get_hub_type();
void 	init_system();
void    timer0_init();
void    interrupt_init();
void 	powerup_modem();
void 	init_global_variables();
uint8_t modem_module_start();
uint8_t check_network_registration();
uint8_t check_SIM_state();
void 	init_stack_buffer();
uint8_t wait_AT_cmd_response();
uint8_t wait_SMS_setting();
void 	process_event_queue();
void 	process_ADC();
void 	process_RF_interrupt();
uint8_t process_SMS_setup_state();
uint8_t process_restart();
void 	process_supervisory();


void UART1_ISR();
void UART2_ISR();
void TMR0_ISR();
void smoker_ISR();
void superhub_ISR();
void handle_LED();
void handle_smoker();
void handle_learn_btn_pressed();
void start_timer0();
void reload_timer0();
void enable_timer3();
void reload_timer3_2s();
void reload_timer3_5s();


// Jen's simple_state_machine pronect
void start_modem();
void restart_modem();
void stop_modem();

bool md_config_ok();



//uint8_t Alarm_f = 0;
uint8_t Standby_f = 0;
uint8_t Error_f = 0;
//  uint8_t Test_f = 0;
//  bit Low_battery = 0;
// bit Smoke_EOL = 0;
bit Test_click = 0;        
uint8_t Smoke_respond = 0;    
bit ADC_time = 0;
//bit Respond_ack = 0;
uint8_t BT_S_respond = 0;
uint8_t BT_L_respond = 0;
uint8_t EOL_respond = 0;
//bit Tamper_open_f = 0;
//bit Tamper_close_f = 0;    
uint16_t test_count = 0;
uint16_t first_test = 18000; //30*60*10 (30mins)

uint8_t 	error_count = 0;
uint16_t 	standby_count = 0;
uint16_t 	error_time_detect = 0;
uint16_t 	test_time_detect = 0;
uint16_t 	adc_count = 0;
uint8_t 	Respond_T_Hour = 0;
uint8_t 	Respond_T_Day = 0;
uint8_t 	err_count=0;
uint8_t 	LED_count = 0;
uint8_t 	error_status_count = 0;
uint8_t 	alarm_count = 0;
//uint8_t 	rx2_cnt = 0;
//uint8_t 	rx2_buf[20];
uint16_t 	exit_learn;
uint8_t 	tamper_status = 1;
uint8_t 	first_tamper = 1;
uint8_t 	led_count = 0;
uint8_t 	learn_delay = 0;    
uint16_t 	test_9sec_count=0;
uint8_t 	OTA_flag = 0;



// UART1 RC buffer
// TODO: How big of the buffer should be???
#define     RX1_BUF_MAX     80
uint8_t 	rx1_cnt = 0;
uint8_t 	rx1_buf[RX1_BUF_MAX];

// UART2 RX buffer
#define     RX2_BUF_MAX     20
uint8_t 	rx2_cnt = 0;
uint8_t 	rx2_buf[RX2_BUF_MAX];


bool g_op_state = false;







	
#endif	/* CONFIG_H */