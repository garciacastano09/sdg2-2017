#ifndef _REFRESCOFSM_H_
#define _REFRESCOFSM_H_

#include "arkanoPi.h"
#include "fsm.h"
#include "tmr.h"
#include <wiringPi.h>

// FLAGS FSM
#define FLAG_TIMEOUT_REFRESCO		0x1
#define REFRESCO_TIMEOUT 1 //tiempo de exploraci�n de columnas

#define	FLAGS_REFRESCO_KEY	1
// #define	STD_IO_BUFFER_KEY	2

//------------------------------------------------------------------
// FUNCIONES SETUP
//------------------------------------------------------------------
void refrescoFSMSetup(fsm_t* refresco_fsm);

//------------------------------------------------------------------
// FUNCIONES SETEO DE FLAGS
//------------------------------------------------------------------
void refresco_tmr_finished(fsm_t* this);

//------------------------------------------------------------------
// FUNCION DE ENTRADA
//------------------------------------------------------------------
int comprueba_timeout_refresco(fsm_t* this);

//------------------------------------------------------------------
// FUNCION DE ACCION
//------------------------------------------------------------------
static void refrescar_leds(fsm_t* this);

#endif
