//------------------------------------------------------------------
// IMPORTS
//------------------------------------------------------------------
#include "arkanoPi.h"
#include "arkanoPiFSM.h"
#include "refrescoFSM.h"
#include <wiringPi.h>
#include "fsm.h"
#include "tmr.h"

//------------------------------------------------------------------
// VARIABLES ESTATICAS
//------------------------------------------------------------------
static int gpio_col[4]={GPIO_COL_1,GPIO_COL_2,GPIO_COL_3,GPIO_COL_4};
static int gpio_row[7]={GPIO_ROW_1,GPIO_ROW_2,GPIO_ROW_3,GPIO_ROW_4,
		GPIO_ROW_5,GPIO_ROW_6,GPIO_ROW_7};

void delayUntil (unsigned int next) {
	// espera hasta la pr�xima activaci�n del reloj
	unsigned int now = millis();
	if (next > now) {
		delay (next - now);
    }
}

//------------------------------------------------------
// FUNCIONES DE INICIALIZACION
//------------------------------------------------------
int systemSetup (void) {
	// int systemSetup (void): procedimiento de configuracion del sistema.
	// Realizar�, entra otras, todas las operaciones necesarias para:
	// configurar el uso de posibles librer�as (e.g. Wiring Pi),
	// configurar las interrupciones externas asociadas a los pines GPIO,
	// configurar las interrupciones peri�dicas y sus correspondientes temporizadores,
	// crear, si fuese necesario, los threads adicionales que pueda requerir el sistema

	int x = 0;
	piLock (STD_IO_BUFFER_KEY);
	printf("%s\n", "[LOG] systemSetup");
	// sets up the wiringPi library
	if (wiringPiSetupGpio () < 0) {
		printf ("Unable to setup wiringPi\n");
		piUnlock (STD_IO_BUFFER_KEY);
		return -1;
	}
	int i=0;
	for (i=0; i<4; i++){
		pinMode(gpio_col[i], OUTPUT);
	}
	for (i=0; i<7; i++){
		pinMode(gpio_row[i], OUTPUT);
		pullUpDnControl (gpio_row[i], PUD_DOWN ); // todas las filas inicialmente a pull down
	}
	pinMode(GPIO_RAQ_IZQ, INPUT);
	pinMode(GPIO_RAQ_DER, INPUT);
	wiringPiISR(GPIO_RAQ_IZQ, INT_EDGE_FALLING, PulsaRaqIzq);
	wiringPiISR(GPIO_RAQ_DER, INT_EDGE_FALLING, PulsaRaqDer);
	// Lanzamos thread para exploracion del teclado convencional del PC
	x = piThreadCreate (thread_explora_teclado);
	if (x != 0) {
		printf ("it didn't start!!!\n");
		piUnlock (STD_IO_BUFFER_KEY);
		return -1;
    }
	piUnlock (STD_IO_BUFFER_KEY);
	return 1;
}


//------------------------------------------------------
// PI_THREAD (thread_explora_teclado): Thread function for keystrokes detection and interpretation
//------------------------------------------------------
PI_THREAD (thread_explora_teclado) {
	int tecla_pulsada;

	while(1) {
		delay(10); // Wiring Pi function: pauses program execution for at least 10 ms
		piLock (STD_IO_BUFFER_KEY);
		if(kbhit()) {
			tecla_pulsada = kbread();
			printf("\nTecla %c\n", tecla_pulsada);
			switch(tecla_pulsada) {
				case 'i':  // Mover raqueta a la izquierda
					pulsaRaqIzq();
					break;

				case 'o':  // Mover Raqueta a la derecha
					pulsaRaqDer();
					break;

				case 'p':  // Mover Raqueta a la derecha
					pelotaTmrFinished();
					break;

				case 'q': // Salir
					exit(0);
					break;

				default:
					printf("INVALID KEY!!!\n");
					break;
			}
		}
		piUnlock (STD_IO_BUFFER_KEY);
	}
}

//------------------------------------------------------------------
// FUNCION PRINCIPAL MAIN
//------------------------------------------------------------------
int main ()
{
	//printf("%s\n", "[LOG] main");
	unsigned int next;
	// Maquina de estados: lista de transiciones
	// {EstadoOrigen,Funci�nDeEntrada,EstadoDestino,Funci�nDeSalida}
	// DECLARACION TEMPORIZADORES
	tmr_t* refresco_tmr = tmr_new(refresco_tmr_finished);

	// DECLARACION TABLAS FSM
	fsm_trans_t arkano_tabla[] = {
		{WAIT_START, comprueba_tecla_pulsada, WAIT_PUSH, InicializaJuego},
		// {WAIT_PUSH, timer_mov_pelota_finished, WAIT_PUSH, MovimientoPelota},
		{WAIT_PUSH, comprueba_tecla_pelota, WAIT_PUSH, MovimientoPelota},
		{WAIT_PUSH, comprueba_tecla_raqueta_izquierda, WAIT_PUSH, MueveRaquetaIzquierda},
		{WAIT_PUSH, comprueba_tecla_raqueta_derecha, WAIT_PUSH, MueveRaquetaDerecha},
		{WAIT_PUSH, comprueba_final_juego, WAIT_END, FinalJuego},
		{WAIT_END, comprueba_tecla_pulsada, WAIT_PUSH, ReseteaJuego},
		{ -1, NULL, -1, NULL },
	};
	fsm_trans_t refresco_tabla[] = {
		{REFRESCO, comprueba_timeout_refresco, REFRESCO, refrescar_leds },
		{ -1, NULL, -1, NULL }, };

	// DECLARACION FSM
	fsm_t* arkano_fsm = fsm_new (WAIT_START, arkano_tabla, NULL);
	fsm_t* refresco_fsm = fsm_new(REFRESCO_COL, refresco_tabla, refresco_tmr);

	// Configuracion e inicializacion del sistema
	arkanoPiFSMSetup (arkano_fsm);
	refrescoFSMSetup (arkano_fsm);
	systemSetup();

	// COMIENZO TEMPORIZADORES
	next = millis();
	tmr_startms((tmr_t*)(refresco_fsm->user_data), REFRESCO_TIMEOUT);

	while (1) {
		fsm_fire (arkano_fsm);
		fsm_fire (refresco_fsm);
		next += CLK_MS;
		delay_until (next);
	}
	fsm_destroy (arkano_fsm);
	fsm_destroy (refresco_fsm);
	tmr_destroy((tmr_t*)(refresco_fsm->user_data));
}
