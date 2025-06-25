Creo que es ese

// PRENDER_DISPLAY CUANDO SE PRESIONE UNA TECLA PAR
(LOOP)
  @KBD
  D=M
  @R0
  M=D
  @1
  D=D&A
  @PAR
  D;JEQ  // Si el resultado es 0, salta a (PAR), si no, continúa

  @SCREEN
  M=0  // Apaga la pantalla
  @LOOP
  0;JMP

(PAR)
  @SCREEN
  D=A
  @I
  M=D  // I = 16384 (Inicio de la pantalla)

(LOOP2)
  @I
  A=M   // A = contenido de I (apunta a la dirección de pantalla)
  M=-1  // Enciende 16 píxeles en esa dirección

  @I
  D=M   // D = RAM[I] (valor actual de la dirección en I)
  @1
  D=D+A // D = D + 1 (avanzamos a la siguiente dirección)
  @I
  M=D   // Guardamos la nueva dirección en I

  @END_SCREEN
  D=A   // Cargamos la dirección final de la pantalla (24576)
  @I
  D=D-M // Restamos END_SCREEN - I
  @LOOP2
  D;JGT  // Si I < 24576, sigue el loop

(END)
  @LOOP
  0;JMP  // Vuelve a verificar el teclado