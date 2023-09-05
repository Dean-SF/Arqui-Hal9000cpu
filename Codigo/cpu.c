/*
Creado por Deyan Sanabria Fallas
Este archivo contiene las funciones y variables necesarias para
administrar, leer y ejecutar un archivo con instrucciones de un
lenguaje artifical creado por el autor
*/

#include <stdio.h>
#include <unistd.h>
#include <ncurses.h>
#define MAX 256
#define AMARILLO 1
#define ROJO 2
#define AZUL 3
#define VERDE 4
#define MORADO 5
#define CYAN 6

// Funciones ubicadas en NASM
extern void nasmSumar(int*,int*,char*,char*,char*,char*);
extern void nasmMover(int*,int*);
extern void nasmCiclar(int*,int*,int*,char*);
extern void nasmMeter(int[],int*,int*);
extern void nasmSacar(int[],int*,int*);
extern void nasmComparar(int*,int*,char*,char*);
extern void nasmSaltar(int*,int*);
extern void nasmSaltarI(int*,int*,char*,char*);
extern void nasmSaltarNI(int*,int*,char*,char*);
extern void nasmRotarI(int*,char*);
extern void nasmRotarD(int*,char*);
extern void nasmCorrerI(int*,char*);
extern void nasmCorrerD(int*,char*);
extern void nasmAND(int*,int*,char*,int);
extern void nasmDSF(int*,char*);
extern void hex5Bits(int*,int*);
extern void decToBin(int*,char[]);

// Variables locales encontradas
FILE *archivo;
int max_x,max_y;
int memoria[16];
int pila[8];
int posPila = -1;
char variables[8][11];
char etiquetas[8][11];
int etiquetasPos[8];
int pCounter = 0;
int iRegister = 0;
char overflow = 0;
char zeroFlag = 0;
char signFlag = 0;
char carryFlag = 0;
int registro1 = 0;
int registro2 = 0;
int registro3 = 0;
int registro4 = 0;

/*
funcion para inicializar el CPU
*/
void initCPU() {
    posPila = -1;
    pCounter = 0;
    iRegister = 0;
    overflow = 0;
    zeroFlag = 0;
    signFlag = 0;
    carryFlag = 0;
    registro1 = 0;
    registro2 = 0;
    registro3 = 0;
    registro4 = 0;
}

/*
funcion para señalizar el estado del ciclo de fetch
*/
void fetch(int ciclo) {
    usleep(500000);
    char binNumber[6];
    if(ciclo == 1) {
        attron(COLOR_PAIR(CYAN));
        mvprintw(3,max_x/2 - 74,"Registro PC: ");
        attroff(COLOR_PAIR(CYAN));
        attron(COLOR_PAIR(MORADO));
        decToBin(&pCounter,binNumber);
        printw("%s",binNumber);
        attroff(COLOR_PAIR(MORADO));
        move(6,max_x/2 - 5);
        attron(COLOR_PAIR(ROJO));
        printw("Fetch, ");
        attroff(COLOR_PAIR(ROJO));
        printw("Decode, ");
        printw("Execute");
    } else if(ciclo == 2) {
        attron(COLOR_PAIR(CYAN));
        mvprintw(4,max_x/2 - 74,"Registro IR: ");
        attroff(COLOR_PAIR(CYAN));
        attron(COLOR_PAIR(MORADO));
        decToBin(&iRegister,binNumber);
        printw("%s",binNumber);
        attroff(COLOR_PAIR(MORADO));
        move(6,max_x/2 - 5);
        printw("Fetch, ");
        attron(COLOR_PAIR(ROJO));
        printw("Decode, ");
        attroff(COLOR_PAIR(ROJO));
        printw("Execute");
    } else if(ciclo == 3) {
        move(6,max_x/2 - 5);
        printw("Fetch, ");
        printw("Decode, ");
        attron(COLOR_PAIR(ROJO));
        printw("Execute");
        attroff(COLOR_PAIR(ROJO));
    }
    refresh();
}

/*
Funcion para abrir uno de los tres archivos conteniendo el programa
a ejecutars
*/
char abrirPrograma(int num) {
    char *direccion;
    switch(num) {
        case 1:
            direccion = "prog1.asf";
            break;
        case 2:
            direccion = "prog2.asf";
            break;
        case 3:
            direccion = "prog3.asf";
            break;
        case 4:
            direccion = "progcustom.asf";
            break;
        default:
            return 0;
    }
    archivo = fopen(direccion, "r");
    if(archivo == NULL)
        return 0;
    return 1;
}

