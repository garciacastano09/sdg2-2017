#ifndef _ARKANOPI_H_
#define _ARKANOPI_H_

#include <time.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/time.h>

#include "arkanoPi.h"
#include "arkanoPiLib.h"
#include <wiringPi.h>
#include "fsm.h"
#include "tmr.h"

//------------------------------------------------------------------
// VARIABLES BUS SPI
//------------------------------------------------------------------
#define SPI_ADC_CH 0
#define SPI_ADC_FREQ 1000000 //1MHz

// PERIODO DE ACTUALIZACION DE LA MAQUINA ESTADOS
#define CLK_MS 10

// PINES GPIO
#define GPIO_COL_1 14
#define GPIO_COL_2 17
#define GPIO_COL_3 18
#define GPIO_COL_4 22
#define GPIO_ROW_1 0
#define GPIO_ROW_2 1
#define GPIO_ROW_3 2
#define GPIO_ROW_4 3
#define GPIO_ROW_5 4
#define GPIO_ROW_6 7
#define GPIO_ROW_7 23
#define GPIO_RAQ_IZQ 26
#define GPIO_RAQ_DER 27

// KEYS DE MUTEX
#define	STD_IO_BUFFER_KEY	1
#define	FLAGS_ARKANO_KEY	2
#define	PANTALLA_KEY	4

// FLAGS DE LA FSM
#define FLAG_PELOTA			0x01
#define FLAG_RAQUETA_DERECHA			0x02
#define FLAG_RAQUETA_IZQUIERDA			0x04
#define FLAG_FINAL_JUEGO		0x8
#define FLAG_JOYSTICK		0x10

// CONSTANTES DE TIEMPO
#define DEBOUNCE_TIME  5
#define PELOTA_TIMEOUT 500 //tiempo de movimiento de pelota
#define REFRESCO_TIMEOUT 1 //tiempo de exploracion de columnas
#define JOYSTICK_TIMEOUT 100

//------------------------------------------------------------------
// STRUCTS
//------------------------------------------------------------------

typedef enum {
	WAIT_START=0,
	WAIT_PUSH=1,
	WAIT_END=2} tipo_estados_juego;

typedef struct {
	tmr_t* pelota_tmr;
	tmr_t* refresco_tmr;
	tmr_t* joystick_tmr;
} tipo_temporizadores;


typedef struct {
	tipo_arkanoPi arkanoPi;
	tipo_estados_juego estado;
	tipo_temporizadores temporizadores;
} tipo_juego;

//------------------------------------------------------------------
// REFRESCO DE LEDS
//------------------------------------------------------------------
void refrescarLeds(union sigval value);
void activaFilasLed (tipo_pantalla* p_pantalla, int* columna);

//------------------------------------------------------------------
// MOVIMIENTO JOYSTICK
//------------------------------------------------------------------
void mueveRaquetaAPosicion (int posicion);
float lecturaADC (void);

//------------------------------------------------------------------
// ARKANOPI FSM: FUNCIONES SETUP
//------------------------------------------------------------------
void arkanoPiFSMSetup(fsm_t* arkano_fsm);

//------------------------------------------------------------------
// ARKANOPI FSM: FUNCIONES SETEO DE FLAGS
//------------------------------------------------------------------
void pulsaRaqIzq(void);
void pulsaRaqDer(void);
void pelotaTmrFinished(union sigval value);

//------------------------------------------------------
// ARKANOPI FSM: FUNCIONES DE ENTRADA
//------------------------------------------------------
int compruebaTeclaPelota (fsm_t* this);
int compruebaTeclaRaquetaDerecha (fsm_t* this);
int compruebaTeclaRaquetaIzquierda (fsm_t* this);
int compruebaFinalJuego (fsm_t* this);
int compruebaTeclaPulsada(fsm_t* this);
int compruebaTimeoutPelota (fsm_t* this);

//------------------------------------------------------
// ARKANOPI FSM: FUNCIONES DE ACCION
//------------------------------------------------------
void inicializaJuego (fsm_t* this);
void finalJuego (fsm_t* this);
void reseteaJuego (fsm_t* this);
void mueveRaquetaIzquierda (fsm_t* this);
void mueveRaquetaDerecha (fsm_t* this);
void movimientoPelota (fsm_t* this);
void movimientoJoystick (fsm_t* this);

//------------------------------------------------------------------
// ARKANOPI FSM: FUNCIONES SUPPORT
//------------------------------------------------------------------
int obtenerTipoDeRebote(void);
void desplazarPelota(void);
void reboteLadrillo(void);
void reboteTecho(void);
void reboteLateral(void);
void reboteRaqueta(void);

//------------------------------------------------------
// ARKANO PI: FUNCIONES DE INICIALIZACION
//------------------------------------------------------
int systemSetup (void);
void delayUntil (unsigned int next);

#endif /* ARKANOPI_H_ */
