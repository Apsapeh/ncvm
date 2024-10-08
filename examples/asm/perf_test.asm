.one_d <double> (
    1.0
)
.four_d <double> (
    4.0
)

stop:
    STOP

main:
    ISR R0 100000002   ; Количество итераций
    ISR R1 2           ; int i = 2; 
    DSMLD R0 #one_d    ; double x = 1.0;
    DSMLD R1 #one_d    ; double pi = 1.0;

    ;ISR R2 1
    ISR R4 2


    while_start:
        IJEG R1 R0 while_end       ;

        DNEG R0 R0 ; x = -x
   
        ILSI R3 R1 1   ; i * 2
        IDEC R3        ; i * 2 - 1
        ITOD R3 R3     ; (double)(i * 2 - 1)
        DDIV R3 R0 R3 ; x / (i * 2 - 1)
        DADD R1 R1 R3 ; pi += x / (i * 2 - 1)
        
        IINC R1
        JMP while_start
    while_end:
    DSMLD R2 #four_d
    DMULT R1 R2 R1
    LIBCALL print_pi
    STOP                ; Остановка программы

;post:
;    DSMLD R1 #four_d
;    LIBCALL print_pi
;    RET
