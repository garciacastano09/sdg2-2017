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
#include <wiringPi.h>

#include "kbhit.h" // para poder detectar teclas pulsadas sin bloqueo y leer las teclas pulsadas

#include "fsm.h"
#include "arkanoPiLib.h"

// FLAGS DEL SISTEMA
#define CLK_MS 10 // PERIODO DE ACTUALIZACION DE LA MAQUINA ESTADOS
#define FLAG_PELOTA			0x01
#define FLAG_RAQUETA_DERECHA			0x02
#define FLAG_RAQUETA_IZQUIERDA			0x04
#define FLAG_FINAL_JUEGO		0x8
#define FLAG_JOYSTICK		0x16

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
#define REFRESH_TIME 40 //tiempo de exploración de columnas

// A 'key' which we can lock and unlock - values are 0 through 3
//	This is interpreted internally as a pthread_mutex by wiringPi
//	which is hiding some of that to make life simple.
#define	FLAGS_KEY	1
#define	STD_IO_BUFFER_KEY	2

typedef enum {
	WAIT_START=0,
	WAIT_PUSH=1,
	WAIT_END=2} tipo_estados_juego;

enum refresco_state {REFRESCO_COL};

typedef struct {
	tipo_arkanoPi arkanoPi;
	tipo_estados_juego estado;
	char teclaPulsada;
} tipo_juego;

//------------------------------------------------------
// FUNCIONES DE ACCION
//------------------------------------------------------
void InicializaJuego (void);
void FinalJuego (void);
void ReseteaJuego (fsm_t* this);
void MueveRaquetaIzquierda (void);
void MueveRaquetaDerecha (void);
void MovimientoPelota (void);

//------------------------------------------------------
// FUNCIONES DE INICIALIZACION
//------------------------------------------------------
int systemSetup (void);

//------------------------------------------------------
// FUNCIONES DE ENTRADA O DE TRANSICION DE LA MAQUINA DE ESTADOS
//------------------------------------------------------
// Prototipos de funciones de entrada
int comprueba_tecla_pelota (fsm_t* this);
int comprueba_tecla_raqueta_derecha (fsm_t* this);
int comprueba_tecla_raqueta_izquierda (fsm_t* this);
int comprueba_joystick (fsm_t* this);
int comprueba_final_juego (fsm_t* this);

//------------------------------------------------------
// SUBRUTINAS DE ATENCION A LAS INTERRUPCIONES
//------------------------------------------------------
PI_THREAD (thread_explora_teclado);

#endif /* ARKANOPI_H_ */
