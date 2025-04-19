// mov R0, #3
@3
D=A
@R0
M=D

// mov R1, #2
@2
D=A
@R1
M=D

// i = 0      (dirección 16)
@i 
M=0

// acc = 0    (dirección 17)
@acc
M=0

(LOOP)
    // Condición de paro i >= R0
    @R0
    D=M
    @i
    D=M-D
    @FIN
    D;JGE

    // acc = acc + R1
    @R1
    D=M
    @acc
    M=M+D

    // i = i + 1
    @i
    M=M+1

    @LOOP
    0;JMP

(FIN)
    // Guardar el resultado en R2
    @acc
    D=M
    @R2
    M=D

// Evitar que se ejecute el resto del contenido en memoria
@HALT
(HALT)
0;JMP
