//------------------------------------------------------------------
// IMPORTS
//------------------------------------------------------------------
#include "arkanoPi_1.h"
#include <wiringPi.h>
#include "fsm.h"
#include "tmr.h"

//------------------------------------------------------------------
// VARIABLES ESTATICAS
//------------------------------------------------------------------
static int gpio_col[4]={GPIO_COL_1,GPIO_COL_2,GPIO_COL_3,GPIO_COL_4};
static int gpio_row[7]={GPIO_ROW_1,GPIO_ROW_2,GPIO_ROW_3,GPIO_ROW_4,
		GPIO_ROW_5,GPIO_ROW_6,GPIO_ROW_7};

static int flags_arkano_FSM = 0;
static tipo_juego juego;
static int debounce_time=0;

static int col_counter=0;
static int columna=0;


//------------------------------------------------------------------
// REFRESCO DE LEDS
//------------------------------------------------------------------
static void refrescarLeds(tmr_t* this){
    col_counter++;
    columna = col_counter%10;
    activaFilasLed(&(juego.arkanoPi.pantalla),&(columna));
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
	tmr_startms((tmr_t*)this, REFRESCO_TIMEOUT);
}

void activaFilasLed (tipo_pantalla* p_pantalla, int* columna){
    printf("%s\n", "[LOG] PintaFilasLed");
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

//------------------------------------------------------------------
// ARKANOPI FSM: FUNCIONES SETUP
//------------------------------------------------------------------
void arkanoPiFSMSetup(fsm_t* arkano_fsm) {
	piLock (FLAGS_ARKANO_KEY);
	flags_arkano_FSM = 0;
	piUnlock (FLAGS_ARKANO_KEY);
	reseteaJuego(arkano_fsm);

	piLock (STD_IO_BUFFER_KEY);
	printf("\nPULSE P para mover la pelota, I para moverse a la izquierda y O para moverse a la derecha.\n");
	piUnlock (STD_IO_BUFFER_KEY);
}

//------------------------------------------------------------------
// ARKANOPI FSM: SETEO DE FLAGS
//------------------------------------------------------------------
void pulsaRaqIzq(void){
	// if(millis() < debounce_time){
	// 	debounce_time = millis() + DEBOUNCE_TIME;
	// 	return;
	// }
	piLock (FLAGS_ARKANO_KEY);
	flags_arkano_FSM |= FLAG_RAQUETA_IZQUIERDA;
	piUnlock (FLAGS_ARKANO_KEY);

	// while(digitalRead(GPIO_RAQ_IZQ) == HIGH){
	// 	delay(1);
	// }
	// debounce_time = millis() + DEBOUNCE_TIME;
}

void pulsaRaqDer(void){
	// if(millis() < debounce_time){
	// 	debounce_time = millis() + DEBOUNCE_TIME;
	// 	return;
	// }
	piLock (FLAGS_ARKANO_KEY);
	flags_arkano_FSM |= FLAG_RAQUETA_DERECHA;
	piUnlock (FLAGS_ARKANO_KEY);

	// while(digitalRead(GPIO_RAQ_DER) == HIGH){
	// 	delay(1);
	// }
	// debounce_time = millis() + DEBOUNCE_TIME;

}

void pelotaTmrFinished(tmr_t* this){
	piLock (FLAGS_ARKANO_KEY);
	flags_arkano_FSM |= FLAG_TIMEOUT_PELOTA;
	piUnlock (FLAGS_ARKANO_KEY);

	tmr_startms((tmr_t*)this, PELOTA_TIMEOUT);
}

//------------------------------------------------------------------
// ARKANOPI FSM: FUNCIONES DE ENTRADA
//------------------------------------------------------------------
int compruebaTeclaPulsada(fsm_t* this){
	int result;
	piLock (FLAGS_ARKANO_KEY);
	// result vale 1 si en la variable flags_arkano_FSM se ha marcado FLAG_PELOTA, FLAG_RAQUETA_DERECHA o FLAG_RAQUETA_IZQUIERDA
	result = (flags_arkano_FSM & FLAG_PELOTA) || (flags_arkano_FSM & FLAG_RAQUETA_DERECHA) || (flags_arkano_FSM & FLAG_RAQUETA_IZQUIERDA);
	piUnlock (FLAGS_ARKANO_KEY);
	return result;
}

int compruebaTeclaRaquetaDerecha (fsm_t* this) {
	int result;
	// result vale 1 si en la variable flags_arkano_FSM se ha marcado FLAG_RAQUETA_DERECHA
	piLock (FLAGS_ARKANO_KEY);
	result = (flags_arkano_FSM & FLAG_RAQUETA_DERECHA);
	piUnlock (FLAGS_ARKANO_KEY);
	return result;
}

int compruebaTeclaRaquetaIzquierda (fsm_t* this) {
	int result;
	// result vale 1 si en la variable flags_arkano_FSM se ha marcado FLAG_RAQUETA_IZQUIERDA
	piLock (FLAGS_ARKANO_KEY);
	result = (flags_arkano_FSM & FLAG_RAQUETA_IZQUIERDA);
	piUnlock (FLAGS_ARKANO_KEY);
	return result;
}

int compruebaFinalJuego (fsm_t* this) {
	int result;
	piLock (FLAGS_ARKANO_KEY);
	// result vale 1 si en la variable flags_arkano_FSM se ha marcado FLAG_FINAL_JUEGO
	result = (flags_arkano_FSM & FLAG_FINAL_JUEGO);
	piUnlock (FLAGS_ARKANO_KEY);
	return result;
}

int compruebaTimeoutPelota (fsm_t* this) {
	int result;
	piLock (FLAGS_ARKANO_KEY);
	// result vale 1 si en la variable flags_arkano_FSM se ha marcado FLAG_FINAL_JUEGO
	result = (flags_arkano_FSM & FLAG_PELOTA);
	piUnlock (FLAGS_ARKANO_KEY);
	return result;
}

//------------------------------------------------------------------
// ARKANOPI FSM: FUNCIONES DE ACCION
//------------------------------------------------------------------
void inicializaJuego (fsm_t* this) {
    // void InicializaJuego (void): funcion encargada de llevar a cabo
    // la oportuna inicializaci�n de toda variable o estructura de datos
    // que resulte necesaria para el desarrollo del juego.

	piLock (STD_IO_BUFFER_KEY);
	printf("%s\n", "[LOG] InicializaJuego");
	inicializaArkanoPi((tipo_arkanoPi*)(&(juego.arkanoPi)));
	actualizaPantalla((tipo_arkanoPi*)(&(juego.arkanoPi)));
	pintaPantallaPorTerminal((tipo_pantalla*)(&(juego.arkanoPi.pantalla)));
	piUnlock (STD_IO_BUFFER_KEY);

	piLock (FLAGS_ARKANO_KEY);
	flags &= ~FLAG_PELOTA;
	flags &= ~FLAG_RAQUETA_DERECHA;
	flags &= ~FLAG_RAQUETA_IZQUIERDA;
	piUnlock (FLAGS_ARKANO_KEY);
}

void mueveRaquetaIzquierda (fsm_t* this) {
    // void MueveRaquetaIzquierda (void): funcion encargada de ejecutar
    // el movimiento hacia la izquierda contemplado para la raqueta.
    // Debe garantizar la viabilidad del mismo mediante la comprobaci�n
    // de que la nueva posici�n correspondiente a la raqueta no suponga
    // que �sta rebase o exceda los l�mites definidos para el �rea de juego
    // (i.e. al menos uno de los leds que componen la raqueta debe permanecer
    // visible durante todo el transcurso de la partida).

	piLock (STD_IO_BUFFER_KEY);
	printf("%s\n", "[LOG] MueveRaquetaIzquierda");
	// Esto asegura que la raqueta se mantenga en la fila mas baja de la matriz
	juego.arkanoPi.raqueta.y=MATRIZ_ALTO-1;
	if(juego.arkanoPi.raqueta.x < MIN_X_RAQUETA){
		printf("%s\n", "[LOG] MueveRaquetaIzquierda: Movimiento a la izquierda imposible");
	}
	else{
		juego.arkanoPi.raqueta.x--;
		pintaRaqueta((tipo_raqueta*)(&(juego.arkanoPi.raqueta)), (tipo_pantalla*)(&(juego.arkanoPi.pantalla)));
	}
	actualizaPantalla((tipo_arkanoPi*)(&(juego.arkanoPi)));
	//PintaPantallaPorTerminal((tipo_pantalla*)(&(juego.arkanoPi.pantalla)));
	piUnlock (STD_IO_BUFFER_KEY);

	piLock (FLAGS_ARKANO_KEY);
	flags_arkano_FSM &= ~FLAG_RAQUETA_IZQUIERDA;
	piUnlock (FLAGS_ARKANO_KEY);

}

void mueveRaquetaDerecha (fsm_t* this) {
    // void MueveRaquetaDerecha (void): funci�n similar a la anterior
    // encargada del movimiento hacia la derecha.

	piLock (STD_IO_BUFFER_KEY);
	printf("%s\n", "[LOG] MueveRaquetaDerecha");
	// Esto asegura que la raqueta se mantenga en la fila mas baja de la matriz
	juego.arkanoPi.raqueta.y=MATRIZ_ALTO-1;
	if(juego.arkanoPi.raqueta.x > MAX_X_RAQUETA){
		printf("%s\n", "[LOG] MueveRaquetaIzquierda: Movimiento a la derecha imposible");
	}
	else{
		juego.arkanoPi.raqueta.x++;
		pintaRaqueta((tipo_raqueta*)(&(juego.arkanoPi.raqueta)), (tipo_pantalla*)(&(juego.arkanoPi.pantalla)));
	}
	actualizaPantalla((tipo_arkanoPi*)(&(juego.arkanoPi)));
	pintaPantallaPorTerminal((tipo_pantalla*)(&(juego.arkanoPi.pantalla)));
	piUnlock (STD_IO_BUFFER_KEY);

	piLock (FLAGS_ARKANO_KEY);
	flags &= ~FLAG_RAQUETA_DERECHA;
	piUnlock (FLAGS_ARKANO_KEY);
}

void movimientoPelota(fsm_t* this) {
    // void MovimientoPelota (void): funci�n encargada de actualizar la
    // posici�n de la pelota conforme a la trayectoria definida para �sta.
    // Para ello deber� identificar los posibles rebotes de la pelota para,
    // en ese caso, modificar su correspondiente trayectoria (los rebotes
    // detectados contra alguno de los ladrillos implicar�n adicionalmente
    // la eliminaci�n del ladrillo). Del mismo modo, deber� tambi�n
    // identificar las situaciones en las que se d� por finalizada la partida:
    // bien porque el jugador no consiga devolver la pelota, y por tanto �sta
    // rebase el l�mite inferior del �rea de juego, bien porque se agoten
    // los ladrillos visibles en el �rea de juego.

	piLock (STD_IO_BUFFER_KEY);

	printf("%s\n", "[LOG] MovimientoPelota");
	printf("Estado: %i\n", juego.estado);
	// Elegimos lo que se hace segun el tipo de rebote
	int flag_rebote = obtenerTipoDeRebote();
	switch(flag_rebote) {
		// Caso en que no existe rebote porque la siguiente casilla esta vacia
	    case NO_REBOTE:
	    	desplazarPelota();
			break;
	    // Caso en que se choca contra la raqueta
	    case REBOTE_RAQUETA:
	    	reboteRaqueta();
	    	break;
		// Caso en que se choca contra un lateral: izquierda o derecha
	    case REBOTE_LATERAL:
	    	reboteLateral();
	        break;
		// Caso en que se choca contra un ladrillo
	    case REBOTE_LADRILLO:
	    	reboteLadrillo();
	        break;
	    // Caso en que se choca contra el techo (cuando ya no hay ladrillos debajo)
	    case REBOTE_TECHO:
	    	reboteTecho();
	        break;
	    // Caso en que se choca contra un el limite inferior (pierdes la partida)
	    case REBOTE_PERDIDA:
			// Pierdes
	    	piLock (FLAGS_ARKANO_KEY);
	    	flags_arkano_FSM |= FLAG_FINAL_JUEGO;
	    	piUnlock (FLAGS_ARKANO_KEY);
	        break;
	}
	actualizaPantalla((tipo_arkanoPi*)(&(juego.arkanoPi)));
	pintaPantallaPorTerminal((tipo_pantalla*)(&(juego.arkanoPi.pantalla)));
	piUnlock (STD_IO_BUFFER_KEY);

	piLock (FLAGS_ARKANO_KEY);
	flags &= ~FLAG_PELOTA;
	piUnlock (FLAGS_ARKANO_KEY);
}

void reseteaJuego (fsm_t* this) {
    //void ReseteaJuego (void): funci�n encargada de llevar a cabo la
    // reinicializaci�n de cuantas variables o estructuras resulten
    // necesarias para dar comienzo a una nueva partida.

	piLock (STD_IO_BUFFER_KEY);
	printf("%s\n", "[LOG] ReseteaJuego");
	inicializaArkanoPi((tipo_arkanoPi*)(&(juego.arkanoPi)));
	pintaMensajeInicial((tipo_pantalla*)(&(juego.arkanoPi.pantalla)));
	printf("%s\n", "PANTALLA INICIAL");
	pintaPantallaPorTerminal((tipo_pantalla*)(&(juego.arkanoPi.pantalla)));
	piUnlock (STD_IO_BUFFER_KEY);

	piLock (FLAGS_ARKANO_KEY);
	flags_arkano_FSM &= ~FLAG_PELOTA;
	flags_arkano_FSM &= ~FLAG_RAQUETA_DERECHA;
	flags_arkano_FSM &= ~FLAG_RAQUETA_IZQUIERDA;
	piUnlock (FLAGS_ARKANO_KEY);
}

void finalJuego(fsm_t* this){
    // void FinalJuego (void): funci�n encargada de mostrar en la ventana de
    // terminal los mensajes necesarios para informar acerca del resultado del juego.

	printf("%s\n", "[LOG] FinalJuego");
	if(calculaLadrillosRestantes((tipo_pantalla*)&(juego.arkanoPi.ladrillos)) == 0){
		printf("%s\n", "HAS GANADO");
	}
	else{
		printf("%s\n", "HAS PERDIDO");
	}
}

//------------------------------------------------------------------
// ARKANOPI FSM: FUNCIONES SUPPORT
//------------------------------------------------------------------
int obtenerTipoDeRebote(void){
    // int ObtenerTipoDeRebote(void): verifica si la proxima casilla causar�
    // un rebote y de qu� tipo

	printf("%s\n", "[LOG] ObtenerTipoDeRebote");
	// Calculo de las nuevas coordenadas que tendr�a la pelota en el pr�ximo movimiento
	int nuevo_y = juego.arkanoPi.pelota.y + juego.arkanoPi.pelota.yv;
	int nuevo_x = juego.arkanoPi.pelota.x + juego.arkanoPi.pelota.xv;

	int raqueta_x = juego.arkanoPi.raqueta.x;

	// Caso de perdida de pelota
	if(nuevo_y == MATRIZ_ALTO){
		printf("%s\n", "[LOG] ProximoMovimiento: REBOTE PERDIDA");
		return REBOTE_PERDIDA;
	}
	// Caso de lateral de la pantalla
	else if(nuevo_x == MATRIZ_ANCHO || nuevo_x == -1){
		printf("%s\n", "[LOG] ProximoMovimiento: REBOTE LATERAL");
		return REBOTE_LATERAL;
	}
	// Caso de techo de la pantalla
	else if(nuevo_y == -1){
		printf("%s\n", "[LOG] ProximoMovimiento: REBOTE TECHO");
		return REBOTE_TECHO;
	}
	// Caso de choque contra raqueta
	else if(nuevo_y == MATRIZ_ALTO - 1 && (nuevo_x == raqueta_x || nuevo_x == raqueta_x + 1 || nuevo_x == raqueta_x - 1)){
		printf("%s\n", "[LOG] ProximoMovimiento: REBOTE RAQUETA");
		return REBOTE_RAQUETA;
	}
	// Caso de choque contra ladrillo
	else if(juego.arkanoPi.ladrillos.matriz[nuevo_x][nuevo_y]){
		printf("%s\n", "[LOG] ProximoMovimiento: REBOTE LADRILLO");
		return REBOTE_LADRILLO;
	}
	else{
		printf("%s\n", "[LOG] ProximoMovimiento: NO REBOTE");
		return NO_REBOTE;
	}
}

void desplazarPelota(void){
    // void DesplazarPelota(void): mueve la posicion de la pelota un pixel
    // seg�n lo definido en la trayectoria. La trayectoria no se cambia.

	printf("%s\n", "[LOG] DesplazarPelota");
	juego.arkanoPi.pelota.x = juego.arkanoPi.pelota.x + juego.arkanoPi.pelota.xv;
	juego.arkanoPi.pelota.y = juego.arkanoPi.pelota.y + juego.arkanoPi.pelota.yv;
}

void reboteLadrillo(void){
    // void ReboteLadrillo(void): elimina el ladrillo y cambia la trayectoria de
    // la pelota hacia abajo

	printf("%s\n", "[LOG] ReboteLadrillo");
	// Eliminamos el ladrillo
	juego.arkanoPi.ladrillos.matriz
		[juego.arkanoPi.pelota.x + juego.arkanoPi.pelota.xv]
		[juego.arkanoPi.pelota.y + juego.arkanoPi.pelota.yv] = 0;
	// Cambiamos trayectoria
	juego.arkanoPi.pelota.yv = - juego.arkanoPi.pelota.yv;
	// Desplazamos pelota
   	desplazarPelota();
}

void reboteTecho(void){
    // void ReboteSuperior(void): cambia la trayectoria de la pelota hacia abajo

	printf("%s\n", "[LOG] ReboteSuperior");
	// Cambiamos trayectoria
	juego.arkanoPi.pelota.yv = - juego.arkanoPi.pelota.yv;
	// Desplazamos pelota
   	desplazarPelota();
}

void reboteLateral(void){
    // void ReboteLateral(void): cambia la trayectoria de la pelota hacia el
    // lado contrario

	printf("%s\n", "[LOG] ReboteLateral");
	// Cambiamos trayectoria
	juego.arkanoPi.pelota.xv = - juego.arkanoPi.pelota.xv;
	// Desplazamos pelota
   	desplazarPelota();
}

void reboteRaqueta(void){
    // void ReboteRaqueta(void): cambia la trayectoria de la pelota con la logica
    // correspondiente

	printf("%s\n", "[LOG] ReboteRaqueta");
	int nueva_posicion_x = juego.arkanoPi.pelota.x + juego.arkanoPi.pelota.xv;
	// Cambiamos trayectoria
	// Caso en que la pelota choca a la IZQUIERDA de la raqueta
	if(nueva_posicion_x == juego.arkanoPi.raqueta.x - 1){
			juego.arkanoPi.pelota.xv = - 1;
	}
	// Caso en que la pelota choca al CENTRO de la raqueta
	else if(nueva_posicion_x == juego.arkanoPi.raqueta.x){
			juego.arkanoPi.pelota.xv = 0;
	}
	// Caso en que la pelota choca a la DERECHA de la raqueta
	else if(nueva_posicion_x == juego.arkanoPi.raqueta.x + 1){
			juego.arkanoPi.pelota.xv = 1;
	}
	else{
		printf("%s\n", "[LOG] ReboteRaqueta: ");
	}
	// La pelota en cualquier caso va hacia arriba
	juego.arkanoPi.pelota.yv = - 1;
   	desplazarPelota();
}


void delayUntil (unsigned int next) {
	// espera hasta la pr�xima activaci�n del reloj
	unsigned int now = millis();
	if (next > now) {
		delay (next - now);
    }
}

//------------------------------------------------------
// ARKANOPI: FUNCIONES DE INICIALIZACION
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

	tmr_t* refresco_tmr = tmr_new(refrescarLeds);
	tmr_startms((tmr_t*)refresco_tmr, REFRESCO_TIMEOUT);

	tmr_t* pelota_tmr = tmr_new(pelotaTmrFinished);
	tmr_startms((tmr_t*)pelota_tmr, PELOTA_TIMEOUT);

	// Lanzamos thread para exploracion del teclado convencional del PC
	// x = piThreadCreate (thread_explora_teclado);
	// if (x != 0) {
	// 	printf ("it didn't start!!!\n");
	// 	piUnlock (STD_IO_BUFFER_KEY);
	// 	return -1;
    // }
	piUnlock (STD_IO_BUFFER_KEY);
	return 1;
}


