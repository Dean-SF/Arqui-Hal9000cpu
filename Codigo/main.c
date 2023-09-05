/*
Creado por Deyan Sanabria Fallas
Este archivo contiene la parte principal del menu
para la ejecucion del simulador de CPU, para su uso
se necesita el archivo cpu.c y operaciones.o (compilado
por NASM y enlazado por GCC a la hora de la compilacion
de main.c) por favor compilar todo en 32bits.
*/

#include <stdio.h>
#include <stdlib.h>
#include "cpu.c"

char automatico = 1;

/*
Funcion para inicializar la memoria y la pila
en 0, pasa por todos sus espacios y asigna 0
*/
void inicializarMemoria() {
    for(int i = 0; i < 16; i++) {
        memoria[i] = 0;
        if(i < 8)
            pila[i] = 0;
    }
}

/*
Funcion que retorna el indice donde empieza
un string, ignorando los espacios iniciales
*/
int primerCaracterLinea(char linea[]) {
    int temp = -1;
    for(int i = 0; i < MAX; i++) {
        if(linea[i] == 0) {         // si encuentra null significa que
            break;                  // no hay mas string
        }
        if(linea[i] != ' ') {       // cuando encuentre algo diferente
            temp = i;               // a espacio, rompe el ciclo y retorna
            break;                  // el indice
        }
    }
    return temp;
}

/*
Funcion para imprimir el CPU en pantalla
*/
void printCPU(char strArch[]) {
    clear(); // Limpia la pantalla anterior
    char binNumber[6];

    // Imprimir registros
    attron(COLOR_PAIR(AMARILLO));
    mvprintw(2,max_x/2 - 74,"REGISTROS BASICOS:");
    attroff(COLOR_PAIR(AMARILLO));
    attron(COLOR_PAIR(CYAN));
    mvprintw(3,max_x/2 - 74,"Registro PC: ");
    attroff(COLOR_PAIR(CYAN));
    attron(COLOR_PAIR(MORADO));
    decToBin(&pCounter,binNumber);
    printw("%05s",binNumber);
    attroff(COLOR_PAIR(MORADO));
    attron(COLOR_PAIR(CYAN));
    mvprintw(4,max_x/2 - 74,"Registro IR: ");
    attroff(COLOR_PAIR(CYAN));
    attron(COLOR_PAIR(MORADO));
    decToBin(&iRegister,binNumber);
    printw("%05s",binNumber);
    attroff(COLOR_PAIR(MORADO));
    attron(COLOR_PAIR(CYAN));
    mvprintw(5,max_x/2 - 74,"R1: ");
    attroff(COLOR_PAIR(CYAN));
    attron(COLOR_PAIR(MORADO));
    decToBin(&registro1,binNumber);
    printw("%05s",binNumber);
    attroff(COLOR_PAIR(MORADO));
    attron(COLOR_PAIR(CYAN));
    mvprintw(6,max_x/2 - 74,"R2: ");
    attroff(COLOR_PAIR(CYAN));
    attron(COLOR_PAIR(MORADO));
    decToBin(&registro2,binNumber);
    printw("%05s",binNumber);
    attroff(COLOR_PAIR(MORADO));
    attron(COLOR_PAIR(CYAN));
    mvprintw(7,max_x/2 - 74,"R3: ");
    attroff(COLOR_PAIR(CYAN));
    attron(COLOR_PAIR(MORADO));
    decToBin(&registro3,binNumber);
    printw("%05s",binNumber);
    attroff(COLOR_PAIR(MORADO));
    attron(COLOR_PAIR(CYAN));
    mvprintw(8,max_x/2 - 74,"R4: ");
    attroff(COLOR_PAIR(CYAN));
    attron(COLOR_PAIR(MORADO));
    decToBin(&registro4,binNumber);
    printw("%05s",binNumber);
    attroff(COLOR_PAIR(MORADO));

    // Imprimir la instruccion actual
    attron(COLOR_PAIR(AMARILLO));
    mvprintw(10,max_x/2 - 74,"Instruccion actual: ");
    attroff(COLOR_PAIR(AMARILLO));
    int indice = primerCaracterLinea(strArch);  // busca que no hayan espacios sobrantes
    if(indice != -1) {                          // antes, debido a que se toma del archivo
        attron(COLOR_PAIR(AZUL));               // la linea para imprimirla
        printw("%s",&strArch[indice]);          
        attroff(COLOR_PAIR(AZUL));
    }
    
    // Imprimir flags
    attron(COLOR_PAIR(AMARILLO));
    mvprintw(12,max_x/2 - 74,"FLAGS:");
    attroff(COLOR_PAIR(AMARILLO));
    attron(COLOR_PAIR(CYAN));
    mvprintw(13,max_x/2 - 74,"Overflow: ");
    attroff(COLOR_PAIR(CYAN));
    attron(COLOR_PAIR(MORADO));
    printw("%c",overflow?'1':'0');
    attroff(COLOR_PAIR(MORADO));
    attron(COLOR_PAIR(CYAN));
    mvprintw(14,max_x/2 - 74,"Carry: ");
    attroff(COLOR_PAIR(CYAN));
    attron(COLOR_PAIR(MORADO));
    printw("%c",carryFlag?'1':'0');
    attroff(COLOR_PAIR(MORADO));
    attron(COLOR_PAIR(CYAN));
    mvprintw(15,max_x/2 - 74,"Zero: ");
    attroff(COLOR_PAIR(CYAN));
    attron(COLOR_PAIR(MORADO));
    printw("%c",zeroFlag?'1':'0');
    attroff(COLOR_PAIR(MORADO));
    attron(COLOR_PAIR(CYAN));
    mvprintw(16,max_x/2 - 74,"Sign: ");
    attroff(COLOR_PAIR(CYAN));
    attron(COLOR_PAIR(MORADO));
    printw("%c",signFlag?'1':'0');
    attroff(COLOR_PAIR(MORADO));
    for(int i = 2; i < 18; i++) {
        mvprintw(i,max_x/2 - 7,"|");
    }

    // Imprimir memoria principal
    attron(COLOR_PAIR(AMARILLO));
    mvprintw(2,max_x/2 - 5,"Memoria Principal:");
    attroff(COLOR_PAIR(AMARILLO));
    int j = 5;
    int linea = 3;
    int hex;

    attron(COLOR_PAIR(MORADO));
    for(int i = 0; i < 16; i++) {
        hex5Bits(&memoria[i],&hex);
        mvprintw(linea,max_x/2 - j,"0x%x",hex);
        if(i != 15) {
            printw(", ");
        }
        j -= 6;
        if(j < -37) {
            j = 5;
            linea++;
        }
    }
    attroff(COLOR_PAIR(MORADO));

    // Imprimir la pila
    j = 5;
    attron(COLOR_PAIR(AMARILLO));
    mvprintw(8,max_x/2 - 5,"Pila:");
    attroff(COLOR_PAIR(AMARILLO));
    attron(COLOR_PAIR(MORADO));
    for(int i = 0; i < 8; i++) {
        hex5Bits(&pila[i],&hex);
        mvprintw(9,max_x/2 - j,"0x%x",hex);
        if(i != 7) {
            printw(", ");
        }
        j -= 6;
    }
    attroff(COLOR_PAIR(MORADO));

    // Imprimir ciclo fetch
    fetch(1);
    attron(COLOR_PAIR(AMARILLO));
    mvprintw(11,max_x/2 - 5,"Interaccion con el programa:");
    attroff(COLOR_PAIR(AMARILLO));
    refresh();
}

