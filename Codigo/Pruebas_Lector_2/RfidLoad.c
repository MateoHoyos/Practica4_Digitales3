#include "mfrc522.h"
#include "lcd_i2c_16x2.h"
#include "Inventario.h"
#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/time.h"
#include "string.h"
#include "hardware/gpio.h"
#include "hardware/timer.h"
//int item=0;
uint8_t tag1[] = {0xD6, 0x33, 0xAF, 0xAE};
MIFARE_Key keyBlock={0xFF, 0xFF, 0xFF, 0xFF,0xFF, 0xFF};
uint8_t Data1[16] = {0x48, 0x4F, 0x4C, 0x41,0X20,0X20,0X20,0X20,0X20,0X20,0X20,0X20,0X20,0X20,0X20,0X20};
//uint8_t block=1;
bool banProd=0;
u_int8_t store[15]={2,1,2,1,1,2,1,1,2,1,1,2,1,1,2};//Esta es la variable del almacen en la cual esta en orden de prduco
//Trigo[0-2]:0=cantiad del producto, 1=Precio entrada, 2=Precio salida,
//Orden cada 3 campos del vector: Trigo, Maiz, Arroz, Soya, Papa.
uint8_t bufferRead[18]; 
uint8_t bufferBod[18]; 
uint8_t one_second  = 0; // variable global para contar segundos
void timer_callback_1(struct repeating_timer *t); // function to count seconds
char mensaje[32] = "                                ";

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
           }
           PCD_StopCrypto1(mfrc);
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
void impProd(uint item){
    
    //printf("Acerque el tag .\n");
    //static int item=0;
        if(!banProd){
            timer_hw->alarm[0] = (uint32_t)(time_us_64() + 1000000);
            timer_hw->intr = 0x00000001;
             printf("Acerque el tag .\n");
        }
    switch (item)
    {
        case 0:
            strcpy(mensaje, "Trigo           Unds:   Precio: ");
            mensaje[21]=store[0]+48;
            mensaje[31]=store[2]+48;
            initVar(mensaje,true);
            break;
        case 1:
            strcpy(mensaje, "Trigo           Unds:   Precio: ");
            mensaje[21]=store[3]+48;
            mensaje[31]=store[5]+48;
            initVar(mensaje,true);
            break;
        case 2:
            strcpy(mensaje, "Trigo           Unds:   Precio: ");
            mensaje[21]=store[6]+48;
            mensaje[31]=store[8]+48;
            initVar(mensaje,true);
            break;
        case 3:
            strcpy(mensaje, "Trigo           Unds:   Precio: ");
            mensaje[21]=store[9]+48;
            mensaje[31]=store[11]+48;
            initVar(mensaje,true);
            break;
        case 4:
            strcpy(mensaje, "Trigo           Unds:   Precio: ");
            mensaje[21]=store[12]+48;
            mensaje[31]=store[14]+48;
            initVar(mensaje,true);
            break;
        default:
            item=0;
            break;
        }
    item++;
    
    
}
void timerCallback(uint num){

}
void initTimer(void){
    /// claim alarm0 for yellow LED
    if(!hardware_alarm_is_claimed (0))
        hardware_alarm_claim(0);
    else
        printf("Tenemos un problemaj alarm 0\n");

    /// Claim alarm 1 for Green LED
    if(!hardware_alarm_is_claimed (1))
        hardware_alarm_claim(1);
    else
        printf("Tenemos un problema alarm 1\n");
    
    /// Claim alarm 2 for Red LED
    if(!hardware_alarm_is_claimed (2))
        hardware_alarm_claim(2);
    else
        printf("Tenemos un problema alarm 2\n");

    /// Claim alarm 3 for Time Out
    if(!hardware_alarm_is_claimed (3))
        hardware_alarm_claim(3);
    else
        printf("Tenemos un problema alarm3\n");

    /// Set callback for each alarm. TODO: replace with an exclusive handler
    hardware_alarm_set_callback(0,impProd);
    hardware_alarm_set_callback(1,impProd);
    hardware_alarm_set_callback(2,impProd);
    hardware_alarm_set_callback(3,impProd);

    
    timer_hw->intr = 0x0000000F;    ///< Clear interrupt flags that maybe pendant
    //timer_hw->inte = 0x0000000F;    ///< Activate interrupts for all 4 alarms

}