void cerrarArchivo() {
    fclose(archivo);
}

/*
Funcion de comparacion de strings, recibe dos arrays de char y sus
tamaños. Esto debido a que al pasar un puntero a una funcion, se pierde
los datos del tamaño y no puede ser recuperado con "sizeof()"
*/
char comparar(char str1[],char str2[],int size1,int size2) {
    if(size1 != size2)
        return 0;
    for(int i = 0; i < size1; i++) { // compara cada caracter
        if(str1[i] != str2[i])
            return 0;
        if(str1[i] == 0 && str2[i] == 0)
            return 1;
    }
    return 0;
}

/*
Funcion auxiliar que interpreta un numero en un string
a un numero normal
*/
int stringNumero(char string[], int size) {
    int numero = 0;
    char negativo = 0;
    for(int i = 0; i < size; i++) {
        if(string[i] == 0) {
            break;
        }
        if(string[i] == '-') {
            negativo = 1;
            continue;
        }
        numero *= 10;
        numero += (string[i] - 48);
    }
    if(negativo) {
        numero *= -1;
    }
    return numero;
}

/*
funcion auxiliar para guardar en memoria, contiene todas las verificaciones
para asociar un "neumonico" con su "opcode"
*/
char instrucciones(int memoria[], char operacion[], int celda) {
    if(comparar(operacion,"sumar",6,6)) {
        memoria[celda] = 0;
        return 1;
    } if(comparar(operacion,"mover",6,6)) {
        memoria[celda] = 1;
        return 1;
    } if(comparar(operacion,"ciclar",7,7)) {
        memoria[celda] = 2;
        return 1;
    } if(comparar(operacion,"meter",6,6)) {
        memoria[celda] = 3;
        return 1;
    } if(comparar(operacion,"sacar",6,6)) {
        memoria[celda] = 4;
        return 1;
    } if(comparar(operacion,"comparar",9,9)) {
        memoria[celda] = 5;
        return 1;
    } if(comparar(operacion,"rotarI",7,7)) {
        memoria[celda] = 6;
        return 1;
    } if(comparar(operacion,"rotarD",7,7)) {
        memoria[celda] = 7;
        return 1;
    } if(comparar(operacion,"correrI",8,8)) {
        memoria[celda] = 8;
        return 1;
    } if(comparar(operacion,"correrD",8,8)) {
        memoria[celda] = 9;
        return 1;
    } if(comparar(operacion,"test",5,5)) {
        memoria[celda] = 10;
        return 1;
    } if(comparar(operacion,"and",4,4)) {
        memoria[celda] = 11;
        return 1;
    } if(comparar(operacion,"saltar",7,7)) {
        memoria[celda] = 12;
        return 1;
    } if(comparar(operacion,"saltarI",8,8)) {
        memoria[celda] = 13;
        return 1;
    } if(comparar(operacion,"saltarNI",9,9)) {
        memoria[celda] = 14;
        return 1;
    } if(comparar(operacion,"DSF",4,4)) {
        memoria[celda] = 15;
        return 1;
    } if(comparar(operacion,"entrada",8,8)) {
        memoria[celda] = 16;
        return 1;
    } if(comparar(operacion,"salida",7,7)) {
        memoria[celda] = 17;
        return 1;
    }
    return 0;
}

/*
Funcion auxiliar que ayuda a obtener la siguiente palabra despues 
de un espacio
*/
int siguientePalabra(char linea[], char almacen[], int inicio) {
    int espacio = 0;
    int retorno;
    for(int i = inicio; i < MAX; i++) {
        retorno = i;
        if(linea[i] != ' ' && linea[i] != '\n') {
            almacen[espacio] = linea[i];
            espacio++;
        } else if (espacio > 0) 
            break;
    }
    almacen[espacio] = 0;
    return retorno;
}

/*
Funcion auxiliar para guardar en un arreglo bidimensional strings
*/
void guardarString(char almacen[][11], char string[], int tamaño,int pos) {
    for(int i = 0; i < tamaño; i++) {
        almacen[pos][i] = string[i];
    }
}

/*
Funcion auxiliar para verificar si algo es una etiqueta
*/
char esEtiqueta(char string[], int tamaño){
    for(int i = 0; i < tamaño; i++) {
        if(string[i] == 0) {
            if(string[i-1] == ':') {
                return 1;
            }
        }
    }
    return 0;
}