/*
Funcion para ejecutar el programa seleccionado
*/
void ejecucion() {
    // Variables necesarias
    char linea[MAX];
    char palabra[22];
    int ultimaPos;
    int lineaPos = -1;
    char seccionCodigo = 0;
    linea[0] = 0;
    printCPU(linea);
    // Se inicia el archivo desde el incio
    rewind(archivo);

    // En cada ciclo se obtiene una linea del archivo con
    // la funcion "fgets()", dicha funcion retorna 0 cuando
    // llega a EOF, de lo contrario retorna 1
    while (fgets(linea,MAX,archivo)) {
        lineaPos++;             // se cuenta por cual linea del archivo se esta
        
        ultimaPos = siguientePalabra(linea,palabra,0);  // se obtiene la primera palabra de la linea
                                                        // y retorna la ultima posicion leida

        if(comparar(palabra,"-INICIO",8,8))  // revisa si llego al inicio del codigo          
            seccionCodigo = 1;
        if(seccionCodigo) {     // una vez en el inicio del codigo ejecuta la instruccion leida
            iRegister = pCounter;   
            if(iRegister != lineaPos) { // se usa para poder saltar a otra linea que
                continue;               // no sea la siguiente
            }
            fetch(1);
            printCPU(linea);
            if(!(correrInstruccion(palabra,linea,ultimaPos,&lineaPos))) // Si la instruccion no se pudo
                return;                                                 // ejecutar, se retorna la funcion
            printCPU(linea);
            // Parar el programa
            if(!automatico) {
                noecho();
                getch();
                echo();
            } else {
                usleep(1000000);
            }
            
        }
        pCounter++;
    }
}

