#include <stdint.h>
#include <util/delay.h>

// Definición de uniones y estructuras para acceso a bits
typedef struct {
    uint8_t b0 : 1;
    uint8_t b1 : 1;
    uint8_t b2 : 1;
    uint8_t b3 : 1;
    uint8_t b4 : 1;
    uint8_t b5 : 1;
    uint8_t b6 : 1;
    uint8_t b7 : 1;
} bitfield_t;

typedef union {
    bitfield_t bits;
    uint8_t value;
} reg8_t;

// Definición de punteros a registros USART
volatile reg8_t *my_UDR0   = (reg8_t *)0xC6;
volatile reg8_t *my_UCSR0A = (reg8_t *)0xC0;
volatile reg8_t *my_UCSR0B = (reg8_t *)0xC1;
volatile reg8_t *my_UCSR0C = (reg8_t *)0xC2;
volatile reg8_t *my_UBRR0  = (reg8_t *)0xC4;

// Definición de punteros a registros ADC
volatile reg8_t *my_ADMUX  = (reg8_t *)0x7C;
volatile reg8_t *my_ADCSRA = (reg8_t *)0x7A;
volatile reg8_t *my_ADCL   = (reg8_t *)0x78;
volatile reg8_t *my_ADCH   = (reg8_t *)0x79;
volatile reg8_t *my_DIDR0  = (reg8_t *)0x7E;

void USART_Init() {
    // UBRR0 = 103; // 9600 baudios para 16MHz
    my_UBRR0->value = 103;
    
    // UCSR0B = (1 << TXEN0); // Habilita transmisor
    my_UCSR0B->value = (1 << 3);
    
    // UCSR0C = (1 << UCSZ01) | (1 << UCSZ00); // 8 bits, sin paridad, 1 stop bit
    my_UCSR0C->value = (1 << 1) | (1 << 2);
}

void USART_Transmit(uint8_t data) {
    // while (!(UCSR0A & (1 << UDRE0))); // Espera hasta que el buffer esté vacío
    while (!(my_UCSR0A->bits.b5));
    
    // UDR0 = data;
    my_UDR0->value = data;
}

void ADC_Init() {
    // ADMUX = (1 << REFS0); // AVcc como referencia
    my_ADMUX->value = (1 << 6);
    
    // ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); // ADC enable, prescaler 128
    my_ADCSRA->value = (1 << 7) | (1 << 2) | (1 << 1) | (1 << 0);
}

uint16_t ADC_Read(uint8_t channel) {
    // ADMUX = (ADMUX & 0xF8) | (channel & 0x07); // Selecciona canal
    my_ADMUX->value = (my_ADMUX->value & 0xF8) | (channel & 0x07);
    
    // ADCSRA |= (1 << ADSC); // Inicia conversión
    my_ADCSRA->bits.b6 = 1;
    
    // while (ADCSRA & (1 << ADSC)); // Espera fin de conversión
    while (my_ADCSRA->bits.b6);
    
    // return ADC;
    return (my_ADCH->value << 8) | my_ADCL->value;
}

void printNumber(uint16_t num) {
    char buffer[5];
    uint8_t i = 0;
    
    if (num == 0) {
        USART_Transmit('0');
        return;
    }
    
    while (num > 0) {
        buffer[i++] = (num % 10) + '0';
        num /= 10;
    }
    
    while (i > 0) {
        USART_Transmit(buffer[--i]);
    }
}

int main(void) {
    USART_Init();
    ADC_Init();
    
    // Deshabilitar entrada digital en A0
    // DIDR0 = (1 << ADC0D);
    my_DIDR0->bits.b0 = 1;
    
    while(1) {
        uint16_t adcValue = ADC_Read(0); // Leer ADC0 (A0)
        
        // Enviar valor ADC
        printNumber(adcValue);
        USART_Transmit('\t');
        
        // Enviar voltaje (formato X.XX V)
        uint16_t voltage = (adcValue * 5000UL) / 1023;
        printNumber(voltage / 1000);
        USART_Transmit('.');
        printNumber((voltage % 1000) / 100);
        printNumber((voltage % 100) / 10);
        USART_Transmit('V');
        USART_Transmit('\r');
        USART_Transmit('\n');
        
        _delay_ms(200);
    }
}