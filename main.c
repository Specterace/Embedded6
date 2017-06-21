/*
 * Lab6.c
 *
 * Created: 6/2/2017 2:27:49 PM
 * Author : Oscar Choy
 * 
 * This project uses an ATMega328P board, a an external DAC converter, a potentiometer, and an
 * external LED light. It will dim and brighten the external LED according to the turned angle of
 * the knob on the potentiometer. Using Analog-to-Digigal conversion, Digital-to-Analog conversion,
 * and SPI, the knob's position will be used by the ATMega328P board through ADC to send 
 * the DAC chip a value (it will send this value using SPI, with the ATMega328P being the master 
 * sending the value and the DAC chip being the slave recieving the value). The DAC will then 
 * convert this value (sent in two char bytes) into an analog voltage that will power the external
 * LED, brightening and dimming the light according to the position of the potentiometer's knob. 
 * The voltage will vary between 1.5V and 5V.
 */

#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include <stdbool.h>

void SPI_MasterInit(void);
void SPI_MasterTransmit(char cData);
void SPI_SlaveInit(void);
char SPI_SlaveReceive(void);
unsigned int GetADCResult(void);
void ADC_Init();

unsigned int valADR = 0;
unsigned int passVal = 0;
float tempVal = 0.0;
unsigned char hiByte = (0x00);
unsigned char loByte = (0x00);

int main(void)
{
	DDRB |= 0<<DDB2;
	DDRB |= 1<<DDB3;
	DDRB |= 1<<DDB5;
	DDRC |= 0<<DDC1;
	
	PORTC |= 1<<PORTC1;
	
	ADC_Init();
	SPI_MasterInit();
	
	while(1) {
		valADR = GetADCResult();
		tempVal = 1230 + (valADR * 2.8);
		passVal = (int)tempVal;
		loByte = (passVal & (0xff));
		hiByte = (0x30) | ((passVal >> 8) & (0x0f));
		PORTB &= ~(1 << DDB2);
		SPI_MasterTransmit(hiByte);
		SPI_MasterTransmit(loByte);
		PORTB |= (1 << DDB2);
	}
	
	return 0;
}

void SPI_MasterInit(void) {
	DDRB = (1<<DDB3)| (1<<DDB5) | (1<<DDB2);
	PORTB |= (1<<PORTB2);
	SPCR = (1<<SPE) | (1<<MSTR) | (1<<SPR0);
	
}

void SPI_MasterTransmit(char cData) {
	SPDR = cData;
	while(!(SPSR & (1<<SPIF)));
}

void ADC_Init() {
	DDRC &= 0x00;
	ADMUX |= (1<<REFS0) | (1<<MUX0);
	ADCSRA |= (1<<ADEN) | (1<<ADPS2) | (1<<ADPS1) | (1<<ADPS0); 
}

unsigned int GetADCResult(void) {
	ADCSRA |= (1<<ADSC);
	while (ADCSRA & 1<<ADSC);
	return(ADC);
}