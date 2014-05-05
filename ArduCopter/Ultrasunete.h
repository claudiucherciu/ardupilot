// ultrasunete.h
/* ATMEGA2560 AnalogIN pin mappings:
 *
 * PF0 ( ADC0 )	                Analog pin 0
 * PF1 ( ADC1 )	                Analog pin 1
 * PF2 ( ADC2 )	                Analog pin 2
 * PF3 ( ADC3 )	                Analog pin 3
 * PF4 ( ADC4/TMK )	        Analog pin 4
 * PF5 ( ADC5/TMS )	        Analog pin 5
 * PF6 ( ADC6 )	                Analog pin 6
 * PF7 ( ADC7 )	                Analog pin 7
 * PK0 ( ADC8/PCINT16 )      	Analog pin 8
 * PK1 ( ADC9/PCINT17 )	        Analog pin 9
 * PK2 ( ADC10/PCINT18 )	Analog pin 10
 * PK3 ( ADC11/PCINT19 )	Analog pin 11
 * PK4 ( ADC12/PCINT20 )	Analog pin 12
 * PK5 ( ADC13/PCINT21 )	Analog pin 13
 * PK6 ( ADC14/PCINT22 )	Analog pin 14
 * PK7 ( ADC15/PCINT23 )	Analog pin 15  
 *
 * Use these registers:
 * PCICR - bit 2 (PCIE2) to enable PCINT23:16
 * PCMSK2 - to select which pins have interrupts enabled
 * 
 */

#include<avr/interrupt.h>

//PINS
//Do not change ECHO port!
#define SENZOR_ECHO_PIN      PINK
#define SENZOR_ECHO_DDR      DDRK
#define SENZOR_FATA_ECHO     0
#define SENZOR_SPATE_ECHO    1
#define SENZOR_STANGA_ECHO   2
#define SENZOR_DREAPTA_ECHO  3


#define SENZOR_TRIG_PORT     PORTF
#define SENZOR_TRIG_DDR      DDRF
#define SENZOR_FATA_TRIG     0
#define SENZOR_SPATE_TRIG    1
#define SENZOR_STANGA_TRIG   2
#define SENZOR_DREAPTA_TRIG  3

#define SENZOR_MAX_RANGE    400 //cm
#define SENZOR_MIN_RANGE      2 //cm

#define SENZOR_DELAY_RESPONSE  450  //[us] the sensor has a delay between TRIG falling edge and ECHO rising edge


//4 sensors {Fata, spate, stanga, dreapta}
unsigned long distante[4] = {SENZOR_MAX_RANGE, SENZOR_MAX_RANGE, SENZOR_MAX_RANGE, SENZOR_MAX_RANGE};
unsigned long distante_start_millis = 0;
volatile unsigned long distante_end_millis[4] = {0, 0, 0, 0};

uint16_t senzorStart, senzorStop;
