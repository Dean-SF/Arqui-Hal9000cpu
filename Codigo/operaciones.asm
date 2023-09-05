; ------------------------------------------------------------------
; Programa creado por Deyan Sanabaria Fallas con el objetivo de
; usar funciones creadas en NASM dentro de C, habran funciones para
; procesar instrucciones del lenguaje de programacion hecho
; para la simulacion de un procesador
; ------------------------------------------------------------------
segment .text

global nasmSumar
global nasmMover
global nasmCiclar
global nasmSacar
global nasmMeter
global nasmComparar
global nasmSaltar
global nasmSaltarI
global nasmSaltarNI
global nasmRotarI
global nasmRotarD
global nasmCorrerI
global nasmCorrerD
global nasmAND
global nasmDSF
global hex5Bits
global decToBin

; Funcion para hacer el procedimiento de suma del
; lenguaje del simulador de procesador
nasmSumar:
    enter 0,0
    push EBX ; Guardar registros importantes
    push EAX
    push EDX

    ; Procedimiento de suma
    mov EAX,[EBP+8]     ; EAX = &Operador1
    mov EBX,[EBP+12]    ; EBX = &Operador2
    mov EDX,[EBX]
    add [EAX],EDX

    ; Se mueve a EBX el valor de lo que hay en EAX
    ; para su uso eficiente
    mov EBX,[EAX]

    ; Comparaciones, detectan si el numero esta entre
    ; -16 y 15, esto para el overflow y las flags
sumComparacion1:
    cmp EBX,-16
    jge sumComparacion2     ; Primero se verifica que sea
    jmp SHORT sumNoEnRango  ; mayor o igual a -16

sumComparacion2:
    cmp EBX,15              ; Luego menor o igual a 15
    jle sumEstaEnRango      ; en ambos si no se cumple salta
    jmp SHORT sumNoEnRango  ; a "NoEnRango" pero si cumple
                            ; a "EstaEnRango"

    ; Ingreso de valores de flags:
    ; Overflow = 0, Carry = 0
sumEstaEnRango:
    mov EBX,[EBP+16]    ; EBX = &Overflow
    mov byte [EBX],0    ; Overflow = 0
    mov EBX,[EBP+20]    ; EBX = &Carry
    mov byte [EBX],0    ; Carry = 0
    jmp SHORT sumZeroSign

    ; Overflow = 1, Carry = 1
sumNoEnRango:
    mov EBX,[EBP+16]    ; EBX = &Overflow
    mov byte [EBX],1    ; Overflow = 0
    mov EBX,[EBP+20]    ; EBX = &Carry
    mov byte [EBX],1    ; Carry = 0

    ; Se le aplica una mascara para simular lo que
    ; pasaria si se llega al limite de bits
    mov EBX,[EAX]
    xor EBX,0xFFFFFFE0
    mov [EAX],EBX

    ; ZeroFlag y SignFlag
sumZeroSign:
    mov EBX,[EAX]   ; EBX = Operador1
    cmp EBX,0       ; Unica comparacion para saber si es
    je sumEsZero    ; 0, si es mayor y si es menor
    jmp SHORT sumNoZero

    ; ZeroFlag = 1
sumEsZero:
    mov EBX,[EBP+28]
    mov byte [EBX],1
    jmp SHORT sumEsPositivo

    ; ZeroFlag = 0
sumNoZero:
    mov EBX,[EBP+28]
    mov byte [EBX],0
    jg sumEsPositivo
    jl sumEsNegativo

    ; SignFlag = 0
sumEsPositivo:
    mov EBX,[EBP+24]
    mov byte [EBX],0
    jmp SHORT sumTerminar

    ; SignFlag = 1
sumEsNegativo:
    mov EBX,[EBP+24]
    mov byte [EBX],1

sumTerminar:
    pop EDX ; Restauracion de registros
    pop EAX
    pop EBX
    leave
    ret

; Funcion para hacer la operacion de mover valores de un lugar del CPU
; al otro del lenguaje creado para la simulacion de CPU
nasmMover:
    enter 0,0   
    push EAX    ; Guardar valores
    push EBX
    push EDX

    mov EAX,[EBP+8]     ; EAX = &Operador1
    mov EBX,[EBP+12]    ; EBX = &Operador2
    mov EDX,[EBX]

    ; Verificacion de que el dato a mover este en el rango deseado
