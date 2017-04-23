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

#include "kbhit.h" // para poder detectar teclas pulsadas sin bloqueo y leer las teclas pulsadas

#include "arkanoPi.h"
#include "arkanoPiFSM.h"
#include "refrescoFSM.h"
#include <wiringPi.h>
#include "fsm.h"
#include "tmr.h"


// FLAGS DEL SISTEMA
#define CLK_MS 10 // PERIODO DE ACTUALIZACION DE LA MAQUINA ESTADOS

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

// A 'key' which we can lock and unlock - values are 0 through 3
//	This is interpreted internally as a pthread_mutex by wiringPi
//	which is hiding some of that to make life simple.
#define	STD_IO_BUFFER_KEY	1

//------------------------------------------------------
// FUNCIONES DE INICIALIZACION
//------------------------------------------------------
int systemSetup (void);
void delayUntil (unsigned int next);

//------------------------------------------------------
// SUBRUTINAS DE ATENCION A LAS INTERRUPCIONES
//------------------------------------------------------
PI_THREAD (thread_explora_teclado);

#endif /* ARKANOPI_H_ */
