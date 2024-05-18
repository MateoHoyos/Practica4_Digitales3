#include "mfrc522.h"
#include "lcd_i2c_16x2.h"
#include "Inventario.h"
#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/time.h"
#include "string.h"
#include "hardware/gpio.h"
#include "hardware/timer.h"
uint8_t tag1[] = {0xD6, 0x33, 0xAF, 0xAE};
MIFARE_Key keyBlock={0xFF, 0xFF, 0xFF, 0xFF,0xFF, 0xFF};
uint8_t Data1[16] = {0x48, 0x4F, 0x4C, 0x41,0X20,0X20,0X20,0X20,0X20,0X20,0X20,0X20,0X20,0X20,0X20,0X20};
//uint8_t block=1;
//uint8_t bufferRead[18];   
uint8_t one_second  = 0; // variable global para contar segundos
void timer_callback_1(struct repeating_timer *t); // function to count seconds


void readBlock(MFRC522Ptr_t mfrc,uint8_t block,uint8_t bufferRead[18]){
    uint8_t len=18;
    printf("Coloque la tarjeta\n\r");
    while(!PICC_IsNewCardPresent(mfrc));
    printf("Leyendo\n\r");
    PICC_ReadCardSerial(mfrc);
    printf("Reconocida\n\r");
    if(PCD_Authenticate(mfrc,PICC_CMD_MF_AUTH_KEY_A,block,&keyBlock, &(mfrc->uid))==0){
        printf("Authentication melo: \n\r");
        if(MIFARE_Read(mfrc,block,bufferRead,&len)==0){
            printf("leida bloque melo: \n\r");
            for (int i = 0; i < 5; i++) {
            printf( "%c",bufferRead[i]);
           PCD_StopCrypto1(mfrc);
           }
           printf( "\n");
        }else{
        printf("Leida de bloque failed: \n\r");
        }
    }else{
        printf("Authentication failed: \n\r");
    }
}
void writeBlock(MFRC522Ptr_t mfrc,uint8_t block, u_int8_t data1[8]){
    printf("Coloque la tarjeta\n\r");
    while(!PICC_IsNewCardPresent(mfrc));
    printf("Leyendo\n\r");
    PICC_ReadCardSerial(mfrc);
    printf("Reconocida\n\r");
    if(PCD_Authenticate(mfrc,PICC_CMD_MF_AUTH_KEY_A,block,&keyBlock, &(mfrc->uid))==0){
        printf("Authentication melo: \n\r");
        if(MIFARE_Write(mfrc,block,data1,16)==0){
            printf("Escrita bloque melo: \n\r");
           PCD_StopCrypto1(mfrc);
        }else{
        printf("Leida de bloque failed: \n\r");
        }
    }else{
        printf("Authentication failed: \n\r");
    }
}
bool readCard(MFRC522Ptr_t mfrc){
    printf("Waiting for card\n\r");
        while(!PICC_IsNewCardPresent(mfrc));
        //Se selecciona la tarjeta
        printf("Selecting card\n\r");
        PICC_ReadCardSerial(mfrc);

        //Mostrar UID en monitor serie
        printf("PICC dump: \n\r");
        PICC_DumpToSerial(mfrc, &(mfrc->uid));

        //Autorización con uid
        printf("Uid is: ");
        for(int i = 0; i < 4; i++) {
            printf("%x ", mfrc->uid.uidByte[i]);
        } printf("\n\r");
        if(memcmp(mfrc->uid.uidByte, tag1, 4) == 0) {
            printf("Usuario valido se a ingresado a la caja \n\r");
            return true;
            
        } else {
            printf("Authentication Failed\n\r");
            printf("The value of my_number is: %d\n",memcmp(mfrc->uid.uidByte, tag1, 4));
            return false;
        }
    
}
void main() {
    stdio_init_all();

    // Declare card UID's
    

    MFRC522Ptr_t mfrc = MFRC522_Init();
    PCD_Init(mfrc, spi0);

    sleep_ms(5000);
    char mensaje[32] = "Hola, mundo! Este es un mensaje.";
    // char test_result = PCD_SelfTest(mfrc);

    // if (test_result == 0) {
    //     printf("Self Test:  SUCCESS");
    // } else {
    //     printf("Self Test: FAILED");
    // }
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
    bool user = true;
    int value_in = 0;
    int value_out = 0;

    // definicion de la alarma para controlar los tiempos de funcionamiento del programa
    struct repeating_timer timer_1;
    add_repeating_timer_ms(1000, timer_callback_1, NULL, &timer_1);

    while(1) {
        //Esperando la tarjeta
        //initVar(mensaje,true);
        //readBlock(mfrc);
        //sleep_ms(5000);
        //verificamos la autentificacion del usuario para poder dar acceso a la lectura de los TAG
        if (!autentificacion){
            if(user){
                strcpy(mensaje, "Bienvenid@      Acerque tarjeta ");
                initVar(mensaje,true);
            }
            while (user){
                if(readCard(mfrc)){
                    user=false;
                    strcpy(mensaje, "Ingrese         contrasena.   ");
                    initVar(mensaje,true);
                    break;
                }else{
                    strcpy(mensaje, "Usuario         no valido");
                    initVar(mensaje,true);
                }
            }
            
            //strcpy(mensaje, "Ingrese         contrasena.   ");
            //initVar(mensaje,true);
            if (verificar_password()){
                printf("Contraseña correcta: \n\r");
                autentificacion = true;
                strcpy(mensaje, "Contrasena      correcta.        ");
                initVar(mensaje,true);
                sleep_ms(2000);
            }
        }
        else if(autentificacion){
            // Leer TAG RFID y obtener información del producto
            leer_TAG_RFID(tipo_producto, &num_items, &precio_unitario, &capacidad_bodega);
            printf("Esperando Tag..: \n\r");
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
}
void timer_callback_1(struct repeating_timer *t){
    // Incrementar el índice de la secuencia
    one_second++;
    // Reiniciar el índice si llegamos al final de la secuencia
}



