
#include "arkanoPi_1.h"

static volatile tipo_juego juego;

volatile int flags = 0;

// espera hasta la próxima activación del reloj
void delay_until (unsigned int next) {
	unsigned int now = millis();

	if (next > now) {
		delay (next - now);
    }
}

//------------------------------------------------------
// FUNCIONES DE LA MAQUINA DE ESTADOS
//------------------------------------------------------

// FUNCIONES DE ENTRADA O COMPROBACIÓN DE LA MAQUINA DE ESTADOS 
int comprueba_tecla_pelota (fsm_t* this) {
	int result;

	piLock (FLAGS_KEY);
	result = (flags & FLAG_TECLA_P);
	piUnlock (FLAGS_KEY);

	return result;
}

int comprueba_tecla_raqueta_derecha (fsm_t* this) {
	int result;

	piLock (FLAGS_KEY);
	result = (flags & FLAG_TECLA_O);
	piUnlock (FLAGS_KEY);

	return result;
}

int comprueba_tecla_raqueta_izquierda (fsm_t* this) {
	int result;

	piLock (FLAGS_KEY);
	result = (flags & FLAG_TECLA_I);
	piUnlock (FLAGS_KEY);

	return result;
}

int comprueba_joystick(fsm_t* this) {
	return;
}

int comprueba_final_juego (fsm_t* this) {
	int result;

	piLock (FLAGS_KEY);
	result = (flags & FLAG_FINAL_JUEGO);
	piUnlock (FLAGS_KEY);

	return result;
}


//---------------------
// FUNCIONES SUPPORT
//---------------------

// int ObtenerTipoDeRebote(void): verifica si la proxima casilla causará
// un rebote y de qué tipo
int ObtenerTipoDeRebote(void){
	printf("%s\n", "[LOG] ObtenerTipoDeRebote");
	int nuevo_y = juego.arkanoPi.pelota.y + juego.arkanoPi.pelota.yv;
	int nuevo_x = juego.arkanoPi.pelota.x + juego.arkanoPi.pelota.xv;
	int raqueta_x = juego.arkanoPi.raqueta.x;
	if(nuevo_y == -1){
		// Caso de perdida de pelota
		printf("%s\n", "[LOG] ProximoMovimiento: REBOTE PERDIDA");
		return REBOTE_PERDIDA;
	}
	if(nuevo_x == MATRIZ_ANCHO || nuevo_x == -1){
		// Caso de lateral de la pantalla
		printf("%s\n", "[LOG] ProximoMovimiento: REBOTE LATERAL");
		return REBOTE_LATERAL;
	}
	if(nuevo_y == 0){
		// Caso de techo de la pantalla
		printf("%s\n", "[LOG] ProximoMovimiento: REBOTE TECHO");
		return REBOTE_TECHO;
	}
	if(nuevo_y == MATRIZ_ALTO - 1 && (nuevo_x == raqueta_x || nuevo_x == raqueta_x + 1 || nuevo_x == raqueta_x - 1)){
		// Caso de choque contra raqueta
		printf("%s\n", "[LOG] ProximoMovimiento: REBOTE RAQUETA");
		return REBOTE_RAQUETA;
	}
	if(juego.arkanoPi.ladrillos.matriz[nuevo_x][nuevo_y]){
		// Caso de choque contra ladrillo
		printf("%s\n", "[LOG] ProximoMovimiento: REBOTE LADRILLO");
		return REBOTE_LADRILLO;
	}
	else{
		printf("%s\n", "[LOG] ProximoMovimiento: NO REBOTE");
		return NO_REBOTE;
	}
}

// void DesplazarPelota(void): mueve la posicion de la pelota un pixel
// según lo definido en la trayectoria. La trayectoria no se cambia.
void DesplazarPelota(void){
	printf("%s\n", "[LOG] DesplazarPelota");
	juego.arkanoPi.pelota.x = juego.arkanoPi.pelota.x + juego.arkanoPi.pelota.xv;
	juego.arkanoPi.pelota.y = juego.arkanoPi.pelota.y + juego.arkanoPi.pelota.yv;
}