void correrPrograma() {

}

/*
Imprime en pantalla el menu cada vez que se invoca.
Antes de imprimir, se limpia la pantalla.
*/
void printMenu() {
    clear(); // Limpia la pantalla anterior
    attron(COLOR_PAIR(AMARILLO));       
    mvprintw(2,max_x/2 - 16,"BIENVENIDO AL SIMULADOR HAL 9000");
    attroff(COLOR_PAIR(AMARILLO));

    mvprintw(4,max_x/2 - 11,"Seleccione un programa");

    attron(COLOR_PAIR(AZUL));
    mvprintw(6,max_x/2 - 14,"1. Sumar 3");
    mvprintw(7,max_x/2 - 14,"2. Duplicar numero");
    mvprintw(8,max_x/2 - 14,"3. Comparar numeros");
    mvprintw(9,max_x/2 - 14,"4. Programa Custom");
    mvprintw(10,max_x/2 - 14,"5. Habilitar corrida automatica");
    mvprintw(11,max_x/2 - 14,"6. Habilitar corrida manual");
    mvprintw(12,max_x/2 - 14,"0. Salir");
    attroff(COLOR_PAIR(AZUL));

    mvprintw(14,max_x/2 - 4,"Opcion: ");
    refresh(); // Muestra en pantalla
}

/*
muestra el menu principal y recibe las opciones presentados
*/
char menu_principal() {
    printMenu();
    int opcion = -1;
    do {
        opcion = -1;
        scanw("%d", &opcion); // Obtiene la opcion que quiere el usuario
        switch (opcion) { // se elige en este lugar
        case 1:
            initCPU();
            abrirPrograma(1);
            guardarEnMemoria();
            ejecucion();
            printMenu();
            return 1;
        case 2:
            initCPU();
            abrirPrograma(2);
            guardarEnMemoria();
            ejecucion();
            printMenu();
            return 1;
        case 3:
            initCPU();
            abrirPrograma(3);
            guardarEnMemoria();
            ejecucion();
            printMenu();
            return 1;
        case 4:
            initCPU();
            abrirPrograma(4);
            guardarEnMemoria();
            ejecucion();
            printMenu();
            return 1;
        case 5:
            automatico = 1;
            printMenu();
            attron(COLOR_PAIR(VERDE));
            mvprintw(16,max_x/2 - 12,"Corrida automatica habilitada");
            attroff(COLOR_PAIR(VERDE));
            move(14,max_x/2 + 4);
            refresh();
            break;
        case 6:
            automatico = 0;
            printMenu();
            attron(COLOR_PAIR(VERDE));
            mvprintw(16,max_x/2 - 12,"Corrida manual habilitada");
            attroff(COLOR_PAIR(VERDE));
            move(14,max_x/2 + 4);
            refresh();
            break;
        default:
            // En caso de alguna opcion que no este dentro de las solicitadas
            printMenu(); // Se imprime el menu haciendo que se borre lo que haya escrito el usuario
            attron(COLOR_PAIR(ROJO));
            mvprintw(16,max_x/2 - 12,"Error vuelva a intentarlo"); // y se imprime un mensaje de error
            attroff(COLOR_PAIR(ROJO));
            move(14,max_x/2 + 4);
            refresh(); 
            break;
        }
    } while (opcion != 0);
    return 0;
}

int main() {
    char salir;
    inicializarMemoria();
    while (salir) { // Se mete el reinicio de ventana en ciclo por errores
        // Inicio de ventana
        initscr();

        if (has_colors() == false) { // Mensaje de que la ventana no soporta colores si fuese el caso
            endwin();
            printf("La terminal no soporta colores\n");
            return 0;
        }

        // Se obtienen las dimensiones de la consola
        getmaxyx(stdscr,max_y,max_x);

        // Se crean los colores que se usaran enla ventana
        start_color();
        init_pair(AMARILLO,COLOR_YELLOW,COLOR_BLACK);
        init_pair(ROJO,COLOR_RED,COLOR_BLACK);
        init_pair(AZUL,COLOR_BLUE,COLOR_BLACK);
        init_pair(VERDE,COLOR_GREEN,COLOR_BLACK);
        init_pair(MORADO,COLOR_MAGENTA,COLOR_BLACK);
        init_pair(CYAN,COLOR_CYAN,COLOR_BLACK);

        // Inicio de menu principal
        salir = menu_principal();

        // Fin de la ventana
        endwin();
    }
    
    return 0;
}

