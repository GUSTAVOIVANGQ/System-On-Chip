#include <stdint.h>
#define SALIDA 1
#define Entrada 0

typedef struct {
    uint8_t b0 : 1;  // Usar uint8_t para bits individuales
    uint8_t b1 : 1;
    uint8_t b2 : 1;
    uint8_t b3 : 1;
    uint8_t b4 : 1;
    uint8_t b5 : 1;
    uint8_t b6 : 1;
    uint8_t b7 : 1;
} DDRx_t, PINx_t, PORTx_t;

// Definir PORTx_t usando la misma estructura que DDRx_t
typedef DDRx_t PORTx_t ;  // Corrección de la sintaxis
typedef DDRx_t PINx_t;

int main() {
    // Para acceder a registros específicos en memoria
    volatile DDRx_t* DireccionB = (DDRx_t*)0x24;  // Dirección de DDRB
    volatile PORTx_t* SalidaB = (PORTx_t*)0x25;  // Dirección de PORTB
    volatile DDRx_t* DireccionD = (DDRx_t*)0x2A;  // Dirección de PORTB
    volatile PINx_t* EntradaD = (PINx_t*)0x29;  // Dirección de PORTB
    DireccionB->b5 = SALIDA; 
    DireccionD->b3 = Entrada; 
    
    while(1) {
      SalidaB -> b5; EntradaD->b3;
    }
    
    return 0;
}