#include <stdint.h>
#include <util/delay.h>

// Definición de registros usando uniones y campos de bits
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

// Dirección I2C del LCD (comúnmente 0x27 o 0x3F)
#define LCD_ADDRESS 0x27

// Configuración de pines del PCF8574 (adaptar según tu módulo)
#define LCD_RS 0
#define LCD_RW 1
#define LCD_EN 2
#define LCD_BACKLIGHT 3
#define LCD_D4 4
#define LCD_D5 5
#define LCD_D6 6
#define LCD_D7 7

// Definición de punteros a registros del ATmega328P (Arduino Uno)
volatile reg8_t *my_UCSR0A = (reg8_t *)0xC0;  // USART Control and Status Register A
volatile reg8_t *my_UCSR0B = (reg8_t *)0xC1;  // USART Control and Status Register B
volatile reg8_t *my_UCSR0C = (reg8_t *)0xC2;  // USART Control and Status Register C
volatile reg8_t *my_UBRR0H = (reg8_t *)0xC5;  // USART Baud Rate Register High
volatile reg8_t *my_UBRR0L = (reg8_t *)0xC4;  // USART Baud Rate Register Low
volatile reg8_t *my_UDR0   = (reg8_t *)0xC6;  // USART I/O Data Register

volatile reg8_t *my_TWBR   = (reg8_t *)0xB8;  // TWI Bit Rate Register
volatile reg8_t *my_TWSR   = (reg8_t *)0xB9;  // TWI Status Register
volatile reg8_t *my_TWAR   = (reg8_t *)0xBA;  // TWI Address Register
volatile reg8_t *my_TWDR   = (reg8_t *)0xBB;  // TWI Data Register
volatile reg8_t *my_TWCR   = (reg8_t *)0xBC;  // TWI Control Register

// Prototipos de funciones
void I2C_Init();
uint8_t I2C_Start(uint8_t address);
uint8_t I2C_Write(uint8_t data);
void I2C_Stop();
void LCD_Init();
void LCD_SendCommand(uint8_t cmd);
void LCD_SendData(uint8_t data);
void LCD_WriteString(const char *str);
void USART_Init(uint16_t baud);
void USART_Transmit(char data);

int main(void) {
    // Inicializar USART para 9600 bauds (16MHz)
    USART_Init(103);
    
    // Mensaje de inicio por USART
    const char *init_msg = "\nSistema iniciando...\n";
    for (int i = 0; init_msg[i] != '\0'; i++) {
        USART_Transmit(init_msg[i]);
    }
    
    // Inicializar I2C
    I2C_Init();
    _delay_ms(100); // Esperar estabilización
    
    // Inicializar LCD
    LCD_Init();
    
    // Mensaje inicial en LCD
    LCD_SendCommand(0x80); // Primera línea
    LCD_WriteString("Hola Arduino!");
    LCD_SendCommand(0xC0); // Segunda línea
    LCD_WriteString("I2C LCD Funciona");
    
    // Confirmación por USART
    const char *ready_msg = "LCD inicializado\n>";
    for (int i = 0; ready_msg[i] != '\0'; i++) {
        USART_Transmit(ready_msg[i]);
    }
    
    while (1) {
        if (my_UCSR0A->bits.b7) { // Check if data is received (RXC0)
            char received = my_UDR0->value;   // Read received data
            
            // Filter out non-printable characters (ASCII < 32, e.g., \n, \r)
            if (received >= 32 && received <= 126) { // Printable ASCII range
                // Echo back via USART
                const char *echo_msg = "Recibido: ";
                for (int i = 0; echo_msg[i] != '\0'; i++) {
                    USART_Transmit(echo_msg[i]);
                }
                USART_Transmit(received);
                USART_Transmit('\n');
                
                // Display on LCD
                LCD_SendCommand(0x01); // Clear display
                _delay_ms(2); // Wait for clear
                LCD_SendCommand(0x80); // First line
                LCD_WriteString("Serial funciona");
                
                // Show received character on second line
                LCD_SendCommand(0xC0); // Second line
                LCD_SendData(received); // Display the character
                LCD_SendData(' '); // Add space for readability
                
                // Show ASCII value
                char asciiMsg[16];
                sprintf(asciiMsg, "ASCII: %d", (int)received);
                LCD_WriteString(asciiMsg);
            }
        }
    }
    
    return 0; // Never reached
}

// Inicialización I2C
void I2C_Init() {
    my_TWSR->value = 0x00; // Prescaler 1
    my_TWBR->value = 0x0C; // 100kHz para 16MHz
    my_TWCR->value = (1 << 2); // Habilitar TWI (TWEN)
}

