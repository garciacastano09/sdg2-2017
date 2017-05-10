//------------------------------------------------------------------
// IMPORTS
//------------------------------------------------------------------
#include "arkanoPi.h"
#include <wiringPi.h>
#include <wiringPiSPI.h>
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

static int debounce_time = 0;

static int col_counter=0;
static int columna=0;

// espera hasta la pr�xima activaci�n del reloj
void delay_until (unsigned int next) {
	unsigned int now = millis();
	if (next > now) {
		delay (next - now);
    }
}
//------------------------------------------------------------------
// REFRESCO DE LEDS
//------------------------------------------------------------------
void refrescarLeds(union sigval value){
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
	tmr_startms((tmr_t*)juego.temporizadores.refresco_tmr, REFRESCO_TIMEOUT);
}

void activaFilasLed (tipo_pantalla* p_pantalla, int* columna){
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

//------------------------------------------------------------------
// ARKANOPI FSM: FUNCIONES SETUP
//------------------------------------------------------------------
void arkanoPiFSMSetup(fsm_t* arkano_fsm) {
	piLock (FLAGS_ARKANO_KEY);
	flags_arkano_FSM = 0;
	piUnlock (FLAGS_ARKANO_KEY);
	reseteaJuego(arkano_fsm);

	/*piLock (STD_IO_BUFFER_KEY);
	printf("\nPULSE P para mover la pelota, I para moverse a la izquierda y O para moverse a la derecha.\n");
	piUnlock (STD_IO_BUFFER_KEY);*/
}

//------------------------------------------------------------------
// ARKANOPI FSM: SETEO DE FLAGS
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

void pelotaTmrFinished(union sigval value){
	piLock (FLAGS_ARKANO_KEY);
	flags_arkano_FSM |= FLAG_PELOTA;
	piUnlock (FLAGS_ARKANO_KEY);

	tmr_startms((tmr_t*)juego.temporizadores.pelota_tmr, PELOTA_TIMEOUT);
}

void joystickTmrFinished(union sigval value){
	piLock (FLAGS_ARKANO_KEY);
	flags_arkano_FSM |= FLAG_JOYSTICK;
	piUnlock (FLAGS_ARKANO_KEY);

	tmr_startms((tmr_t*)juego.temporizadores.joystick_tmr, JOYSTICK_TIMEOUT);
}

//------------------------------------------------------------------
// ARKANOPI FSM: FUNCIONES DE ENTRADA
//------------------------------------------------------------------
int compruebaTeclaPulsada(fsm_t* this){
	int result;
	piLock (FLAGS_ARKANO_KEY);
	// result vale 1 si en la variable flags_arkano_FSM se ha marcado FLAG_PELOTA, FLAG_RAQUETA_DERECHA o FLAG_RAQUETA_IZQUIERDA
	result = (flags_arkano_FSM & FLAG_RAQUETA_DERECHA) || (flags_arkano_FSM & FLAG_RAQUETA_IZQUIERDA);
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

int compruebaTimeoutJoystick (fsm_t* this) {
	int result;
	piLock (FLAGS_ARKANO_KEY);
	// result vale 1 si en la variable flags_arkano_FSM se ha marcado FLAG_FINAL_JUEGO
	result = (flags_arkano_FSM & FLAG_JOYSTICK);
	piUnlock (FLAGS_ARKANO_KEY);
	return result;
}

//------------------------------------------------------------------
// ARKANOPI FSM: FUNCIONES DE ACCION
//------------------------------------------------------------------
void inicializaJuego (fsm_t* this) {
    // void InicializaJuego (void): funcion encargada de llevar a cabo
    // la oportuna inicializaciï¿½n de toda variable o estructura de datos
    // que resulte necesaria para el desarrollo del juego.

	piLock (STD_IO_BUFFER_KEY);
	printf("%s\n", "[LOG] InicializaJuego");
	inicializaArkanoPi((tipo_arkanoPi*)(&(juego.arkanoPi)));
	actualizaPantalla((tipo_arkanoPi*)(&(juego.arkanoPi)));
	// pintaPantallaPorTerminal((tipo_pantalla*)(&(juego.arkanoPi.pantalla)));
	piUnlock (STD_IO_BUFFER_KEY);

	piLock (FLAGS_ARKANO_KEY);
	flags_arkano_FSM &= ~FLAG_PELOTA;
	flags_arkano_FSM &= ~FLAG_RAQUETA_DERECHA;
	flags_arkano_FSM &= ~FLAG_RAQUETA_IZQUIERDA;
	piUnlock (FLAGS_ARKANO_KEY);
}

void mueveRaquetaIzquierda (fsm_t* this) {
    // void MueveRaquetaIzquierda (void): funcion encargada de ejecutar
    // el movimiento hacia la izquierda contemplado para la raqueta.
    // Debe garantizar la viabilidad del mismo mediante la comprobaciï¿½n
    // de que la nueva posiciï¿½n correspondiente a la raqueta no suponga
    // que ï¿½sta rebase o exceda los lï¿½mites definidos para el ï¿½rea de juego
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
	// pintaPantallaPorTerminal((tipo_pantalla*)(&(juego.arkanoPi.pantalla)));
	piUnlock (STD_IO_BUFFER_KEY);

	piLock (FLAGS_ARKANO_KEY);
	flags_arkano_FSM &= ~FLAG_RAQUETA_IZQUIERDA;
	piUnlock (FLAGS_ARKANO_KEY);

}

void mueveRaquetaDerecha (fsm_t* this) {
    // void MueveRaquetaDerecha (void): funciï¿½n similar a la anterior
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
	// pintaPantallaPorTerminal((tipo_pantalla*)(&(juego.arkanoPi.pantalla)));
	piUnlock (STD_IO_BUFFER_KEY);

	piLock (FLAGS_ARKANO_KEY);
	flags_arkano_FSM &= ~FLAG_RAQUETA_DERECHA;
	piUnlock (FLAGS_ARKANO_KEY);
}

void movimientoPelota(fsm_t* this) {
    // void MovimientoPelota (void): funciï¿½n encargada de actualizar la
    // posiciï¿½n de la pelota conforme a la trayectoria definida para ï¿½sta.
    // Para ello deberï¿½ identificar los posibles rebotes de la pelota para,
    // en ese caso, modificar su correspondiente trayectoria (los rebotes
    // detectados contra alguno de los ladrillos implicarï¿½n adicionalmente
    // la eliminaciï¿½n del ladrillo). Del mismo modo, deberï¿½ tambiï¿½n
    // identificar las situaciones en las que se dï¿½ por finalizada la partida:
    // bien porque el jugador no consiga devolver la pelota, y por tanto ï¿½sta
    // rebase el lï¿½mite inferior del ï¿½rea de juego, bien porque se agoten
    // los ladrillos visibles en el ï¿½rea de juego.

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
	// pintaPantallaPorTerminal((tipo_pantalla*)(&(juego.arkanoPi.pantalla)));
	piUnlock (STD_IO_BUFFER_KEY);

	piLock (FLAGS_ARKANO_KEY);
	flags_arkano_FSM &= ~FLAG_PELOTA;
	piUnlock (FLAGS_ARKANO_KEY);
}

void reseteaJuego (fsm_t* this) {
    //void ReseteaJuego (void): funciï¿½n encargada de llevar a cabo la
    // reinicializaciï¿½n de cuantas variables o estructuras resulten
    // necesarias para dar comienzo a una nueva partida.

	piLock (STD_IO_BUFFER_KEY);
	printf("%s\n", "[LOG] ReseteaJuego");
	inicializaArkanoPi((tipo_arkanoPi*)(&(juego.arkanoPi)));
	pintaMensajeInicial((tipo_pantalla*)(&(juego.arkanoPi.pantalla)));
	printf("%s\n", "PANTALLA INICIAL");
	// pintaPantallaPorTerminal((tipo_pantalla*)(&(juego.arkanoPi.pantalla)));
	piUnlock (STD_IO_BUFFER_KEY);

	piLock (FLAGS_ARKANO_KEY);
	flags_arkano_FSM &= ~FLAG_PELOTA;
	flags_arkano_FSM &= ~FLAG_RAQUETA_DERECHA;
	flags_arkano_FSM &= ~FLAG_RAQUETA_IZQUIERDA;
	piUnlock (FLAGS_ARKANO_KEY);
}

void finalJuego(fsm_t* this){
    // void FinalJuego (void): funciï¿½n encargada de mostrar en la ventana de
    // terminal los mensajes necesarios para informar acerca del resultado del juego.

	printf("%s\n", "[LOG] FinalJuego");
	piLock (FLAGS_ARKANO_KEY);
	flags_arkano_FSM &= ~FLAG_FINAL_JUEGO;
	piUnlock (FLAGS_ARKANO_KEY);

	if(calculaLadrillosRestantes((tipo_pantalla*)&(juego.arkanoPi.ladrillos)) == 0){
		pintaMensajeVictoria((tipo_pantalla*)(&(juego.arkanoPi.pantalla)));
		printf("%s\n", "HAS GANADO");
	}
	else{
		pintaMensajeDerrota((tipo_pantalla*)(&(juego.arkanoPi.pantalla)));
		printf("%s\n", "HAS PERDIDO");
	}
	piLock (FLAGS_ARKANO_KEY);
	flags_arkano_FSM &= ~FLAG_FINAL_JUEGO;
	piUnlock (FLAGS_ARKANO_KEY);
}

void movimientoJoystick (fsm_t* this){
    // void movimientoJoystick (fsm_t* this): funcion encargada de mostrar la posicion de la raqueta
	// correspondiente al joystick analogico haciendo lectura del ADC
	piLock (STD_IO_BUFFER_KEY);
	printf("%s\n", "[LOG] movimientoJoystick");
	piUnlock (STD_IO_BUFFER_KEY);

	float voltaje_medido = lecturaADC();
	if(voltaje_medido < 333.333) {
		mueveRaquetaAPosicion(-1);
	}
	else if(voltaje_medido < 666.666) {
		mueveRaquetaAPosicion(0);
	}
	else if(voltaje_medido < 999.999) {
		mueveRaquetaAPosicion(1);
	}
	else if(voltaje_medido < 1333.333) {
		mueveRaquetaAPosicion(2);
	}
	else if(voltaje_medido < 1666.666) {
		mueveRaquetaAPosicion(3);
	}
	else if(voltaje_medido < 1999.999) {
		mueveRaquetaAPosicion(4);
	}
	else if(voltaje_medido < 2333.333) {
		mueveRaquetaAPosicion(5);
	}
	else if(voltaje_medido < 2666.666) {
		mueveRaquetaAPosicion(6);
	}
	else if(voltaje_medido < 2999.999) {
		mueveRaquetaAPosicion(7);
	}
	else if(voltaje_medido < 3333.333) {
		mueveRaquetaAPosicion(8);
	}
	else if(voltaje_medido < 3666.666) {
		mueveRaquetaAPosicion(9);
	}
	else if(voltaje_medido < 4000.000) {
		mueveRaquetaAPosicion(10);
	}
	else {
		piLock (STD_IO_BUFFER_KEY);
		printf("%s\n", "[LOG] movimientoJoystick: lecturaADC fuera de escala");
		piUnlock (STD_IO_BUFFER_KEY);
	}
	piLock (FLAGS_ARKANO_KEY);
	flags_arkano_FSM &= ~FLAG_JOYSTICK;
	piUnlock (FLAGS_ARKANO_KEY);
}

//------------------------------------------------------------------
// ARKANOPI FSM: FUNCIONES SUPPORT
//------------------------------------------------------------------
void mueveRaquetaAPosicion (int posicion) {
	piLock (STD_IO_BUFFER_KEY);
	printf("%s\n", "[LOG] mueveRaquetaAPosicion");
	piUnlock (STD_IO_BUFFER_KEY);

	if(posicion < -1 || posicion > 10){
		piLock (STD_IO_BUFFER_KEY);
		printf("%s\n", "[LOG] mueveRaquetaAPosicion: posicion fuera de escala");
		piUnlock (STD_IO_BUFFER_KEY);
	}
	// Esto asegura que la raqueta se mantenga en la fila mas baja de la matriz
	juego.arkanoPi.raqueta.y=MATRIZ_ALTO-1;
	juego.arkanoPi.raqueta.x = posicion;

	pintaRaqueta((tipo_raqueta*)(&(juego.arkanoPi.raqueta)), (tipo_pantalla*)(&(juego.arkanoPi.pantalla)));
	actualizaPantalla((tipo_arkanoPi*)(&(juego.arkanoPi)));
	pintaPantallaPorTerminal((tipo_pantalla*)(&(juego.arkanoPi.pantalla)));


}int obtenerTipoDeRebote(void){
    // int ObtenerTipoDeRebote(void): verifica si la proxima casilla causarï¿½
    // un rebote y de quï¿½ tipo

	printf("%s\n", "[LOG] ObtenerTipoDeRebote");
	// Calculo de las nuevas coordenadas que tendrï¿½a la pelota en el prï¿½ximo movimiento
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
    // segï¿½n lo definido en la trayectoria. La trayectoria no se cambia.

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
	// espera hasta la prï¿½xima activaciï¿½n del reloj
	unsigned int now = millis();
	if (next > now) {
		delay (next - now);
    }
}

//------------------------------------------------------------------
// JOYSTICK: LECTURA ADC
//------------------------------------------------------------------
float lecturaADC (void) {
	/*unsigned char ByteSPI[3]; //Buffer lectura escritura SPI
	int resultado_SPI = 0; //Control operacion SPI
	float voltaje_medido = 0.0; //Valor medido. A calcular a partir del buffer
	ByteSPI[0] = 0b10011111; // Configuracion ADC (10011111 unipolar, 0-2.5v, canal 0, salida 1)
    ByteSPI[1] = 0b0;
    ByteSPI[2] = 0b0;
	resultado_SPI = wiringPiSPIDataRW (SPI_ADC_CH, ByteSPI, 3); //Enviamos y leemos tres bytes (8+12+4 bits)
	usleep(20);
	int salida_SPI = ((ByteSPI[1] << 5) | (ByteSPI[2] >> 3)) & 0xFFF;
	voltaje_medido = 2*2.50 * (((float) salida_SPI)/4095.0);
	printf("Lectura ADC MAX1246: %d\n", resultado_SPI); //
	printf("Primer byte: %02X \n", ByteSPI[0]);
	printf("Segundo Byte: %02X \n", ByteSPI[1]);
	printf("Tercer byte: %02X \n", ByteSPI[2]);
	printf("Valor entero: %i \n", salida_SPI);
	printf("Voltaje medido: %f \n",voltaje_medido);
	fflush(stdout);
	return voltaje_medido;*/
	return rand()%4000;
}

//------------------------------------------------------
// ARKANOPI: FUNCIONES DE INICIALIZACION
//------------------------------------------------------
int systemSetup (void) {
	// int systemSetup (void): procedimiento de configuracion del sistema.
	// Realizarï¿½, entra otras, todas las operaciones necesarias para:
	// configurar el uso de posibles librerï¿½as (e.g. Wiring Pi),
	// configurar las interrupciones externas asociadas a los pines GPIO,
	// configurar las interrupciones periï¿½dicas y sus correspondientes temporizadores,
	// crear, si fuese necesario, los threads adicionales que pueda requerir el sistema
	piLock (STD_IO_BUFFER_KEY);
	printf("%s\n", "[LOG] systemSetup");
	// sets up the wiringPi library
	if (wiringPiSetupGpio () < 0) {
		printf ("Unable to setup wiringPi\n");
		piUnlock (STD_IO_BUFFER_KEY);
		return -1;
	}
	// Configuracion del ADC en el CH0
	// TODO Aqui peta
	if (wiringPiSPISetup (SPI_ADC_CH, SPI_ADC_FREQ) < 0) {
		piLock (STD_IO_BUFFER_KEY);
		printf ("No se pudo configurar correctamente el dispositivo en CH0\n");
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
	wiringPiISR(GPIO_RAQ_IZQ, INT_EDGE_FALLING, pulsaRaqIzq);
	wiringPiISR(GPIO_RAQ_DER, INT_EDGE_FALLING, pulsaRaqDer);

	juego.temporizadores.refresco_tmr = tmr_new(refrescarLeds);
	juego.temporizadores.pelota_tmr = tmr_new(pelotaTmrFinished);
	juego.temporizadores.joystick_tmr = tmr_new(joystickTmrFinished);

	tmr_startms((tmr_t*)juego.temporizadores.refresco_tmr, REFRESCO_TIMEOUT);
	tmr_startms((tmr_t*)juego.temporizadores.pelota_tmr, PELOTA_TIMEOUT);
	tmr_startms((tmr_t*)juego.temporizadores.joystick_tmr, JOYSTICK_TIMEOUT);

	piUnlock (STD_IO_BUFFER_KEY);
	return 1;
}

//------------------------------------------------------------------
// ARKANOPI: FUNCION PRINCIPAL MAIN
//------------------------------------------------------------------
int main ()
{
	//printf("%s\n", "[LOG] main");
	unsigned int next;
	// Maquina de estados: lista de transiciones
	// {EstadoOrigen,Funciï¿½nDeEntrada,EstadoDestino,Funciï¿½nDeSalida}
	fsm_trans_t arkano_tabla[] = {
		{WAIT_START, compruebaTeclaPulsada, WAIT_PUSH, inicializaJuego},
		{WAIT_PUSH, compruebaTimeoutPelota, WAIT_PUSH, movimientoPelota},
		{WAIT_PUSH, compruebaTimeoutJoystick, WAIT_PUSH, movimientoJoystick},
		{WAIT_PUSH, compruebaTeclaRaquetaIzquierda, WAIT_PUSH, mueveRaquetaIzquierda},
		{WAIT_PUSH, compruebaTeclaRaquetaDerecha, WAIT_PUSH, mueveRaquetaDerecha},
		{WAIT_PUSH, compruebaFinalJuego, WAIT_END, finalJuego},
		{WAIT_END, compruebaTeclaPulsada, WAIT_PUSH, reseteaJuego},
		{ -1, NULL, -1, NULL },
	};
	// Configuracion e inicializacion del sistema
	fsm_t* arkano_fsm = fsm_new (WAIT_START, arkano_tabla, NULL);
	arkanoPiFSMSetup (arkano_fsm);
	systemSetup();

	next = millis();
	while (1) {
		fsm_fire (arkano_fsm);
		next += CLK_MS;
		delay_until (next);
	}
	fsm_destroy (arkano_fsm);
	tmr_destroy((tmr_t*)juego.temporizadores.refresco_tmr);
	tmr_destroy((tmr_t*)juego.temporizadores.pelota_tmr);
	tmr_destroy((tmr_t*)juego.temporizadores.joystick_tmr);
}