moverComparacion1:
    cmp EDX,-16
    jge moverComparacion2    
    jmp SHORT moverFueraRango

moverComparacion2:
    cmp EDX,15              
    jle moverMovimiento      
    jmp SHORT moverFueraRango  
                            
moverFueraRango:
    ; Se le aplica una mascara para simular lo que
    ; pasaria si se llega al limite de bits
    xor EDX,0xFFFFFFE0

moverMovimiento:
    mov [EAX],EDX       ; Operador1 = Operador2

moverFin:
    pop EDX     ; Restaurar valores
    pop EBX
    pop EAX
    leave
    ret


; Funcion que opera los datos del simulador de CPU
; para lograr Hacer ciclos
nasmCiclar:
    enter 0,0
    push EAX    ; Guardar valores
    push EBX
    push EDX

    mov EAX,[EBP+8]     ; EAX = &operador1
    mov EBX,[EBP+12]    ; EBX = &Registro3
    mov EDX,[EBX]       ; EDX = Registro3

    cmp EDX,1
    jle cicloFin

    dec dword [EBX]
    mov EBX,[EBP+16]    ; EBX = &pCounter
    mov EDX,[EAX]       ; EDX = operador1
    dec EDX
    mov [EBX],EDX       ; pCounter = operador1

    mov EBX,[EBP+20]
    mov byte [EBX],1

cicloFin:
    pop EDX     ; Restaurar valores
    pop EBX
    pop EAX
    leave
    ret

; Funcion destinada a simular una pila para el CPU simulado
; su funcion es hacer push dentro de la pila
nasmMeter:
    enter 0,0
    push EAX    ; Guardar valores
    push EBX
    push EDX
    push ESI

    mov EAX,[EBP+8]     ; EAX = &pila
    mov EBX,[EBP+16]    ; EBX = &posPila
    mov ESI,[EBX]       ; ESI = posPila
    inc ESI
    cmp ESI,8
    jge meterFin

    mov EBX,[EBP+12]    ; EBX = &operador1
    mov EDX,[EBX]       ; EDX = operador1
    mov [EAX+ESI*4],EDX ; pila[ESI] = operador1

    mov EBX,[EBP+16]    ; EBX = &posPila
    mov [EBX],ESI       ; posPila++;

meterFin:
    pop ESI
    pop EDX     ; Restaurar valores
    pop EBX
    pop EAX
    leave
    ret

; Funcion para simular el funcionamiento de una pila
; saca un elemento de la pila
nasmSacar:
    enter 0,0
    push EAX    ; Guardar valores
    push EBX
    push EDX
    push ESI

    mov EAX,[EBP+8]     ; EAX = &pila
    mov EBX,[EBP+16]    ; EBX = &posPila
    mov ESI,[EBX]       ; ESI = posPila
    cmp ESI,0
    jl sacarFin
    mov EDX,[EAX+ESI*4]     ; EDX = pila[posPila]
    mov [EAX+ESI*4],dword 0 ; pila[posPila] = 0
    mov EAX,[EBP+12]        ; EAX = &operador1
    mov [EAX],EDX           ; operador1 = pila[posPila]
    dec dword [EBX]

sacarFin:
    pop ESI
    pop EDX     ; Restaurar valores
    pop EBX
    pop EAX
    leave
    ret

; Funcion para comparar dos elementos en el lenguaje del
; simulador de CPU
nasmComparar:
    enter 0,0
    push EAX    ; Guardar valores
    push EBX
    push EDX

    mov EAX,[EBP+8]     ; EAX = &operador1
    mov EBX,[EBP+12]    ; EBX = &operador2
    mov EDX,[EBX]       ; EDX = operador2

    cmp [EAX],EDX
    jne comNoZero

comEsZero:
    mov EBX,[EBP+16]
    mov byte [EBX],1
    jmp SHORT comPositivo

comNoZero:
    mov EBX,[EBP+16]
    mov byte [EBX],0

comSignFlag:
    jl comNegativo