// Funcion que recibe una linea leida del archivo
// y revisa si es una linea vacia
char esLineaVacia(char linea[]) {
    for(int i = 0; i < MAX; i++) {
        if(linea[i] == 0)
            return 1;
        if(linea[i] != ' ' && linea[i] != '\n') {
            return 0;
        }
    }
    return 0;
}

/*
Funcion para "guardar en memoria" datos del programa
*/
char guardarEnMemoria() {
    // variables necesarias para el funcionamiento del programa
    char linea[MAX];
    char palabra[11];
    char inicio = 0;
    char datos = 0;
    int variable = 0;
    int celda = 0;
    int etiqueta = 0;
    int ultimaPos;
    int ultimaLinea = -1;

    // El ciclo se mantiene hasta que se llega al EOF
    while (fgets(linea,MAX,archivo)) {
        ultimaLinea++; 
        ultimaPos = siguientePalabra(linea,palabra,0); // se obtiene una palabra de la linea leida

        // Par de ifs para verificar si se esta en el segmento de datos
        // o en la parte del codigo
        if(comparar(palabra,"::Datos",8,8)) {       
            datos = 1;
            continue;
        } else if(comparar(palabra,"-INICIO",8,8)) {
            datos = 0;
            inicio = 1;
            continue;
        }

        // Si se llena la memoria retorna la funcion
        if(celda == 16 || variable == 8 || etiqueta == 8)
            return 0;
        
        // Si se esta en el segmento de datos
        if(datos) {
            if(esLineaVacia(linea)) // se revisa por lineas vacias para saltarlas
                continue;
            guardarString(variables,palabra,11,variable); // se guarda nombre de variables
            variable++;
            siguientePalabra(linea,palabra,ultimaPos);  // y se busca por el valor a guardar
            memoria[celda] = stringNumero(palabra,11);
            celda++;
        }

        // Si se esta en el segmento de codigo
        if(inicio) {
            if(instrucciones(memoria,palabra,celda)) // Comprueba que lo leido es una instruccion
                celda++;
            else if (esEtiqueta(palabra,11)) {      // Comprueba si es una etiqueta
                guardarString(etiquetas,palabra,11,etiqueta); // Si lo es la guarda junto
                etiquetasPos[etiqueta] = ultimaLinea;               // a su posicion en el codio
                etiqueta++;
            } else if(comparar(palabra,"-FIN",5,5)) 
                return 1;                           // La lectura del programa solo es exitosa si
        }                                           // encuentra el final del codio
    }
    return 0;
}

// Funcion que separa dos operadores separados por comas, tambien
// sirve para encontrar solo el primer operador
char obtenerOperadores(char op1[],char op2[],char operadores[]) {
    // variables locales
    char *temp = op1;
    int charPos = 0;
    char retorno = 0;

    // busca por el string donde se encuentran los operadores
    for(int i = 0; i < 22; i++) {
        if(operadores[i] == ',') { // si encuentra una coma, cambia al
            temp[charPos] = 0;     // segundo operador y reinicia
            temp = op2;            // la posicion del string del operador
            charPos = 0;
            retorno = 1;
            continue;
        }
        if(operadores[i] == 0 || charPos == ' ') {
            temp[charPos] = 0;   // este if busca por el final
            charPos++;           // del string del operador
            break;
        }
        temp[charPos] = operadores[i];
        charPos++;
    
    }
    return retorno;
}

// Funcion que retorna un registro si el parametro se
// refiere a uno o NULL si este no es un registro
int *esRegistro(char operador[]) {
    int *temp = NULL;

    // Compara los strings con el parametro
    // si coincide con alguno asigna la direccion
    // de memoria del registro, de lo contrario
    // retorna NULL
    if(comparar(operador,"R1",3,3)) {
        temp = &registro1;
        return temp;
    } if(comparar(operador,"R2",3,3)) {
        temp = &registro2;
        return temp;
    } if(comparar(operador,"R3",3,3)) {
        temp = &registro3;
        return temp;
    } if(comparar(operador,"R4",3,3)) {
        temp = &registro4;
        return temp;
    }
    return temp;
}

// Funcion para obtener la direccion de memoria
// de una variable en la "memoria principal"
int *obtenerDirMem(char operador[]) {
    int *temp = NULL; // Se almacena la direccion de memoria si se encuentra
    for(int i = 0; i < 8; i++) {
        if(comparar(variables[i],operador,11,11)) {
            temp = &(memoria[i]);  // se busca por todas las variables si coincide
        }                          // con el parametro, se asigna a temp la direccion
    }                              // de memoria y se retorna
    return temp;
}

