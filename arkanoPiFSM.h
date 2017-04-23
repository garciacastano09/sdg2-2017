#ifndef _ARKANOPIFSM_H_
#define _ARKANOPIFSM_H_

#include "arkanoPi_1.h"
#include "fsm.h"
#include "tmr.h"
#include <wiringPi.h>
#include "arkanoPiLib.h"

// FLAGS DE LA FSM
#define FLAG_PELOTA			0x01
#define FLAG_RAQUETA_DERECHA			0x02
#define FLAG_RAQUETA_IZQUIERDA			0x04
#define FLAG_FINAL_JUEGO		0x8
#define FLAG_TIMEOUT_PELOTA		0x16

#define DEBOUNCE_TIME  5
#define PELOTA_TIMEOUT 500 //tiempo de movimiento de pelota

// A 'key' which we can lock and unlock - values are 0 through 3
//	This is interpreted internally as a pthread_mutex by wiringPi
//	which is hiding some of that to make life simple.
#define	FLAGS_ARKANO_KEY	1
#define	STD_IO_BUFFER_KEY	2

typedef enum {
	WAIT_START=0,
	WAIT_PUSH=1,
	WAIT_END=2} tipo_estados_juego;

typedef struct {
	tipo_arkanoPi arkanoPi;
	tipo_estados_juego estado;
	char teclaPulsada;
} tipo_juego;

//------------------------------------------------------------------
// FUNCIONES SETUP
//------------------------------------------------------------------
void arkanoPiFSMSetup(fsm_t* arkano_fsm);

//------------------------------------------------------------------
// FUNCIONES SETEO DE FLAGS
//------------------------------------------------------------------
void pulsaRaqIzq(void);
void pulsaRaqDer(void);
void pelotaTmrFinished(fsm_t* this);

//------------------------------------------------------
// FUNCIONES DE ENTRADA
//------------------------------------------------------
int compruebaTeclaPelota (fsm_t* this);
int compruebaTeclaRaquetaDerecha (fsm_t* this);
int compruebaTeclaRaquetaIzquierda (fsm_t* this);
int compruebaFinalJuego (fsm_t* this);
int compruebaTeclaPulsada(fsm_t* this);
int compruebaTimeoutPelota (fsm_t* this);

//------------------------------------------------------
// FUNCIONES DE ACCION
//------------------------------------------------------
void inicializaJuego (void);
void finalJuego (void);
void reseteaJuego (void);
void mueveRaquetaIzquierda (void);
void mueveRaquetaDerecha (void);
void movimientoPelota (void);

//------------------------------------------------------------------
// FUNCIONES SUPPORT
//------------------------------------------------------------------
int obtenerTipoDeRebote(void);
void desplazarPelota(void);
void reboteLadrillo(void);
void reboteTecho(void);
void reboteLateral(void);
void reboteRaqueta(void);


#endif /* ARKANOPIFSM_H_ */