comPositivo:
    mov EBX,[EBP+20]
    mov byte [EBX],0
    jmp SHORT comFin

comNegativo:
    mov EBX,[EBP+20]
    mov byte [EBX],1

comFin:
    pop EDX     ; Restaurar valores
    pop EBX
    pop EAX
    leave
    ret

; Funcion para simular un salto en el simulador de CPU
nasmSaltar:
    enter 0,0
    push EAX    ; Guardar valores
    push EBX
    push EDX

    mov EAX,[EBP+8]     ; EAX = &operador1
    mov EBX,[EBP+12]    ; EBX = &pCounter
    mov EDX,[EAX]
    mov [EBX],EDX       ; pCounter = operador1

    pop EDX     ; Restaurar valores
    pop EBX
    pop EAX
    leave
    ret

; Funcion para simular un salto condicional 
; en el simulador de CPU, en este caso, salta
; si la comparacion anterior es igual 
; (saltar si es igual)
nasmSaltarI:
    enter 0,0
    push EAX    ; Guardar valores
    push EBX
    push EDX

    mov EAX,[EBP+16]    ; EAX = &zeroFlag
    mov BL,[EAX]       ; EBX = zeroFlag
    cmp BL,0
    je sIFin

    mov EAX,[EBP+8]     ; EAX = &operador1
    mov EBX,[EBP+12]    ; EBX = &pCounter
    mov EDX,[EAX]
    mov [EBX],EDX       ; pCounter = operador1

    mov EAX,[EBP+20]    ; EAX = &reiniciar
    mov byte [EAX],1         ; reiniciar = 1

sIFin:
    pop EDX     ; Restaurar valores
    pop EBX
    pop EAX
    leave
    ret

; Funcion para simular un salto condicional 
; en el simulador de CPU, en este caso, salta
; si la comparacion anterior NO es igual 
; (saltar si NO es igual)
nasmSaltarNI:
    enter 0,0
    push EAX    ; Guardar valores
    push EBX
    push EDX

    mov EAX,[EBP+16]    ; EAX = &zeroFlag
    mov BL,[EAX]        ; EBX = zeroFlag
    cmp BL,1
    je sNIFin

    mov EAX,[EBP+8]     ; EAX = &operador1
    mov EBX,[EBP+12]    ; EBX = &pCounter
    mov EDX,[EAX]
    mov [EBX],EDX       ; pCounter = operador1

    mov EAX,[EBP+20]    ; EAX = &reiniciar
    mov byte [EAX],1         ; reiniciar = 1

sNIFin:
    pop EDX     ; Restaurar valores
    pop EBX
    pop EAX
    leave
    ret


; Funcion para simular una rotacion izquierda
; para la simulacion del procesador de 5 bits
nasmRotarI:
    enter 0,0
    push EAX    ; Guardar valores
    push EBX
    push EDX

    mov EAX,[EBP+8]     ; EAX = &operador1
    mov EBX,[EBP+12]    ; EBX = &carryFlag
    mov EDX,[EAX]       ; EDX = operador1

    shl EDX,1

    test EDX,0x20       ; se revisa el 6to bit si es 1 o 0
    jz rIColocar0       

rIColocar1:             ; si es 1 el 6to bit se cambia el primer bit
    xor EDX,0x1         ; que quedo como 0 a 1 (simular rotacion)
    mov byte [EBX],1    ; y en la carry flag se pone 1
    jmp SHORT rIRestoBits

rIColocar0:
    mov byte [EBX],0    ; si es 0 el 6to bit, como es un shift izquierdo
                        ; el primer bit ya es 0

   ; En esta seccion se compensa porque el CPU real maneja
   ; 32bits por ende, si el 5to bit es 1, el resto de bits
   ; del procesador real tienen que ser 1 para que de el
   ; resultado correcto a la hora de usar el numero                     
rIRestoBits:
    test EDX,0x10   ; Se revisa el 5to bit para hacer que el
    jz rIResto0     ; resto de bits despues de el sean
                    ; 0 o 1 con una mascara

rIResto1:
    or EDX,0xFFFFFFE0   ; Fuerza 1 en los bits despues del 5to
    jmp SHORT rIFin

