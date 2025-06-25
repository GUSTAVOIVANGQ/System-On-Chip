#include <avr/io.h>
#include <avr/interrupt.h>
// Practica 1
#define F_CPU 16000000UL  // Frecuencia del reloj (16MHz para Arduino)
#define BAUD 9600
#define MYUBRR F_CPU/16/BAUD-1

void USART_Init(unsigned int ubrr) {
    // Configurar el baud rate
    UBRR0H = (unsigned char)(ubrr>>8);
    UBRR0L = (unsigned char)ubrr;
    
    // Habilitar transmisor
    UCSR0B = (1<<TXEN0);
    
    // Configurar formato del frame: 8 bits de datos, 1 bit de parada
    UCSR0C = (1<<UCSZ01)|(1<<UCSZ00);
}

void USART_Transmit(unsigned char data) {
    // Esperar hasta que el buffer de transmisión esté vacío
    while (!(UCSR0A & (1<<UDRE0)));
    
    // Poner el dato en el buffer, envía el dato
    UDR0 = data;
}

int main(void) {
    cli(); // Deshabilitar interrupciones
    
    DDRB = 0x20; // Configura el pin 5 del puerto B como salida (LED)
    PORTD |= 0x04; // Activa la resistencia pull-up en el pin 2 del puerto D
    EICRA = 0b00001000; // Configura INT1 para disparar en flanco de bajada
    EIMSK = 2; // Habilita la interrupción INT1
    
    // Inicializar UART
    USART_Init(MYUBRR);
    
    sei(); // Habilitar interrupciones
    
    while(1) {
        // Bucle infinito vacío
    }
}

ISR(INT1_vect) {
    PINB = 0x20; // Cambia el estado del pin 5 del puerto B (LED)
    USART_Transmit('A'); // Transmite la letra 'A' por UART
    // Puedes cambiar 'A' por cualquier otro carácter ASCII
}