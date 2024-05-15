/**
 * @file Teclado Matricial
 *
 * @brief Código principal para el uso del teclado matricial 4x4.
 * Definición del teclado, captura de teclas y función antirrebote.
 */

// Libreias requeridas
#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/time.h"
#include "string.h"
#include "hardware/gpio.h"
#include "hardware/timer.h"
#include "Teclado.h"

/**
 * @brief Estructura para almacenar información de productos
*/
typedef struct {
    char tipo[20];  // nombre del producto
    int num_items;  // Cantidad de productos
    float precio_unitario;
    int capacidad_maxima;
} Producto;

/**
 * @brief Creación del teclado matricial 4x4
*/
const char matrix_keys[4][4] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}
};

/**
 * @brief Creación de variables
 * Se crean variables tanto para la lectura del teclado en filas y columnas 
 * Variables para el antirrebote.
*/
const int keypad_rows[4] = {2, 3, 4, 5};
const int keypad_columns[4] = {6, 7, 8, 9};
char key[4] = "";
char transa_in[4] = "";     // solo se manejar transacciones de maxiomo 9999
char transa_out[4] = "";    // solo se manejar transacciones de maxiomo 9999
bool flag_a = false;
bool flag_b = false;
int cont_key = 0;
int contA = 0;
int contB = 0;
bool tecla_antirrebote[4][4] = {false};

/**
 * @brief Setup
 * Se inicializa el teclado según los pines para las columnas y filas 
*/
void keyboard_init() {
    stdio_init_all();
    /**
     * @param keypad_rows variable en la que se almacena la fila del teclado 
     * Se configura como salida, ya que como se verá dentro de la función scan_keys()
     * las filas siempre van en 1, debido a que se busca cuando la columna que se presiones 
     * esté en uno y así se detecte la tecla que se presiona 
     * 
    */
    for (int i = 0; i < 4; i++) {
        gpio_init(keypad_rows[i]);
        gpio_set_dir(keypad_rows[i], GPIO_OUT);
    }
    /**
     * @param keypad_columns variable que almacena las columnas del teclado
     * Se inicializa como salida y como pull_down ya que cuando se presiona una tecla, 
     * esta variable se pone en uno, encontrando así la tecla presionada, es por esto 
     * que se pone @param gpio_pulldown_keypad_columns para que se mantenga el estado 
     * lógico de 0 cuando no se está aplicando una señal externa al pin de la Raspberry.
    */
    for (int i = 0; i < 4; i++) {
        gpio_init(keypad_columns[i]);
        gpio_set_dir(keypad_columns[i], GPIO_IN);
        gpio_pull_down(keypad_columns[i]);
    }
}

/**
 * @brief convert_to_int
 * Esta función recibe una cadena de caracteres y retorna un tipo de dato int asociado.
 * Nos permite capturar por ejemplo el numero de productos que queremos agrgar o sacar del inventario.
 * @param entrada arreglo de caracteres que contiene la entrada completa del usuario.
 * @param password arreglo de caracteres donde se almacena la contraseña extraída de entrada.
 */
int convert_to_int(char entrada[], char inicio, char fin){
    // Variable para almacenar el número entero resultante
    int numero = 0;
    // Variable para indicar si estamos dentro del rango entre inicio y fin
    int Rango = 0;

    // Recorremos el vector de caracteres
    for (int i = 0; entrada[i] != '\0'; i++) {
        // Si encontramos el carácter de inicio, activamos la bandera
        if (entrada[i] == inicio) {
            Rango = 1;
        }
        // Si estamos dentro del rango y encontramos un dígito, lo acumulamos en el número
        else if (Rango && entrada[i] >= '0' && entrada[i] <= '9') {
            numero = numero * 10 + (entrada[i] - '0');
        }
        // Si encontramos el carácter de fin, salimos del ciclo
        else if (Rango && entrada[i] == fin) {
            break;
        }
    }
    return numero;
}