void main() {
    //_wfi();
    //stdio_init_all();
    //configurar_temporizador();  // Configurar el temporizador para las interrupciones periódicas
    // Declare card UID's
    
    MFRC522Ptr_t mfrc = MFRC522_Init();
    PCD_Init(mfrc, spi0);

    sleep_ms(5000);
    
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
    bool inicio=false;
    int value_in = 0;
    int value_out = 0;
    int opcion=0;
    // definicion de la alarma para controlar los tiempos de funcionamiento del programa
    struct repeating_timer timer_1;
    add_repeating_timer_ms(1000, timer_callback_1, NULL, &timer_1);
    //add_alarm_in_ms(2000, &impProd, NULL, true);
    while(1) {
        tight_loop_contents();  // Mantener el programa en ejecución
        //Esperando la tarjeta
        //initVar(mensaje,true);
        //readBlock(mfrc);
        //sleep_ms(5000);
    if(inicio){
        printf("Elija una opcion:\n");
        printf("1. Iniciar caja\n");
        printf("2. Grabar tag\n");
        strcpy(mensaje, "Phush 1 grabar tag. 2 continuar.");
        initVar(mensaje,true);
        int opcion;
        printf("Ingrese la opcion deseada: ");
        scanf("%d", &opcion);
        inicio=false;
    }
    if(opcion==1){//cuando se activa es para escribir los tags de resto se mantiene en 0
        int tipoP=30;
        int cant=20;
        int USin=25;
        int USout=15;
        strcpy(mensaje, "Modo escritura                  .");
        initVar(mensaje,true);
        uint8_t write[8];
        printf("ELija.\n");
        printf("1. Trigo\n");
        printf("2. Maiz\n");
        printf("3. Arroz\n");
        printf("4. Soya\n");
        printf("5. Papa\n");
        //scanf("%d", &tipoP);
        
        printf("Ingrese catidad del producto.\n");
        //scanf("%d", &cant);
        printf("Ingrese precio de entrada del producto.\n");
        //scanf("%d", &USin);
        printf("Ingrese precio de salida del producto.\n");
        //scanf("%d", &USout);
        write[0]=tipoP;
        write[1]=cant;
        write[2]=USin;
        write[3]=USout;
        write[4]=17;
        printf("Acerque el tag .\n");
        //scanf("%d", &USout);
        writeBlock(mfrc,1,write);
        opcion=0;
    }else{
         if (!autentificacion){
            if(user){
                strcpy(mensaje, "Bienvenid@       Acerque tarjeta.");
                initVar(mensaje,true);
            }
            while (user){
                if(readCard(mfrc)){
                    user=false;
                    strcpy(mensaje, "Ingrese         contrasena.  .");
                    initVar(mensaje,true);
                    break;
                }else{
                    strcpy(mensaje, "Usuario         no valido.     .");
                    initVar(mensaje,true);
                }
            }
            
            //strcpy(mensaje, "Ingrese         contrasena.   ");
            //initVar(mensaje,true);
            if (verificar_password()){
                printf("Contraseña correcta: \n\r");
                autentificacion = true;
                strcpy(mensaje, "Contrasena      correcta.       .");
                initVar(mensaje,true);
                impProd(0);
                //sleep_ms(2000);
            }
        }
        else if(autentificacion){
            // Leer TAG RFID y obtener información del producto
            //
            
            //banProd=1;
            //tight_loop_contents();  // Mantener el programa en ejecución
            
            //leer_TAG_RFID(tipo_producto, &num_items, &precio_unitario, &capacidad_bodega);
            
            // Chequeamos si hay una transicion y la atendemos 
            get_transaccion(&letra_ori, &value_in, &value_out);
            //if ((letra_ori == 'A') || (flag_a)){
                if (letra_ori == 'A'){
                    printf("Esperando Tag..: \n\r");
                flag_a = true;
                letra_ori='z';
                //banProd=1;
                strcpy(mensaje, "Ingreso         Acerque tag.   .");
                initVar(mensaje,true);
                readBlock(mfrc,1,bufferRead);
                
                for (size_t i = 0; i < 3; i++)
                {
                    /* code */
                }
                
                
            }
            if (letra_ori == 'D'&&(flag_a)){
                    //flag_a = 0;
                    //banProd=1;
                    strcpy(mensaje, "Coloque tarjeta bodeja         .");
                    initVar(mensaje,true);
                    readBlock(mfrc,1,bufferBod);
                    if(store[bufferRead[0]*3-3]<bufferBod[bufferRead[0]-1]){
                        store[bufferRead[0]*3-3]++;
                        //impProd(bufferRead[0]);
                    }else{
                        strcpy(mensaje, "Cantidad de producto al limite.");
                        initVar(mensaje,true);
                    }
                    
                    flag_a = false;
                    printf("cantidad de elementos de entrada: %d\n", value_in);
                    
                    impProd(bufferRead[0]);
                    
                    //transaccion_ingreso(&productos[0], inventario, &valor_mercancia_bodega, &valor_ventas);
                }
            if (letra_ori == 'B'){
                flag_b = true;
                letra_ori='z';
                strcpy(mensaje, "Salida           Acerque tag.  .");
                initVar(mensaje,true);
                readBlock(mfrc,1,bufferRead);
                store[bufferRead[0]*3-3]--;
                impProd(bufferRead[0]);
                
            }
            if (letra_ori == 'D'&&(flag_b)){
                    strcpy(mensaje, "Coloque tarjeta bodeja         .");
                    initVar(mensaje,true);
                    readBlock(mfrc,1,bufferBod);
                    if(store[bufferRead[0]*3-3]>0){
                        store[bufferRead[0]*3-3]--;
                        strcpy(mensaje, "restado   corectamente      ");
                        initVar(mensaje,true);
                    }else{
                        strcpy(mensaje, "Cantidad de producto al minimo.");
                        initVar(mensaje,true);
                    }
                    
                    flag_b = false;
                    printf("cantidad de elementos de entrada: %d\n", value_in);
                    
                    impProd(bufferRead[0]);
                }
        
            if(one_second == 1){
                // ver datos en consola
                //one_second = 0;
            }
        }
    }
        //verificamos la autentificacion del usuario para poder dar acceso a la lectura de los TAG
       
    } 
}
void timer_callback_1(struct repeating_timer *t){
    // Incrementar el índice de la secuencia
    one_second++;
    //impProd();
    // Reiniciar el índice si llegamos al final de la secuencia
}



