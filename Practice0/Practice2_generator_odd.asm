(LOOP)
@KBD
D=M
@R0
M=D
@1
D=D&A
@PAR
D;JEQ
@SCREEN
M=0
@LOOP
0;JMP
(PAR)
@SCREEN
M=-1
@LOOP
0;JMP