// void ReboteLadrillo(void): elimina el ladrillo y cambia la trayectoria de
// la pelota hacia abajo
void ReboteLadrillo(void){
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

// void ReboteSuperior(void): cambia la trayectoria de la pelota hacia abajo
void ReboteTecho(void){
	printf("%s\n", "[LOG] ReboteSuperior");
	// Cambiamos trayectoria
	juego.arkanoPi.pelota.yv = - juego.arkanoPi.pelota.yv;
	// Desplazamos pelota
   	DesplazarPelota();
}

// void ReboteLateral(void): cambia la trayectoria de la pelota hacia el
// lado contrario
void ReboteLateral(void){
	printf("%s\n", "[LOG] ReboteLateral");
	// Cambiamos trayectoria
	juego.arkanoPi.pelota.xv = - juego.arkanoPi.pelota.xv;
	// Desplazamos pelota
   	DesplazarPelota();
}

// void ReboteRaqueta(void): cambia la trayectoria de la pelota con la logica
// correspondiente
void ReboteRaqueta(void){
	printf("%s\n", "[LOG] ReboteRaqueta");
	int nueva_posicion_x = juego.arkanoPi.pelota.x;
	// Cambiamos trayectoria
	// Caso en que la pelota choca a la IZQUIERDA de la raqueta
	if(nueva_posicion_x == juego.arkanoPi.raqueta.x - 1){
			juego.arkanoPi.pelota.xv = - 1;
	}
	// Caso en que la pelota choca al CENTRO de la raqueta
	if(nueva_posicion_x == juego.arkanoPi.raqueta.x){
			juego.arkanoPi.pelota.xv = 0;
	}
	// Caso en que la pelota choca a la DERECHA de la raqueta
	if(nueva_posicion_x == juego.arkanoPi.raqueta.x + 1){
			juego.arkanoPi.pelota.xv = 1;
	}
	else{
		printf("%s\n", "[LOG] ReboteRaqueta: ");
		return;
	}
	// La pelota en cualquier caso va hacia arriba
	juego.arkanoPi.pelota.yv = - 1;
   	DesplazarPelota();
}

//------------------------------------------------------
// FUNCIONES DE ACCION
//------------------------------------------------------

// void InicializaJuego (void): funcion encargada de llevar a cabo
// la oportuna inicialización de toda variable o estructura de datos
// que resulte necesaria para el desarrollo del juego.
void InicializaJuego (void) {
	piLock (STD_IO_BUFFER_KEY);
	printf("%s\n", "[LOG] InicializaJuego");
	InicializaArkanoPi((tipo_arkanoPi*)(&(juego.arkanoPi)));
	ActualizaPantalla((tipo_arkanoPi*)(&(juego.arkanoPi)));
	PintaPantallaPorTerminal((tipo_pantalla*)(&(juego.arkanoPi.pantalla)));
	piUnlock (STD_IO_BUFFER_KEY);

	piLock (FLAGS_KEY);
	flags &= ~FLAG_TECLA;
	flags &= ~FLAG_PELOTA;
	flags &= ~FLAG_RAQUETA_DERECHA;
	flags &= ~FLAG_RAQUETA_IZQUIERDA;
	flags &= ~FLAG_JOYSTICK;
	piUnlock (FLAGS_KEY);
}

// void MueveRaquetaIzquierda (void): funcion encargada de ejecutar
// el movimiento hacia la izquierda contemplado para la raqueta.
// Debe garantizar la viabilidad del mismo mediante la comprobación
// de que la nueva posición correspondiente a la raqueta no suponga
// que ésta rebase o exceda los límites definidos para el área de juego
// (i.e. al menos uno de los leds que componen la raqueta debe permanecer
// visible durante todo el transcurso de la partida).
void MueveRaquetaIzquierda (void) {
	piLock (STD_IO_BUFFER_KEY);
	printf("%s\n", "[LOG] MueveRaquetaIzquierda");
	juego.arkanoPi.raqueta.y=MATRIZ_ALTO-1;
	if(juego.arkanoPi.raqueta.x < MIN_X_RAQUETA+1){
		printf("%s\n", "[LOG] MueveRaquetaIzquierda: Movimiento a la izquierda imposible");
		return;
	}
	else{
		juego.arkanoPi.raqueta.x--;
		PintaRaqueta((tipo_raqueta*)(&(juego.arkanoPi.raqueta)), (tipo_pantalla*)(&(juego.arkanoPi.pantalla)));
	}
	ActualizaPantalla((tipo_arkanoPi*)(&(juego.arkanoPi)));
	PintaPantallaPorTerminal((tipo_pantalla*)(&(juego.arkanoPi.pantalla)));

	piUnlock (STD_IO_BUFFER_KEY);

	piLock (FLAGS_KEY);
	flags &= ~FLAG_TECLA;
	flags &= ~FLAG_PELOTA;
	flags &= ~FLAG_RAQUETA_DERECHA;
	flags &= ~FLAG_RAQUETA_IZQUIERDA;
	flags &= ~FLAG_JOYSTICK;
	piUnlock (FLAGS_KEY);

}

// void MueveRaquetaDerecha (void): función similar a la anterior
// encargada del movimiento hacia la derecha.
void MueveRaquetaDerecha (void) {
	piLock (STD_IO_BUFFER_KEY);
	printf("%s\n", "[LOG] MueveRaquetaDerecha");
	juego.arkanoPi.raqueta.y=MATRIZ_ALTO-1;
	if(juego.arkanoPi.raqueta.x > MAX_X_RAQUETA-1){
		printf("%s\n", "[LOG] MueveRaquetaIzquierda: Movimiento a la izquierda imposible");
		return;
	}
	else{
		juego.arkanoPi.raqueta.x++;
		PintaRaqueta((tipo_raqueta*)(&(juego.arkanoPi.raqueta)), (tipo_pantalla*)(&(juego.arkanoPi.pantalla)));
	}
	ActualizaPantalla((tipo_arkanoPi*)(&(juego.arkanoPi)));
	PintaPantallaPorTerminal((tipo_pantalla*)(&(juego.arkanoPi.pantalla)));

	piUnlock (STD_IO_BUFFER_KEY);

	piLock (FLAGS_KEY);
	flags &= ~FLAG_TECLA;
	flags &= ~FLAG_PELOTA;
	flags &= ~FLAG_RAQUETA_DERECHA;
	flags &= ~FLAG_RAQUETA_IZQUIERDA;
	flags &= ~FLAG_JOYSTICK;
	piUnlock (FLAGS_KEY);

 
}

// void MovimientoPelota (void): función encargada de actualizar la
// posición de la pelota conforme a la trayectoria definida para ésta.
// Para ello deberá identificar los posibles rebotes de la pelota para,
// en ese caso, modificar su correspondiente trayectoria (los rebotes
// detectados contra alguno de los ladrillos implicarán adicionalmente
// la eliminación del ladrillo). Del mismo modo, deberá también
// identificar las situaciones en las que se dé por finalizada la partida:
// bien porque el jugador no consiga devolver la pelota, y por tanto ésta
// rebase el límite inferior del área de juego, bien porque se agoten
// los ladrillos visibles en el área de juego.
void MovimientoPelota(void) {
	piLock (STD_IO_BUFFER_KEY);

	printf("%s\n", "[LOG] MovimientoPelota");
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
	    	ComprobarExito();
	        break;
	    // Caso en que se choca contra el techo (cuando ya no hay ladrillos debajo)
	    case REBOTE_TECHO:
	    	ReboteTecho();
	        break;
	    // Caso en que se choca contra un el limite inferior (pierdes la partida)
	    case REBOTE_PERDIDA:
			// Pierdes
			CambiarEstado(WAIT_END);
			Perder();
	        break;
	}
	ActualizaPantalla((tipo_arkanoPi*)(&(juego.arkanoPi)));
	PintaPantallaPorTerminal((tipo_pantalla*)(&(juego.arkanoPi.pantalla)));

	piUnlock (STD_IO_BUFFER_KEY);

	piLock (FLAGS_KEY);
	flags &= ~FLAG_TECLA;
	flags &= ~FLAG_PELOTA;
	flags &= ~FLAG_RAQUETA_DERECHA;
	flags &= ~FLAG_RAQUETA_IZQUIERDA;
	flags &= ~FLAG_JOYSTICK;
	piUnlock (FLAGS_KEY);

}