// Funcion para obtener la linea a la que pertenece
// una etiqueta
int *obtenerLineaEjecucion(char operador[]) {

    // Variable donde se guarda la linea de la etiqueta
    int *temp = NULL;

    // Ciclo for para buscar en el arreglo de etiquetas si estas
    // si lo que se recibe de parametro es una etiqueta
    for(int i = 0; i < 8; i++) {
        // se compara la entrada con las etiquetas del arreglo
        if(comparar(etiquetas[i],operador,11,11)) {
            temp = &etiquetasPos[i];    // Si la encuentra asigna a temp
            break;                      // la direccion de memoria donde
        }                               // esta el numero de linea a saltar
    }
    return temp;    
}

// Funcion para obtener un string dentro del lenguaje creado
char obtenerString(char almacen[],char linea[]) {

    // Variables
    char comilla1 = 0;
    char retorno = 0;
    int posAlmacen = 0;

    // Ciclo for para recorrer toda la linea leida
    for(int i = 0; i < MAX; i++) {
        // Si se encuentra un null, no hay mas string
        // por lo que se rompe el ciclo y retorna
        if(linea[i] == 0)
            break;

        // Si se encuentra una comilla y no se habia encontrado
        // previamente, se altera una variable para avisar que
        // empezo el string
        if(linea[i] == '"' && !(comilla1)) {
            comilla1 = 1;
            continue;
        }

        // Si la primer comilla se encuentra
        if(comilla1) {

            // Revisa si el caracter que se esta leyendo se es un
            // backslash, y no se ha llegado al final de la linea
            // para comprobar si el caracter que sigue despues del
            // backslash es una 'n' indicando que se quiere imprimir un
            // newline
            if(linea[i] == '\\' && i != MAX - 1) {
                if(linea[i+1] == 'n') {
                    almacen[posAlmacen] = '\n';
                    i++;
                    posAlmacen++;
                    continue;
                }
            } else if(linea[i] == '"') { // Si no revisa si se encontro el final
                retorno = 1;             // del string, si es asi rompe el ciclo
                break;                   // y retorna con exito
            }
            almacen[posAlmacen] = linea[i]; // En esta seccion copia cada caracter del string
            posAlmacen++;                   // en otro lugar
        }
    }
    almacen[posAlmacen] = 0; // se coloca un NULL al final del string para saber su final
    return retorno;
}
// Funcion que lee la instruccion y si esta la sintaxis correcta, se ejecuta
char correrInstruccion(char operacion[],char linea[],int pos, int *lineaPos) {

    // Variables indispensables
    char operadores[22];
    char opStr1[11];
    char opStr2[11];
    int *operador1 = NULL;
    int *operador2 = NULL;
    char string[50];
    char esString = 1;

    // Se intenta obtener un string, si esto resulta no se uno
    // se ejecuta la obtencion de operadores
    fetch(2);
    if(!(obtenerString(string,linea))) {
        esString = 0;
        siguientePalabra(linea,operadores,pos);
        obtenerOperadores(opStr1,opStr2,operadores); // se separan los operadores
        operador1 = esRegistro(opStr1);     // Se revisa si son registros algun operador
        operador2 = esRegistro(opStr2);

        // si estos no son registros se intenta leer como una variable
        if(operador1 == NULL) {
            operador1 = obtenerDirMem(opStr1); 
        }
        if(operador2 == NULL) {
            operador2 = obtenerDirMem(opStr2);
        }

        // Si no es una variable se intenta como si fuera una etiqueta
        if(operador1 == NULL) {
            operador1 = obtenerLineaEjecucion(opStr1);
        }

        // Si no se interpreta como un numero
        if(operador1 == NULL) {
            int temp = stringNumero(opStr1,11);
            operador1 = &temp;
        }
        if(operador2 == NULL) {
            int temp = stringNumero(opStr2,11);
            operador2 = &temp;
        }
    } 

    // Esta seccion se usa para ejecutar la operacion que se encontro
    // pasando los parametros, registros y flags que se necesiten alterar
    fetch(3);
    if(comparar(operacion,"sumar",6,6)) {
        if(operador1 == NULL && operador1 == NULL)
            return 0;
        nasmSumar(operador1,operador2,&overflow,&carryFlag,&signFlag,&zeroFlag);
        return 1;
    } if(comparar(operacion,"mover",6,6)) {
        if(operador1 == NULL && operador1 == NULL)
            return 0;
        nasmMover(operador1,operador2);
        return 1;
    } if(comparar(operacion,"ciclar",7,7)) {
        if(operador1 == NULL)
            return 0;
        char reiniciar = 0;
        nasmCiclar(operador1,&registro3,&pCounter,&reiniciar);
        if(reiniciar) {
            *lineaPos = -1;
            rewind(archivo);
        }
        return 1;
    } if(comparar(operacion,"meter",6,6)) {
        if(operador1 == NULL)
            return 0;
        nasmMeter(pila,operador1,&posPila);
        return 1;
    } if(comparar(operacion,"sacar",6,6)) {
        if(operador1 == NULL)
            return 0;
        nasmSacar(pila,operador1,&posPila);
        return 1;
    } if(comparar(operacion,"comparar",9,9)) {
        if(operador1 == NULL && operador1 == NULL)
            return 0;
        nasmComparar(operador1,operador2,&zeroFlag,&signFlag);
        return 1;
    } if(comparar(operacion,"rotarI",7,7)) {
        if(operador1 == NULL)
            return 0;
        nasmRotarI(operador1,&carryFlag);
        return 1;
    } if(comparar(operacion,"rotarD",7,7)) {
        if(operador1 == NULL)
            return 0;
        nasmRotarD(operador1,&carryFlag);
        return 1;
    } if(comparar(operacion,"correrI",8,8)) {
        if(operador1 == NULL)
            return 0;
        nasmCorrerI(operador1,&carryFlag);
        return 1;
    } if(comparar(operacion,"correrD",8,8)) {
        if(operador1 == NULL)
            return 0;
        nasmCorrerD(operador1,&carryFlag);
        return 1;
    } if(comparar(operacion,"test",5,5)) {
        if(operador1 == NULL && operador2 == NULL)
            return 0;
        nasmAND(operador1,operador2,&zeroFlag,1);
        return 1;
    } if(comparar(operacion,"and",4,4)) {
        if(operador1 == NULL && operador2 == NULL)
            return 0;
        nasmAND(operador1,operador2,&zeroFlag,0);
        return 1;
    } if(comparar(operacion,"saltar",7,7)) {
        if(operador1 == NULL)
            return 0;
        nasmSaltar(operador1,&pCounter);
        *lineaPos = -1;
        rewind(archivo);
        return 1;
    } if(comparar(operacion,"saltarI",8,8)) {
        if(operador1 == NULL)
            return 0;
        char reiniciar = 0;
        nasmSaltarI(operador1,&pCounter,&zeroFlag,&reiniciar);
        if(reiniciar) {
            *lineaPos = -1;
            rewind(archivo);
        }
        return 1;
    } if(comparar(operacion,"saltarNI",9,9)) {
        if(operador1 == NULL)
            return 0;
        char reiniciar = 0;
        nasmSaltarNI(operador1,&pCounter,&zeroFlag,&reiniciar);
        if(reiniciar) {
            *lineaPos = -1;
            rewind(archivo);
        }
        return 1;
    } if(comparar(operacion,"DSF",4,4)) {
        if(operador1 == NULL)
            return 0;
        nasmDSF(operador1,&overflow);
        return 1;
    } if(comparar(operacion,"entrada",8,8)) {
        if(operador1 == NULL)
            return 0;
        move(12,max_x/2 - 5);
        attron(COLOR_PAIR(VERDE)); 
        printw("Entrada: ");
        attroff(COLOR_PAIR(VERDE));
        scanw("%d",operador1);
        while (*operador1 < -16 || *operador1 > 15) {
            attron(COLOR_PAIR(ROJO)); 
            mvprintw(13,max_x/2 - 5,"Numeros entre -16 y 15, intente de nuevo: ");
            attroff(COLOR_PAIR(ROJO));
            move(12,max_x/2 - 5);
            attron(COLOR_PAIR(VERDE)); 
            printw("Entrada:                                             ");
            attroff(COLOR_PAIR(VERDE));
            move(12,max_x/2 + 4);
            scanw("%d",operador1);
        }
        return 1;
    } if(comparar(operacion,"salida",7,7)) {
        if(esString) {
            move(12,max_x/2 - 5);
            attron(COLOR_PAIR(VERDE)); 
            printw("Salida: ");
            attroff(COLOR_PAIR(VERDE));
            printw("%s",string);
        } else {
            if(operador1 == NULL)
                return 0;
            move(12,max_x/2 - 5);
            attron(COLOR_PAIR(VERDE)); 
            printw("Salida: ");
            attroff(COLOR_PAIR(VERDE));
            printw("%d",*operador1);
        }
        refresh();
        noecho();
        getch();
        echo();
        return 1;
    }
    return 1;
}