bool verificar_password(){
    for (int row = 0; row < 4; row++) {
        gpio_put(keypad_rows[row], 1);
        for (int column = 0; column < 4; column++) {
            if (gpio_get(keypad_columns[column]) && !tecla_antirrebote[row][column]) {
                // obtenemos la contraceña
                key[cont_key] = matrix_keys[row][column];
                printf("Presionaste %c\n", matrix_keys[row][column]);
                sleep_ms(500);
                tecla_antirrebote[row][column] = true;
                cont_key++;
                if (cont_key == 3){
                    if(key[0] == '1' && key[1] == '2' && key[2] == '2' && key[3] == '3'){
                        return true; 
                    }
                    else{
                        cont_key = 0;
                        memset(key, 0, sizeof(key));
                    }
                }
            }
            else if (!gpio_get(keypad_columns[column])) {
                // La tecla se ha soltado, ayuda en el antirrebote 
                tecla_antirrebote[row][column] = false;
            }
        }
        gpio_put(keypad_rows[row], 0);
    }
    return false;
}

//funcion para capurar los datos de una transaccion de entrada o salida respectivamente.
// A + numero de elementos que se desean agregar + D
// B + numero de elementos que se desaan retirar + D
// D funciona cmo tecla de confirmacion.
void get_transaccion(char *let, int *entrada, int *salida){
    for (int row = 0; row < 4; row++) {
        gpio_put(keypad_rows[row], 1);
        for (int column = 0; column < 4; column++) {
            if (gpio_get(keypad_columns[column]) && !tecla_antirrebote[row][column]) {
                *let = matrix_keys[row][column];
                sleep_ms(500);
                tecla_antirrebote[row][column] = true;

                // para el caso de una transacion de elementos de entrada
                if ((*let == 'A') || (flag_a)){
                    flag_a = true;
                    printf("Presionaste %c\n", *let);
                    transa_in[contA] = *let;
                    contA++;
                    if (*let == 'D'){
                        flag_a = false;
                        value = convert_to_int(amplitud, 'A', 'D');

                        // solo se aceptan transaciones de un elemento o más
                        if(value > 0 ){
                            *entrada = value; 
                        }
                        contA = 0;
                        memset(transa_in, 0, sizeof(transa_in));
                    }
                    if (contA > 4){
                        contA = 0;
                    }
                }

                // para el caso de una transaccion de elementos de salida
                if ((*let == 'B') || (flag_b)){
                    flag_b = true;
                    printf("Presionaste %c\n", *let);
                    transa_out[contB] = *let;
                    contB++;
                    if (*let == 'D'){
                        flag_b = false;
                        value = convert_to_int(nivelDC, 'B', 'D');
                        if(value >= 0){
                            *salida = value; 
                        }
                        contB = 0;
                        memset(transa_out, 0, sizeof(transa_out));
                    }
                    if (contB > 5){
                        contB = 0;
                    }
                }
            }
            else if (!gpio_get(keypad_columns[column])) {
                // La tecla se ha soltado, ayuda en el antirrebote 
                tecla_antirrebote[row][column] = false;
            }
        }
        gpio_put(keypad_rows[row], 0);
    }
}

/**
 * @brief Función para leer el TAG RFID
*/
void leer_TAG_RFID(char *tipo_producto, int *num_items, float *precio_unitario, int *capacidad_max) {
    // leer el TAG RFID y extraer la información
    strcpy(tipo_producto, "Producto1");
    *num_items = 10;
    *precio_unitario = 5.99;
    *capacidad_max = 100;
}

/**
 * @brief Función para realizar transacción de ingreso
*/
void transaccion_ingreso(Producto *producto, int *inventario, float *valor_mercancia_bodega, float *valor_ventas) {
    // Implementar lógica para transacción de ingreso
}
/**
 * @brief Función para realizar transacción de salida
*/
void transaccion_salida(Producto *producto, int *inventario, float *valor_mercancia_bodega, float *valor_ventas) {
    // Implementar lógica para transacción de salida
}