//------------------------------------------------------
// ARKANOPI: PI_THREAD (thread_explora_teclado): Thread function for keystrokes detection and interpretation
//------------------------------------------------------
// PI_THREAD (thread_explora_teclado) {
// 	int tecla_pulsada;
//
// 	while(1) {
// 		delay(10); // Wiring Pi function: pauses program execution for at least 10 ms
// 		piLock (STD_IO_BUFFER_KEY);
// 		if(kbhit()) {
// 			tecla_pulsada = kbread();
// 			printf("\nTecla %c\n", tecla_pulsada);
// 			switch(tecla_pulsada) {
// 				case 'i':  // Mover raqueta a la izquierda
// 					pulsaRaqIzq();
// 					break;
//
// 				case 'o':  // Mover Raqueta a la derecha
// 					pulsaRaqDer();
// 					break;
//
// 				case 'p':  // Mover Raqueta a la derecha
// 					pelotaTmrFinished();
// 					break;
//
// 				case 'q': // Salir
// 					exit(0);
// 					break;
//
// 				default:
// 					printf("INVALID KEY!!!\n");
// 					break;
// 			}
// 		}
// 		piUnlock (STD_IO_BUFFER_KEY);
// 	}
// }

//------------------------------------------------------------------
// ARKANOPI: FUNCION PRINCIPAL MAIN
//------------------------------------------------------------------
int main ()
{
	//printf("%s\n", "[LOG] main");
	unsigned int next;
	// Maquina de estados: lista de transiciones
	// {EstadoOrigen,Funci�nDeEntrada,EstadoDestino,Funci�nDeSalida}
	fsm_trans_t arkano_tabla[] = {
		{WAIT_START, compruebaTeclaPulsada, WAIT_PUSH, inicializaJuego},
		{WAIT_PUSH, compruebaTimeoutPelota, WAIT_PUSH, movimientoPelota},
		{WAIT_PUSH, compruebaTeclaRaquetaIzquierda, WAIT_PUSH, mueveRaquetaIzquierda},
		{WAIT_PUSH, compruebaTeclaRaquetaDerecha, WAIT_PUSH, mueveRaquetaDerecha},
		{WAIT_PUSH, compruebaFinalJuego, WAIT_END, finalJuego},
		{WAIT_END, compruebaTeclaPulsada, WAIT_PUSH, reseteaJuego},
		{ -1, NULL, -1, NULL },
	};
	// Configuracion e inicializacion del sistema
	arkanoPiFSMSetup (arkano_fsm);
	systemSetup();

	next = millis();
	while (1) {
		fsm_fire (arkano_fsm);
		next += CLK_MS;
		delay_until (next);
	}
	fsm_destroy (arkano_fsm);
	// tmr_destroy((tmr_t*)(refresco_fsm->user_data));
}
