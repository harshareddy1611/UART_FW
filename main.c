#ifndef F_CPU
#define F_CPU 16000000UL  
#endif

#include <avr/io.h>
#include <util/delay.h>
#include <avr/eeprom.h>
#include <stdio.h>
#include <avr/interrupt.h>  

#define BAUD 2400
#define UBRR_VALUE ((F_CPU / (16UL * BAUD)) - 1)
#define MAX_SIZE 1020
#define TIMEOUT 5000  

char buffer[MAX_SIZE];

void USART_Init() {
    UBRR0H = (uint8_t)(UBRR_VALUE >> 8);
    UBRR0L = (uint8_t)UBRR_VALUE;
    UCSR0B = (1 << RXEN0) | (1 << TXEN0);  
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);  
}

void USART_Transmit(char data) {
    while (!(UCSR0A & (1 << UDRE0)));  
    UDR0 = data;
}

char USART_Receive() {
    while (!(UCSR0A & (1 << RXC0)));  
    return UDR0;
}

void USART_Transmit_String(const char* str) {
    while (*str) {
        USART_Transmit(*str++);
    }
}

volatile uint32_t timer_millis = 0;


ISR(TIMER0_OVF_vect) {
    timer_millis += 16; 
}

void Timer0_Init() {
    TCCR0B |= (1 << CS01) | (1 << CS00);  
    TIMSK0 |= (1 << TOIE0);  
    sei();  
}

uint32_t millis() {
    uint32_t ms;
    cli(); 
    ms = timer_millis;
    sei();  
    return ms;
}

void store_data_in_eeprom(int size) {
    int i;
    for (i = 0; i < size; i++) {
        eeprom_write_byte((uint8_t*)i, buffer[i]);
        _delay_ms(3);
    }
}

void send_data_from_eeprom(int size) {
    int i;
    USART_Transmit_String("\nSTART_EEPROM\n");
    for (i = 0; i < size; i++) {
        USART_Transmit(eeprom_read_byte((uint8_t*)i));
        _delay_us(600);
    }
    USART_Transmit_String("\nEND_EEPROM\n");
}

int main() {
    USART_Init();
    Timer0_Init();
    USART_Transmit_String("Arduino Ready. Waiting for PC data...\n");

    int index = 0;
    uint32_t lastReceivedTime = millis();

    while ((millis() - lastReceivedTime) < TIMEOUT) {
        if (UCSR0A & (1 << RXC0)) { 
            char received = USART_Receive();
            if (index < MAX_SIZE) {
                buffer[index++] = received;
            }
            lastReceivedTime = millis();
        }
    }

    store_data_in_eeprom(index);

    char speed_str[16];
    uint32_t speed = (index * 8) / ((millis() - lastReceivedTime) / 1000.0);
    sprintf(speed_str, "%lu bits/sec\n", speed);
    USART_Transmit_String("\nData storage complete.\n");
    USART_Transmit_String("Transmission Speed (PC → Arduino): ");
    USART_Transmit_String(speed_str);

    _delay_ms(2000);
    send_data_from_eeprom(index);

    while (1); 
}
