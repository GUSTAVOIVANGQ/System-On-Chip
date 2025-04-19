// Inicializar RAM[0] con el valor 3
@3
D=A
@0
M=D

// Cargar el valor de RAM[0] en el registro D
@0
D=M

// Inicializar el resultado en 0
@1
M=0

// Inicializar el contador en 0
@i
M=0

(LOOP)
    // Verificar si el contador ha alcanzado el valor de D
    @i
    D=M
    @0
    D=D-M
    @END
    D;JEQ

    // Sumar el valor de RAM[0] al resultado
    @0
    D=M
    @1
    M=M+D

    // Incrementar el contador
    @i
    M=M+1

    // Repetir el bucle
    @LOOP
    0;JMP

(END)
    // Fin del programa
    @END
    0;JMP