rIResto0:
   and EDX,0x1F         ; Fuerza 0 en los bits despues del 5to 
    
rIFin:
    mov [EAX],EDX
    pop EDX     ; Restaurar valores
    pop EBX
    pop EAX
    leave
    ret


; Funcion para simular una rotacion derecha 
; para la simulacion del procesador de 5 bits
nasmRotarD:
    enter 0,0
    push EAX    ; Guardar valores
    push EBX
    push EDX

    mov EAX,[EBP+8]     ; EAX = &operador1
    mov EDX,[EAX]       ; EDX = operador1
    mov EBX,EDX         ; EBX = operador1

    shr EDX,1
    test EBX,0x1
    jz rDColocar0

rDColocar1:
    or EDX,0xFFFFFFF0   ; Fuerza 1 en los bits despues del 4to
    mov EBX,[EBP+12]    ; EBX = &carryFlag
    mov byte [EBX],1
    jmp rDFin

rDColocar0:
    and EDX,0xF         ; Fuerza 0 en los bits despues del 4to
    mov EBX,[EBP+12]    ; EBX = &carryFlag
    mov byte [EBX],0

rDFin:
    mov [EAX],EDX
    pop EDX     ; Restaurar valores
    pop EBX
    pop EAX
    leave
    ret

; Funcion para simular un corrimiento a la
; izquierda para un procesador de 5bits
nasmCorrerI:
    enter 0,0
    push EAX    ; Guardar valores
    push EBX
    push EDX

    mov EAX,[EBP+8]     ; EAX = &operador1
    mov EDX,[EAX]       ; EDX = operador1
    mov EBX,[EBP+12]    ; EBX = &carryFlag

    test EDX,0x10       ; Revisa el valor para colocar
    jz cICarry0         ; en carryFlag

cICarry1:
    mov byte [EBX],1        ; carryFlag = 1
    jmp SHORT cICorrimiento

cICarry0:
    mov byte [EBX],0        ; carryFlag = 0

cICorrimiento:
    shl EDX,1
    test EDX,0x10           ; ocurre el corrimiento y para
    jz cIColocar0           ; mantener constancia se fuerza el
                            ; resto de bits despues del 5to
                            ; a tener el mismo valor que el 5to

cIColocar1:
    or EDX,0xFFFFFFE0   ; Fuerza 1 en los bits despues del 5to
    jmp SHORT cIFin

cIColocar0:
    and EDX,0x1F         ; Fuerza 0 en los bits despues del 4to

cIFin:
    mov [EAX],EDX
    pop EDX     ; Restaurar valores
    pop EBX
    pop EAX
    leave
    ret

; Funcion para simular un corrimiento a la
; derecha para un procesador de 5bits
nasmCorrerD:
    enter 0,0
    push EAX    ; Guardar valores
    push EBX
    push EDX

    mov EAX,[EBP+8]     ; EAX = &operador1
    mov EDX,[EAX]       ; EDX = operador1
    mov EBX,[EBP+12]    ; EBX = &carryFlag

    test EDX,0x1
    jz cRCarry0

cRCarry1:
    mov byte [EBX],1    ; carryFlag = 1
    jmp SHORT cRCorrimiento

cRCarry0:
    mov byte [EBX],0    ; carryFlag = 0

cRCorrimiento:
    shr EDX,1
    and EDX,0xF ; Forzar a 0 despues del 4to bit

cRFin:
    mov [EAX],EDX
    pop EDX     ; Restaurar valores
    pop EBX
    pop EAX
    leave
    ret

; Funcion para simular la instruccion
; AND y TEST para procesador de 5 bits
nasmAND:
    enter 0,0
    push EAX    ; Guardar valores
    push EBX
    push EDX

    mov EAX,[EBP+8]     ; EAX = &operador1
    mov EBX,[EBP+12]    ; EBX = &operador2
    mov EDX,[EBX]       ; EDX = operador2 (mask)
    mov EBX,EDX         ; EBX = operador2 (mask)
    mov EDX,[EAX]       ; EDX = operador1 (aplicar la mask)
    and EDX,EBX         ; aplicacion de mask

    ; Set de flag:
    cmp EDX,0
    jne andFlag0       

