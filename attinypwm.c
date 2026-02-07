#include <avr/io.h>
#include <util/delay.h>

void pwm_init() {
    // 1. Set PB1 as an output pin
    DDRB |= (1 << PB1);

    // 2. Set TCCR0A (Timer/Counter Control Register A)
    // COM0B1: Clear OC0B on Compare Match, set at BOTTOM (Non-inverting mode)
    // WGM01 & WGM00: Set Fast PWM Mode (Mode 3)
    TCCR0A = (1 << COM0B1) | (1 << WGM01) | (1 << WGM00);

    // 3. Set TCCR0B (Timer/Counter Control Register B)
    // CS01: Set prescaler to 8 (Clock / 8)
    TCCR0B = (1 << CS01);
}

int main(void) {
    pwm_init();

    uint8_t brightness = 0;

    while (1) {
        // OCR0B controls the duty cycle on PB1
        OCR0B = brightness;
        
        brightness++;
        _delay_ms(10); 
    }
}