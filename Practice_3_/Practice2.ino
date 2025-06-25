#include <stdint.h>

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

// Definición de punteros a registros del Timer0 (en ATmega328P)
volatile reg8_t *my_TCCR0A = (reg8_t *)0x44;  // Registro de control A del Timer0
volatile reg8_t *my_TCCR0B = (reg8_t *)0x45;  // Registro de control B del Timer0
volatile reg8_t *my_OCR0A  = (reg8_t *)0x47;  // Registro de comparación A
volatile reg8_t *my_DDRD   = (reg8_t *)0x2A;  // Registro de dirección de datos D (OC0A = PD6)

int main() {
    // Configurar PD6 (OC0A) como salida (pin 6 en Arduino Uno)
    my_DDRD->bits.b6 = 1;

    // Configurar Timer0 en modo CTC (WGM02=0, WGM01=1, WGM00=0)
    my_TCCR0A->value = (1 << 1);  // Modo CTC (WGM01=1)
    my_TCCR0A->bits.b6 = 1;        // Toggle OC0A en comparación (COM0A0=1)

    // Asignar valor de OCR0A = 147
    my_OCR0A->value = 147;

    // Configurar prescaler = 1024 (CS02=1, CS01=0, CS00=1)
    my_TCCR0B->value = (1 << 2) | (1 << 0);

    while (1) {
        // Nada que hacer aquí, el Timer0 genera la señal automáticamente
    }

    return 0;
}