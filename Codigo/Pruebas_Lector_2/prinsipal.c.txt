#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/time.h"
#include "string.h"
#include "hardware/gpio.h"
#include "hardware/timer.h"
#include "Led.h"
#include "Inventario.h"

uint8_t one_second  = 0; // variable global para contar segundos
void timer_callback_1(struct repeating_timer *t); // function to count seconds

/**
 * @brief main se llama a la función scan_keys que es donde se contiene el sistema de ingreso, analizando el teclado 
 * y poniendo las señales luminosas. 
*/
int main() {

    Producto productos[5];
    int inventario[5] = {0}; // Inventario inicializado a cero
    float valor_mercancia_bodega = 0.0;
    float valor_ventas = 0.0;
    int capacidad_bodega = 0;
    char tipo_producto[20];
    int num_items;
    float precio_unitario;

    keyboard_init();
    printf("Ingrese el valor del teclado\n");
    char letra_ori = '0';
    bool flag_a = false;
    bool flag_b = false;
    bool autentificacion = false;
    int value_in = 0;
    int value_out = 0;

    // definicion de la alarma para controlar los tiempos de funcionamiento del programa
    struct repeating_timer timer_1;
    add_repeating_timer_ms(1000, timer_callback_1, NULL, &timer_1);


    while (1)
    {

        //verificamos la autentificacion del usuario para poder dar acceso a la lectura de los TAG
        if (!autentificacion){
            if (verificar_password){
                autentificacion = true;
            }
        }
        else if(autentificacion){
            // Leer TAG RFID y obtener información del producto
            leer_TAG_RFID(tipo_producto, &num_items, &precio_unitario, &capacidad_bodega);

            // Chequeamos si hay una transicion y la atendemos 
            get_transaccion(&letra_ori, &value_in, &value_out);
            if ((letra_ori == 'A') || (flag_a)){
                flag_a = true;

                if (letra_ori == 'D'){
                    flag_a = false;
                    printf("cantidad de elementos de entrada: %d\n", value_in);
                    transaccion_ingreso(&productos[0], inventario, &valor_mercancia_bodega, &valor_ventas);
                }
            }
            if ((letra_ori == 'B') || (flag_b)){
                flag_b = true;

                if (letra_ori == 'D'){
                    flag_b = false;
                    printf("numero de elementos de salida: %d\n", value_out);
                    transaccion_salida(&productos[0], inventario, &valor_mercancia_bodega, &valor_ventas);
                }
            }
        
            if(one_second == 1){
                // ver datos en consola
                one_second = 0;
            }
        }
    }
    return 0;
}
void timer_callback_1(struct repeating_timer *t){
    // Incrementar el índice de la secuencia
    one_second++;
    // Reiniciar el índice si llegamos al final de la secuencia
}