//void ReseteaJuego (void): función encargada de llevar a cabo la
// reinicialización de cuantas variables o estructuras resulten
// necesarias para dar comienzo a una nueva partida.
void ReseteaJuego (void) {
	piLock (STD_IO_BUFFER_KEY);

	printf("%s\n", "[LOG] ReseteaJuego");
	InicializaJuego();

	piUnlock (STD_IO_BUFFER_KEY);

	piLock (FLAGS_KEY);
	flags &= ~FLAG_TECLA;
	flags &= ~FLAG_PELOTA;
	flags &= ~FLAG_RAQUETA_DERECHA;
	flags &= ~FLAG_RAQUETA_IZQUIERDA;
	flags &= ~FLAG_JOYSTICK;
	piUnlock (FLAGS_KEY);

}

// void FinalJuego (void): función encargada de mostrar en la ventana de
// terminal los mensajes necesarios para informar acerca del resultado del juego.
int FinalJuego(void){
	printf("%s\n", "[LOG] FinalJuego");
	if(CalculaLadrillosRestantes((tipo_pantalla*)&(juego.arkanoPi.ladrillos)) == 0){
		// Ganas
		return 1;
	}
	else{
		return 0;
	}
}


void ControlJoystick (void){
	return;
}



//------------------------------------------------------
// FUNCIONES DE INICIALIZACION
//------------------------------------------------------

