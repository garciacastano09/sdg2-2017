//------------------------------------------------------------------
// IMPORTS
//------------------------------------------------------------------
#include "arkanoPi.h"
#include "fsm.h"
#include "tmr.h"
#include <wiringPi.h>

//------------------------------------------------------------------
// VARIABLES ESTATICAS
//------------------------------------------------------------------
static int flagsRefrescoFSM = 0;
static int col_counter=0;
static int columna=0;

//------------------------------------------------------------------
// FUNCIONES SETUP
//------------------------------------------------------------------
void refrescoFSMSetup(fsm_t* refresco_fsm);

//------------------------------------------------------------------
// FUNCIONES SETEO DE FLAGS
//------------------------------------------------------------------
void refresco_tmr_finished(fsm_t* this){
    piLock (FLAGS_REFRESCO_KEY);
    flags |= REFRESCO_TIMEOUT;
    piUnlock (FLAGS_REFRESCO_KEY);
    tmr_startms((tmr_t*)(this->user_data), REFRESCO_TIMEOUT);
}

//------------------------------------------------------------------
// FUNCION DE ENTRADA
//------------------------------------------------------------------
int comprueba_timeout_refresco(fsm_t* this){
    int result;
    piLock (FLAGS_REFRESCO_KEY);
    result = (flags & FLAG_TIMEOUT_REFRESCO);
    piUnlock (FLAGS_REFRESCO_KEY);
    return result;
}

//------------------------------------------------------------------
// FUNCION DE ACCION
//------------------------------------------------------------------
static void refrescar_leds(fsm_t* this){
    col_counter++;
    columna = col_counter%10;
    ActivaFilasLed(&(juego.arkanoPi.pantalla),&(columna));
    switch (columna){
        case 0:
            digitalWrite(gpio_col[0], LOW);
            digitalWrite(gpio_col[1], LOW);
            digitalWrite(gpio_col[2], LOW);
            digitalWrite(gpio_col[3], LOW);
            break;
        case 1:
            digitalWrite(gpio_col[0], HIGH);
            digitalWrite(gpio_col[1], LOW);
            digitalWrite(gpio_col[2], LOW);
            digitalWrite(gpio_col[3], LOW);
            break;
        case 2:
            digitalWrite(gpio_col[0], LOW);
            digitalWrite(gpio_col[1], HIGH);
            digitalWrite(gpio_col[2], LOW);
            digitalWrite(gpio_col[3], LOW);
            break;

        case 3:
            digitalWrite(gpio_col[0], HIGH);
            digitalWrite(gpio_col[1], HIGH);
            digitalWrite(gpio_col[2], LOW);
            digitalWrite(gpio_col[3], LOW);
            break;

        case 4:
            digitalWrite(gpio_col[0], LOW);
            digitalWrite(gpio_col[1], LOW);
            digitalWrite(gpio_col[2], HIGH);
            digitalWrite(gpio_col[3], LOW);
            break;
        case 5:
            digitalWrite(gpio_col[0], HIGH);
            digitalWrite(gpio_col[1], LOW);
            digitalWrite(gpio_col[2], HIGH);
            digitalWrite(gpio_col[3], LOW);
            break;
        case 6:
            digitalWrite(gpio_col[0], LOW);
            digitalWrite(gpio_col[1], HIGH);
            digitalWrite(gpio_col[2], HIGH);
            digitalWrite(gpio_col[3], LOW);
            break;
        case 7:
            digitalWrite(gpio_col[0], HIGH);
            digitalWrite(gpio_col[1], HIGH);
            digitalWrite(gpio_col[2], HIGH);
            digitalWrite(gpio_col[3], LOW);
            break;
        case 8:
            digitalWrite(gpio_col[0], LOW);
            digitalWrite(gpio_col[1], LOW);
            digitalWrite(gpio_col[2], LOW);
            digitalWrite(gpio_col[3], HIGH);
            break;
        case 9:
            digitalWrite(gpio_col[0], HIGH);
            digitalWrite(gpio_col[1], LOW);
            digitalWrite(gpio_col[2], LOW);
            digitalWrite(gpio_col[3], HIGH);
            break;
        default:
            digitalWrite(gpio_col[0], LOW);
            digitalWrite(gpio_col[1], LOW);
            digitalWrite(gpio_col[2], LOW);
            digitalWrite(gpio_col[3], LOW);
            break;
    }
    piLock (FLAGS_REFRESCO_KEY);
    flags &= (~FLAG_TIMEOUT_REFRESCO);
    piUnlock (FLAGS_REFRESCO_KEY);
}

//------------------------------------------------------------------
// FUNCION DE SUPPORT
//------------------------------------------------------------------
void ActivaFilasLed (tipo_pantalla* p_pantalla, int* columna){
    // printf("%s\n", "[LOG] PintaFilasLed");
    int i;
    for(i=0;i<MATRIZ_ALTO;i++) {
        if(p_pantalla->matriz[*columna][i] == 1){
            digitalWrite(gpio_row[i], LOW);
        }
        else{
            digitalWrite(gpio_row[i], HIGH);
        }
    }
}