andFlag1:   ; Si es 0 despues de la mask la zeroFlag sera 1
    mov EBX,[EBP+16]    ; EBX = &zeroFlag
    mov byte [EBX],1
    jmp SHORT andOrTest

andFlag0:   ; Si no es 0 despues de la mask la zeroFlag sera 0
    mov EBX,[EBP+16]    ; EBX = &zeroFlag
    mov byte [EBX],0

andOrTest:              ; Comprueba si el ultimo parametro es
    mov EBX,[EBP+20]    ; 0 para hacer la operacion de AND
    cmp EBX,1           ; o 1 para la operacion de TEST
    je andFin

and:
    mov [EAX],EDX

andFin:
    pop EDX     ; Restaurar valores
    pop EBX
    pop EAX
    leave
    ret


; Funcion especial para elevar al
; cuadrado un numero para la simulacion
; de un procesador de 5 bits, con sus
; limitantes
nasmDSF:
    enter 0,0
    push EAX    ; Guardar valores
    push EBX
    push EDX
    push ECX

    mov EAX,[EBP+8] ; EAX = &operador1
    mov EBX,[EAX]   ; EBX = operador1

    cmp EBX,0
    jge dsfNoNegar

dsfNegar:
    neg EBX

dsfNoNegar:
    mov EDX,EBX     ; EDX = operador1
    mov ECX,EBX     ; ECX = operador1

dsfCasosBase:   ; si es 1 o 0 queda igual el numero
    je dsfFin
    cmp EBX,1
    je dsfFin

    dec ECX     ; se le decrementa 1 para que haga la cantidad correcta
                ; de ciclos
dsfCiclo:
    add EBX,EDX     ; Elevacion al cuadrado apartir
    loop dsfCiclo   ; de sumas consecutivas

dsfComparacion1:    ; Revision de si esta fuera del rango de
    cmp EBX,-16     ; 5 bits
    jl dsfNoRango

dsfComparacion2:
    cmp EBX,15
    jg dsfNoRango

dsfEnRango:
    mov EDX,[EBP+12]    ; Si esta en rango no hay overflow y
    mov byte [EDX],0         ; no hay problemas
    jmp dsfFin

dsfNoRango:
    mov EDX,[EBP+12]    ; Si esta fuera de rango
    mov byte [EDX],1         ; se coloca el overflow en la flag
    test EBX,0x10       ; si se simula un fuera de rango
    jz dsfColocar0

dsfColocar1:
    or EBX,0xFFFFFFE0
    jmp dsfFin

dsfColocar0:
    and EBX,0x1F
    
dsfFin:
    mov [EAX],EBX
    pop ECX     ; Restaurar valores
    pop EDX     
    pop EBX
    pop EAX
    leave
    ret


; Funcion que aplica una mascara de 0x1F
; en el primer parametro y lo retorna
; en el segundo parametro
hex5Bits:
    enter 0,0
    push EAX    ; Guardar valores
    push EBX
    push EDX

    mov EAX,[EBP+8] ; EAX = &numero
    mov EBX,[EAX]   ; EAX = numero
    and EBX,0x1F    ; Mascara
    mov EAX,[EBP+12] ; EAX = &retorno
    mov [EAX],EBX

    pop EDX     ; Restaurar valores 
    pop EBX
    pop EAX
    leave
    ret

; Funcion que retorna el valor binario guardado
; en una variable, a un string
decToBin:
    enter 0,0
    push EAX    ; Guardar valores
    push EBX
    push EDX

    mov EAX,[EBP+8]     ; EAX = &variable
    mov EBX,[EAX]       ; EBX = variable
    mov EAX,EBX         ; EAX = variable
    mov EBX,[EBP+12]    ; EBX = char[]

    mov ECX,5
binCiclo:
    test EAX,0x10
    jz binColocar0

binColocar1:
    mov byte [EBX],49
    jmp SHORT binCiclar

binColocar0:
    mov byte [EBX],48

binCiclar:
    inc EBX
    shl EAX,1
    loop binCiclo
    mov byte [EBX],0

    pop EDX     ; Restaurar valores 
    pop EBX
    pop EAX
    leave
    ret