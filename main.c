/*
 * main.c
 *
 * Created: 1/28/2026 8:16:40 AM
 *  Author: Jumbleview
 *  based on "Make: AVR Programming" by Elliot Williams (Chapter: Timers PWM Demo)
 *  Program receives decimal input from serial interface
 *  and set it as timer preset for PWM LED control
 */ 

#include <xc.h>

#include <avr/io.h>

// F_CPU assumes Atmega328 running on internal 8Mhz clock with SKDIV fuse unchecked
#define F_CPU 8000000UL

#include <util/delay.h>

#include <string.h>
#include <stdbool.h>


// Define CPU Frequency for delay and baud calculations

#define BAUD 9600
#define MYUBRR F_CPU/16/BAUD-1

#define pinOut(port,bit) \
(DDR##port |= (1 << ( DD##port##bit)));

#define pinPutHigh(port,bit) \
PORT##port |= (1 << (PORT##port##bit)); 

#define pinPutLow(port,bit) \
PORT##port &= ~(1 << (PORT##port##bit));

// -v- USART functions -v-
// usartInit initializes USART hardware 
void usartInit(unsigned int ubrr) {
	// Set baud rate registers
	UBRR0H = (unsigned char)(ubrr >> 8);
	UBRR0L = (unsigned char)ubrr;
	
	// Enable receiver and transmitter
	UCSR0B = (1 << RXEN0) | (1 << TXEN0);
	
	// Set frame format: 8 data bits, 1 stop bit
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

// usartTransmit sends one symbol via USART
void usartTransmit(unsigned char data) {
	// Wait for empty transmit buffer
	while (!(UCSR0A & (1 << UDRE0)));
	
	// Put data into buffer (sends the data)
	UDR0 = data;
}

// usartIsEmpty returns  'true' if there is no data to receive via USART  
bool usartIsEmpty() {
	return !(UCSR0A & (1 << RXC0));
}

// usartReceive waits for single byte to arrive via USART and returns it 
unsigned char usartReceive(void) {
	/* Wait for data to be received */
	while (usartIsEmpty());
	
	/* Get and return received data from buffer */
	return UDR0;
}

// usartPrint sends string buffer via USART
void usartPrint(const char* str) {
	while (*str) {
		usartTransmit(*str++);
	}
}

// usartGetDecimal read input line form serial interface ('\r\) arrives. If incoming character is decimal
// previous result multiplied by 10 and incoming character converted to number and added to the result
uint8_t usartGetDecimal(){
	uint8_t decimal = 0;
	unsigned char input = '\0';
	do {
		input = usartReceive();
		usartTransmit(input);
		if (input < '0' || input > '9') {
			continue;
		}
		decimal = decimal*10 + (input - '0');
	} while(input != '\r');
	usartTransmit('\n');
	return decimal;
}

// -v- Timer initialization -v-
static inline void initTimer(void) {
	// Timer 1 A,B
//	TCCR1A |= (1 << WGM10);                      /* Fast PWM mode, 8-bit */
//	TCCR1B |= (1 << WGM12);                       /* Fast PWM mode, pt.2 */
//	TCCR1B |= (1 << CS11);                     /* PWM Freq = F_CPU/8/256 */
//	TCCR1A |= (1 << COM1A1);                      /* PWM output on OCR1A */
//	TCCR1A |= (1 << COM1B1);                      /* PWM output on OCR1B */

	// Timer 2
//	TCCR2A |= (1 << WGM20);                             /* Fast PWM mode */
//	TCCR2A |= (1 << WGM21);                       /* Fast PWM mode, pt.2 */
//	TCCR2B |= (1 << CS21);                     /* PWM Freq = F_CPU/8/256 */
//	TCCR2A |= (1 << COM2A1);                      /* PWM output on OCR2A */
// 1. Set PB1 (OC1A) as output
//	DDRB |= (1 << DDB1);

	// 2. Configure Timer1 for Fast PWM, 8-bit
	// Mode 5: WGM13=0, WGM12=1, WGM11=0, WGM10=1
	// Compare Output Mode: Clear OC1A on compare match, set at BOTTOM (non-inverting)
	TCCR1A = (1 << COM1A1) | (1 << WGM10);  // non-inverting mode
	
	// TCCR1A = (1 << COM1A1) | (1 << COM1A0) | (1 << WGM10); // inverting mode
	
	TCCR1B = (1 << WGM12) | (1 << CS11) | (1 << CS10) ; //  Prescaler is 64

	OCR1A = 255;

}

// -v- main -v- 
int main(void) {
	pinOut(B,1);   // Configure B1 for output
	usartInit(MYUBRR);
	usartPrint("Hello from ATmega328!\r\n");	
	initTimer();
	uint8_t brightness = 255;
while (1) {
		usartPrint("\r\nEnter (0-255) for PWM duty cycle: ");
		brightness = usartGetDecimal();
		OCR1A = brightness;
	}                                                  /* End event loop */
	return 0;                            /* This line is never reached */
}
