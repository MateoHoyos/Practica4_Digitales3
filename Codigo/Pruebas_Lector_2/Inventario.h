#ifndef INVENTARIO_H
#define INVENTARIO_H

/**
 * Funciones disponibles para manejar el inventario
*/
typedef struct {
    char tipo[20];
    int num_items;
    float precio_unitario;
    int capacidad_maxima;
} Producto;
void keyboard_init(); // inicializacion del teclado matricial
bool verificar_password(); // para la verificacion del usuario principal
void get_transaccion(char *let, int *entrada, int *salida);
int convert_to_int(char entrada[], char inicio, char fin);
void transaccion_ingreso(Producto *producto, int *inventario, float *valor_mercancia_bodega, float *valor_ventas);
void transaccion_salida(Producto *producto, int *inventario, float *valor_mercancia_bodega, float *valor_ventas);
void leer_TAG_RFID(char *tipo_producto, int *num_items, float *precio_unitario, int *capacidad_max);

#endif