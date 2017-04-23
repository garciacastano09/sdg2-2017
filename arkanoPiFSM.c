//------------------------------------------------------------------
// IMPORTS
//------------------------------------------------------------------
#include "arkanoPi_1.h"
#include "fsm.h"
#include "tmr.h"
#include <wiringPi.h>
#include "arkanoPiLib.h"

//------------------------------------------------------------------
// VARIABLES ESTATICAS
//------------------------------------------------------------------
static int flags_arkano_FSM = 0;
static tipo_juego juego;
static int debounce_time=0;

//------------------------------------------------------------------
// FUNCIONES SETUP
//------------------------------------------------------------------
void arkanoPiFSMSetup(fsm_t* arkano_fsm) {
	piLock (FLAGS_ARKANO_KEY);
	flags_arkano_FSM = 0;
	piUnlock (FLAGS_ARKANO_KEY);
	ReseteaJuego(arkano_fsm);

	piLock (STD_IO_BUFFER_KEY);
	printf("\nPULSE P para mover la pelota, I para moverse a la izquierda y O para moverse a la derecha.\n");
	piUnlock (STD_IO_BUFFER_KEY);
}

//------------------------------------------------------------------
// FUNCIONES SETEO DE FLAGS
//------------------------------------------------------------------
void pulsaRaqIzq(void){
	if(millis() < debounce_time){
		debounce_time = millis() + DEBOUNCE_TIME;
		return;
	}
	piLock (FLAGS_ARKANO_KEY);
	flags_arkano_FSM |= FLAG_RAQUETA_IZQUIERDA;
	piUnlock (FLAGS_ARKANO_KEY);

	while(digitalRead(GPIO_RAQ_IZQ) == HIGH){
		delay(1);
	}
	debounce_time = millis() + DEBOUNCE_TIME;
}

void pulsaRaqDer(void){
	if(millis() < debounce_time){
		debounce_time = millis() + DEBOUNCE_TIME;
		return;
	}
	piLock (FLAGS_ARKANO_KEY);
	flags_arkano_FSM |= FLAG_RAQUETA_DERECHA;
	piUnlock (FLAGS_ARKANO_KEY);

	while(digitalRead(GPIO_RAQ_DER) == HIGH){
		delay(1);
	}
	debounce_time = millis() + DEBOUNCE_TIME;

}

void pelotaTmrFinished(fsm_t* this){
	piLock (FLAGS_ARKANO_KEY);
	flags_arkano_FSM |= PELOTA_TIMEOUT;
	piUnlock (FLAGS_ARKANO_KEY);
}

//------------------------------------------------------------------
// FUNCIONES DE ENTRADA
//------------------------------------------------------------------
int compruebaTeclaPulsada(fsm_t* this){
	int result;
	piLock (FLAGS_ARKANO_KEY);
	// result vale 1 si en la variable flags_arkano_FSM se ha marcado FLAG_PELOTA, FLAG_RAQUETA_DERECHA o FLAG_RAQUETA_IZQUIERDA
	result = (flags_arkano_FSM & FLAG_PELOTA) || (flags_arkano_FSM & FLAG_RAQUETA_DERECHA) || (flags_arkano_FSM & FLAG_RAQUETA_IZQUIERDA);
	piUnlock (FLAGS_ARKANO_KEY);
	return result;
}

int compruebaTeclaPelota (fsm_t* this) {
	int result;
	piLock (FLAGS_ARKANO_KEY);
	// result vale 1 si en la variable flags_arkano_FSM se ha marcado FLAG_PELOTA
	result = (flags_arkano_FSM & FLAG_PELOTA);
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

int comprueba_timeout_pelota (fsm_t* this) {
	int result;
	piLock (FLAGS_ARKANO_KEY);
	// result vale 1 si en la variable flags_arkano_FSM se ha marcado FLAG_FINAL_JUEGO
	result = (flags_arkano_FSM & FLAG_TIMEOUT_PELOTA);
	piUnlock (FLAGS_ARKANO_KEY);
	return result;
}

//------------------------------------------------------------------
// FUNCIONES DE ACCION
//------------------------------------------------------------------
void InicializaJuego (void) {
    // void InicializaJuego (void): funcion encargada de llevar a cabo
    // la oportuna inicializaci�n de toda variable o estructura de datos
    // que resulte necesaria para el desarrollo del juego.

	piLock (STD_IO_BUFFER_KEY);
	printf("%s\n", "[LOG] InicializaJuego");
	InicializaArkanoPi((tipo_arkanoPi*)(&(juego.arkanoPi)));
	ActualizaPantalla((tipo_arkanoPi*)(&(juego.arkanoPi)));
	PintaPantallaPorTerminal((tipo_pantalla*)(&(juego.arkanoPi.pantalla)));
	piUnlock (STD_IO_BUFFER_KEY);

	piLock (FLAGS_ARKANO_KEY);
	flags &= ~FLAG_PELOTA;
	flags &= ~FLAG_RAQUETA_DERECHA;
	flags &= ~FLAG_RAQUETA_IZQUIERDA;
	piUnlock (FLAGS_ARKANO_KEY);
}

void MueveRaquetaIzquierda (void) {
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
		PintaRaqueta((tipo_raqueta*)(&(juego.arkanoPi.raqueta)), (tipo_pantalla*)(&(juego.arkanoPi.pantalla)));
	}
	ActualizaPantalla((tipo_arkanoPi*)(&(juego.arkanoPi)));
	//PintaPantallaPorTerminal((tipo_pantalla*)(&(juego.arkanoPi.pantalla)));
	piUnlock (STD_IO_BUFFER_KEY);

	piLock (FLAGS_ARKANO_KEY);
	flags_arkano_FSM &= ~FLAG_RAQUETA_IZQUIERDA;
	piUnlock (FLAGS_ARKANO_KEY);

}

