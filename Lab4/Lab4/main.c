#define F_CPU 16000000
//LIBRERIAS
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdint.h>

//VARIABLE GUARDADA, LISTA DE NUEMROS
uint8_t counter = 0;
uint8_t display[16] = {0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x6F,0x77,0x7C,0x39,0x5E,0x79,0x71};

//INICIO VA.
void initPC0(void);
void initADC(void);
int main(void)
{
	cli();
	DDRC |= (1 << PINC1) | (1 << PINC2) | (1 << PINC3);
	PORTC = 0;
	PORTC |= (1 << PORTC1);
	
	DDRD = 0xFF;
	PORTD = 0;
	UCSR0B = 0;
	
	PORTB |= (1 << PINB0) | (1 << PINB1);		//PINES CON PULL UP
	DDRB &= ~(1 << PINB0) | ~(1 << PINB1);		//ENTRADAS
	
	
	
	initADC();
	initPC0();
	sei();

	//LOPP 
	while (1)
	{
		ADCSRA |= (1 << ADSC);
		_delay_ms(5);//DELAY DE 5MS
		
		//ADC EN PUERTOD
		uint8_t decValue = ADCH;
		//COCIENTE Y RESTO	VAL DISP		
		uint8_t value1 = decValue / 16;		
		uint8_t value2 = decValue % 16;		
		
		//MULTEPLEXACION DE DISPLAYS
		PORTC = 0;
		PORTC |= (1 << PORTC1);
		PORTC &= ~(1 << PORTC2) | ~(1 << PORTC3);
		PORTD = counter;
		_delay_ms(5);
		
		PORTC = 0;
		PORTC |= (1 << PORTC2);
		PORTC &= ~(1 << PORTC1) | ~(1 << PORTC3);
		PORTD = display[value1];
		_delay_ms(5);
		
		PORTC = 0;
		PORTC |= (1 << PORTC3);
		PORTC &= ~(1 << PORTC2) | ~(1 << PORTC1);
		PORTD = display[value2];
	}
}

void initADC(void){
	ADMUX = 0;
	//VOLTAJE A 5V POT
	ADMUX |= (1 << REFS0);
	ADMUX &= ~(1 << REFS1);
	
	ADMUX |= (1 << ADLAR);	
	
	//CAMBIAR A ADC
	ADCSRA = 0;
	ADCSRA |= (1 << ADEN);	
	ADCSRA |= (1 << ADIE);	//INTERRUPCION
	
	//PRECALER 128 > 125
	ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
	//PC0 COMO ENTRADA
	DIDR0 |= (1 << ADC0D);	
}

ISR (ADC_vect){
	//PORTD = ADCH;	
	//PUETOD MUESTRA ADC Y APAGA BANDERA
	ADCSRA |= (1 << ADIF);	
}

void initPC0(void){
	//CAMBIO DE PINES
	PCICR |= (1 << PCIE0);	
	//MASCARA		
	PCMSK0 |= (1 << PCINT0) | (1 << PCINT1);	
}

//OPERACION INCREMENTA DECREMNETA
ISR (PCINT0_vect){
	if (!(PINB & (1 << PINB0))) {
		counter++;
	}else if (!(PINB & (1 << PINB1))) {
		counter--;
	}
}