// Inicio comunicación I2C
uint8_t I2C_Start(uint8_t address) {
    my_TWCR->value = (1 << 7) | (1 << 5) | (1 << 2); // TWINT | TWSTA | TWEN
    while (!(my_TWCR->bits.b7));
    
    if ((my_TWSR->value & 0xF8) != 0x08) return 0; // Error
    
    my_TWDR->value = address;
    my_TWCR->value = (1 << 7) | (1 << 2); // TWINT | TWEN
    while (!(my_TWCR->bits.b7));
    
    return (my_TWSR->value & 0xF8) == 0x18;
}

// Escritura I2C
uint8_t I2C_Write(uint8_t data) {
    my_TWDR->value = data;
    my_TWCR->value = (1 << 7) | (1 << 2); // TWINT | TWEN
    while (!(my_TWCR->bits.b7));
    return (my_TWSR->value & 0xF8) == 0x28;
}

// Detener I2C
void I2C_Stop() {
    my_TWCR->value = (1 << 7) | (1 << 4) | (1 << 2); // TWINT | TWSTO | TWEN
    while (my_TWCR->bits.b4);
}

// Inicialización LCD
void LCD_Init() {
    _delay_ms(50); // Esperar después de encender
    
    // Secuencia de inicialización en 4 bits
    uint8_t init_sequence[] = {0x30, 0x30, 0x30, 0x20, 0x28, 0x0C, 0x06, 0x01};
    
    for (uint8_t i = 0; i < sizeof(init_sequence); i++) {
        LCD_SendCommand(init_sequence[i]);
        _delay_ms(5); // Esperar entre comandos
    }
    _delay_ms(2); // Esperar clear display
}

// Enviar comando al LCD
void LCD_SendCommand(uint8_t cmd) {
    uint8_t high_nibble = cmd & 0xF0;
    uint8_t low_nibble = (cmd << 4) & 0xF0;
    
    // Enviar nibble alto
    I2C_Start(LCD_ADDRESS << 1);
    I2C_Write(high_nibble | (1 << LCD_EN) | (1 << LCD_BACKLIGHT));
    _delay_us(1);
    I2C_Write(high_nibble | (1 << LCD_BACKLIGHT));
    _delay_us(100);
    I2C_Stop();
    
    // Enviar nibble bajo
    I2C_Start(LCD_ADDRESS << 1);
    I2C_Write(low_nibble | (1 << LCD_EN) | (1 << LCD_BACKLIGHT));
    _delay_us(1);
    I2C_Write(low_nibble | (1 << LCD_BACKLIGHT));
    _delay_us(100);
    I2C_Stop();
}

// Enviar dato al LCD
void LCD_SendData(uint8_t data) {
    uint8_t high_nibble = data & 0xF0;
    uint8_t low_nibble = (data << 4) & 0xF0;
    
    // Enviar nibble alto
    I2C_Start(LCD_ADDRESS << 1);
    I2C_Write(high_nibble | (1 << LCD_RS) | (1 << LCD_EN) | (1 << LCD_BACKLIGHT));
    _delay_us(1);
    I2C_Write(high_nibble | (1 << LCD_RS) | (1 << LCD_BACKLIGHT));
    _delay_us(100);
    I2C_Stop();
    
    // Enviar nibble bajo
    I2C_Start(LCD_ADDRESS << 1);
    I2C_Write(low_nibble | (1 << LCD_RS) | (1 << LCD_EN) | (1 << LCD_BACKLIGHT));
    _delay_us(1);
    I2C_Write(low_nibble | (1 << LCD_RS) | (1 << LCD_BACKLIGHT));
    _delay_us(100);
    I2C_Stop();
}

// Escribir string en LCD
void LCD_WriteString(const char *str) {
    while (*str) {
        LCD_SendData(*str++);
    }
}

// Inicialización USART
void USART_Init(uint16_t baud) {
    my_UBRR0H->value = (uint8_t)(baud >> 8);
    my_UBRR0L->value = (uint8_t)baud;
    my_UCSR0B->value = (1 << 4) | (1 << 3); // RXEN0 | TXEN0
    my_UCSR0C->value = (1 << 2) | (1 << 1); // UCSZ01 | UCSZ00
}

// Transmitir por USART
void USART_Transmit(char data) {
    while (!(my_UCSR0A->bits.b5)); // Esperar UDRE0
    my_UDR0->value = data;
}