void MueveRaquetaDerecha (void) {
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
		PintaRaqueta((tipo_raqueta*)(&(juego.arkanoPi.raqueta)), (tipo_pantalla*)(&(juego.arkanoPi.pantalla)));
	}
	ActualizaPantalla((tipo_arkanoPi*)(&(juego.arkanoPi)));
	PintaPantallaPorTerminal((tipo_pantalla*)(&(juego.arkanoPi.pantalla)));
	piUnlock (STD_IO_BUFFER_KEY);

	piLock (FLAGS_ARKANO_KEY);
	flags &= ~FLAG_RAQUETA_DERECHA;
	piUnlock (FLAGS_ARKANO_KEY);
}

void MovimientoPelota(void) {
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
	int flag_rebote = ObtenerTipoDeRebote();
	switch(flag_rebote) {
		// Caso en que no existe rebote porque la siguiente casilla esta vacia
	    case NO_REBOTE:
	    	DesplazarPelota();
			break;
	    // Caso en que se choca contra la raqueta
	    case REBOTE_RAQUETA:
	    	ReboteRaqueta();
	    	break;
		// Caso en que se choca contra un lateral: izquierda o derecha
	    case REBOTE_LATERAL:
	    	ReboteLateral();
	        break;
		// Caso en que se choca contra un ladrillo
	    case REBOTE_LADRILLO:
	    	ReboteLadrillo();
	        break;
	    // Caso en que se choca contra el techo (cuando ya no hay ladrillos debajo)
	    case REBOTE_TECHO:
	    	ReboteTecho();
	        break;
	    // Caso en que se choca contra un el limite inferior (pierdes la partida)
	    case REBOTE_PERDIDA:
			// Pierdes
	    	piLock (FLAGS_ARKANO_KEY);
	    	flags_arkano_FSM |= FLAG_FINAL_JUEGO;
	    	piUnlock (FLAGS_ARKANO_KEY);
	        break;
	}
	ActualizaPantalla((tipo_arkanoPi*)(&(juego.arkanoPi)));
	PintaPantallaPorTerminal((tipo_pantalla*)(&(juego.arkanoPi.pantalla)));
	piUnlock (STD_IO_BUFFER_KEY);

	piLock (FLAGS_ARKANO_KEY);
	flags &= ~FLAG_PELOTA;
	piUnlock (FLAGS_ARKANO_KEY);

	timein_mov_pelota=millis();
}

void ReseteaJuego (void) {
    //void ReseteaJuego (void): funci�n encargada de llevar a cabo la
    // reinicializaci�n de cuantas variables o estructuras resulten
    // necesarias para dar comienzo a una nueva partida.

	piLock (STD_IO_BUFFER_KEY);
	printf("%s\n", "[LOG] ReseteaJuego");
	InicializaArkanoPi((tipo_arkanoPi*)(&(juego.arkanoPi)));
	PintaMensajeInicial((tipo_pantalla*)(&(juego.arkanoPi.pantalla)));
	printf("%s\n", "PANTALLA INICIAL");
	PintaPantallaPorTerminal((tipo_pantalla*)(&(juego.arkanoPi.pantalla)));
	piUnlock (STD_IO_BUFFER_KEY);

	piLock (FLAGS_ARKANO_KEY);
	flags_arkano_FSM &= ~FLAG_PELOTA;
	flags_arkano_FSM &= ~FLAG_RAQUETA_DERECHA;
	flags_arkano_FSM &= ~FLAG_RAQUETA_IZQUIERDA;
	piUnlock (FLAGS_ARKANO_KEY);
}

void FinalJuego(void){
    // void FinalJuego (void): funci�n encargada de mostrar en la ventana de
    // terminal los mensajes necesarios para informar acerca del resultado del juego.

	printf("%s\n", "[LOG] FinalJuego");
	if(CalculaLadrillosRestantes((tipo_pantalla*)&(juego.arkanoPi.ladrillos)) == 0){
		printf("%s\n", "HAS GANADO");
	}
	else{
		printf("%s\n", "HAS PERDIDO");
	}
}

//------------------------------------------------------------------
// FUNCIONES SUPPORT
//------------------------------------------------------------------
int ObtenerTipoDeRebote(void){
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

void DesplazarPelota(void){
    // void DesplazarPelota(void): mueve la posicion de la pelota un pixel
    // seg�n lo definido en la trayectoria. La trayectoria no se cambia.

	printf("%s\n", "[LOG] DesplazarPelota");
	juego.arkanoPi.pelota.x = juego.arkanoPi.pelota.x + juego.arkanoPi.pelota.xv;
	juego.arkanoPi.pelota.y = juego.arkanoPi.pelota.y + juego.arkanoPi.pelota.yv;
}

void ReboteLadrillo(void){
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
   	DesplazarPelota();
}

void ReboteTecho(void){
    // void ReboteSuperior(void): cambia la trayectoria de la pelota hacia abajo

	printf("%s\n", "[LOG] ReboteSuperior");
	// Cambiamos trayectoria
	juego.arkanoPi.pelota.yv = - juego.arkanoPi.pelota.yv;
	// Desplazamos pelota
   	DesplazarPelota();
}

void ReboteLateral(void){
    // void ReboteLateral(void): cambia la trayectoria de la pelota hacia el
    // lado contrario

	printf("%s\n", "[LOG] ReboteLateral");
	// Cambiamos trayectoria
	juego.arkanoPi.pelota.xv = - juego.arkanoPi.pelota.xv;
	// Desplazamos pelota
   	DesplazarPelota();
}

void ReboteRaqueta(void){
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
   	DesplazarPelota();
}