// int systemSetup (void): procedimiento de configuracion del sistema.
// Realizará, entra otras, todas las operaciones necesarias para:
// configurar el uso de posibles librerías (e.g. Wiring Pi),
// configurar las interrupciones externas asociadas a los pines GPIO,
// configurar las interrupciones periódicas y sus correspondientes temporizadores,
// crear, si fuese necesario, los threads adicionales que pueda requerir el sistema
int system_setup (void) {
	printf("%s\n", "[LOG] systemSetup");

	int x = 0;
	
	piLock (STD_IO_BUFFER_KEY);

	// sets up the wiringPi library
	if (wiringPiSetupGpio () < 0) {
		printf ("Unable to setup wiringPi\n");
		piUnlock (STD_IO_BUFFER_KEY);
		return -1;
    }

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

void fsm_setup(fsm_t* arkano_fsm) {
	piLock (FLAGS_KEY);
	flags = 0;
	piUnlock (FLAGS_KEY);

	ReseteaJuego(arkano_fsm);

	piLock (STD_IO_BUFFER_KEY);
	printf("\nPULSE P para mover la pelota, I para moverse a la izquierda y O para moverse a la derecha.\n");
	piUnlock (STD_IO_BUFFER_KEY);
}

//------------------------------------------------------
// PI_THREAD (thread_explora_teclado): Thread function for keystrokes detection and interpretation
//------------------------------------------------------
PI_THREAD (thread_explora_teclado) {
	int teclaPulsada;

	while(1) {
		delay(10); // Wiring Pi function: pauses program execution for at least 10 ms

		piLock (STD_IO_BUFFER_KEY);

		if(kbhit()) {
			teclaPulsada = kbread();
			printf("Tecla %c\n", teclaPulsada);

			switch(teclaPulsada) {
				case 'i':  // Mover raqueta a la izquierda
					piLock (FLAGS_KEY);
					flags |= FLAG_TECLA_I;
					piUnlock (FLAGS_KEY);
					break;

				case 'o':  // Mover Raqueta a la derecha
					piLock (FLAGS_KEY);
					flags |= FLAG_TECLA_O;
					piUnlock (FLAGS_KEY);
					break;

				case 'p':  // Mover Raqueta a la derecha
					piLock (FLAGS_KEY);
					flags |= FLAG_TECLA_P;
					piUnlock (FLAGS_KEY);
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


int main ()
{
	printf("%s\n", "[LOG] main");
	unsigned int next;

	// Maquina de estados: lista de transiciones
	// {EstadoOrigen,FunciónDeEntrada,EstadoDestino,FunciónDeSalida}
	fsm_trans_t arkano_tabla[] = {
		{WAIT_START, comprueba_tecla_pulsada, WAIT_PUSH, InicializaJuego},
		{WAIT_PUSH, comprueba_tecla_pelota, WAIT_PUSH, InicializaJuego},
		{WAIT_PUSH, comprueba_tecla_raqueta_izquierda, WAIT_PUSH, MueveRaquetaIzquierda},
		{WAIT_PUSH, comprueba_tecla_raqueta_derecha, WAIT_PUSH, MueveRaquetaDerecha},
		{WAIT_PUSH, comprueba_joystick, WAIT_PUSH, ControlJoystick},
		{WAIT_PUSH, comprueba_final_juego, WAIT_END, FinalJuego},
		{WAIT_END, comprueba_tecla_pulsada, WAIT_PUSH, ReseteaJuego},
		{ -1, NULL, -1, NULL }, 
	};

	fsm_t* arkano_fsm = fsm_new (WAIT_START, arkano_tabla, NULL);
	// Configuracion e inicializacion del sistema
	system_setup();
	fsm_setup (arkano_fsm);
	next = millis();
	while (1) {
		fsm_fire (arkano_fsm);
		next += CLK_MS;
		delay_until (next);
	}

	fsm_destroy (arkano_fsm);
}


