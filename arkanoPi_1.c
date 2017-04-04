//------------------------------------------------------------------
// IMPORTS
//------------------------------------------------------------------
#include "arkanoPi_1.h"
#include <wiringPi.h>
#include "fsm.h"

//------------------------------------------------------------------
// VARIABLES ESTATICAS
//------------------------------------------------------------------
static tipo_juego juego;
static int gpio_col[4]={GPIO_COL_1,GPIO_COL_2,GPIO_COL_3,GPIO_COL_4};
static int gpio_row[7]={GPIO_ROW_1,GPIO_ROW_2,GPIO_ROW_3,GPIO_ROW_4,
		GPIO_ROW_5,GPIO_ROW_6,GPIO_ROW_7};
// array de filas
static int col[4] = {0,0,0,0};
static int timein=0;
static int col_counter=0;
static int columna=0;

//------------------------------------------------------------------
// VARIABLES VOLATILES
//------------------------------------------------------------------
static int flags = 0;

//------------------------------------------------------------------
// FUNCIONES REFRESCO FSM
//------------------------------------------------------------------
static int timer_finished (fsm_t* this){
	return (millis()-timein >=REFRESH_TIME);
}

// rutina que se ejecuta cada vez que ha pasado cierto tiempo
static void col_x(fsm_t* this) {
	col_counter++;
	columna = col_counter%10;
	ActivaFilasLed(&(juego.arkanoPi.pantalla),&(columna));
	/* digitalWrite(gpio_row[0], LOW);
	digitalWrite(gpio_row[1], LOW);
	digitalWrite(gpio_row[2], LOW);
	digitalWrite(gpio_row[3], LOW);
	digitalWrite(gpio_row[4], LOW);
	digitalWrite(gpio_row[5], LOW);
	digitalWrite(gpio_row[6], LOW); */

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

	//printf("[LOG] Columna numero: %d\n", columna);
	//col[indice-1]=0;
	//col[indice]=1;
	timein=millis();
}

void ActivaFilasLed (tipo_pantalla* p_pantalla, int* columna) {
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

// espera hasta la próxima activación del reloj
void delay_until (unsigned int next) {
	unsigned int now = millis();
	if (next > now) {
		delay (next - now);
    }
}

//------------------------------------------------------------------
// ARKANOPI FSM: FUNCIONES DE ENTRADA
//------------------------------------------------------------------

// FUNCIONES DE ENTRADA O COMPROBACIÓN DE LA MAQUINA DE ESTADOS 
int comprueba_tecla_pulsada(fsm_t* this){
	int result;

	piLock (FLAGS_KEY);
	// result vale 1 si en la variable flags se ha marcado FLAG_PELOTA, FLAG_RAQUETA_DERECHA o FLAG_RAQUETA_IZQUIERDA
	result = (flags & FLAG_PELOTA) || (flags & FLAG_RAQUETA_DERECHA) || (flags & FLAG_RAQUETA_IZQUIERDA);
	piUnlock (FLAGS_KEY);
	return result;
}

int comprueba_tecla_pelota (fsm_t* this) {
	int result;

	piLock (FLAGS_KEY);
	// result vale 1 si en la variable flags se ha marcado FLAG_PELOTA
	result = (flags & FLAG_PELOTA);
	piUnlock (FLAGS_KEY);

	return result;
}

int comprueba_tecla_raqueta_derecha (fsm_t* this) {
	int result;
	// result vale 1 si en la variable flags se ha marcado FLAG_RAQUETA_DERECHA
	piLock (FLAGS_KEY);
	result = (flags & FLAG_RAQUETA_DERECHA);
	piUnlock (FLAGS_KEY);

	return result;
}

int comprueba_tecla_raqueta_izquierda (fsm_t* this) {
	int result;
	// result vale 1 si en la variable flags se ha marcado FLAG_RAQUETA_IZQUIERDA
	piLock (FLAGS_KEY);
	result = (flags & FLAG_RAQUETA_IZQUIERDA);
	piUnlock (FLAGS_KEY);

	return result;
}

// int comprueba_joystick(fsm_t* this) {
// 	return 999;
// }

int comprueba_final_juego (fsm_t* this) {
	int result;

	piLock (FLAGS_KEY);
	// result vale 1 si en la variable flags se ha marcado FLAG_FINAL_JUEGO
	result = (flags & FLAG_FINAL_JUEGO);
	piUnlock (FLAGS_KEY);

	return result;
}

//------------------------------------------------------------------
// ARKANOPI FSM: FUNCIONES SUPPORT
//------------------------------------------------------------------

// int ObtenerTipoDeRebote(void): verifica si la proxima casilla causará
// un rebote y de qué tipo
int ObtenerTipoDeRebote(void){
	printf("%s\n", "[LOG] ObtenerTipoDeRebote");
	// Calculo de las nuevas coordenadas que tendría la pelota en el próximo movimiento
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

//------------------------------------------------------------------
// ARKANOPI FSM: FUNCIONES DE ACCION
//------------------------------------------------------------------

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
	flags &= ~FLAG_PELOTA;
	flags &= ~FLAG_RAQUETA_DERECHA;
	flags &= ~FLAG_RAQUETA_IZQUIERDA;
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
	PintaPantallaPorTerminal((tipo_pantalla*)(&(juego.arkanoPi.pantalla)));

	piUnlock (STD_IO_BUFFER_KEY);

	piLock (FLAGS_KEY);
	flags &= ~FLAG_RAQUETA_IZQUIERDA;
	piUnlock (FLAGS_KEY);

}

// void MueveRaquetaDerecha (void): función similar a la anterior
// encargada del movimiento hacia la derecha.
void MueveRaquetaDerecha (void) {
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

	piLock (FLAGS_KEY);
	flags &= ~FLAG_RAQUETA_DERECHA;
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
	    	piLock (FLAGS_KEY);
	    	flags |= FLAG_FINAL_JUEGO;
	    	piUnlock (FLAGS_KEY);
	        break;
	}
	ActualizaPantalla((tipo_arkanoPi*)(&(juego.arkanoPi)));
	PintaPantallaPorTerminal((tipo_pantalla*)(&(juego.arkanoPi.pantalla)));

	piUnlock (STD_IO_BUFFER_KEY);

	piLock (FLAGS_KEY);
	flags &= ~FLAG_PELOTA;
	piUnlock (FLAGS_KEY);
}

//void ReseteaJuego (void): función encargada de llevar a cabo la
// reinicialización de cuantas variables o estructuras resulten
// necesarias para dar comienzo a una nueva partida.
void ReseteaJuego (fsm_t* this) {
	piLock (STD_IO_BUFFER_KEY);
	printf("%s\n", "[LOG] ReseteaJuego");
	InicializaArkanoPi((tipo_arkanoPi*)(&(juego.arkanoPi)));
	PintaMensajeInicial((tipo_pantalla*)(&(juego.arkanoPi.pantalla)));
	printf("%s\n", "PANTALLA INICIAL");
	PintaPantallaPorTerminal((tipo_pantalla*)(&(juego.arkanoPi.pantalla)));
	piUnlock (STD_IO_BUFFER_KEY);

	piLock (FLAGS_KEY);
	flags &= ~FLAG_PELOTA;
	flags &= ~FLAG_RAQUETA_DERECHA;
	flags &= ~FLAG_RAQUETA_IZQUIERDA;
	piUnlock (FLAGS_KEY);
}

// void FinalJuego (void): función encargada de mostrar en la ventana de
// terminal los mensajes necesarios para informar acerca del resultado del juego.
void FinalJuego(void){
	printf("%s\n", "[LOG] FinalJuego");
	if(CalculaLadrillosRestantes((tipo_pantalla*)&(juego.arkanoPi.ladrillos)) == 0){
		printf("%s\n", "HAS GANADO");
	}
	else{
		printf("%s\n", "HAS PERDIDO");
	}
}


// void ControlJoystick (void){
// 	return;
// }



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
	int x = 0;
	
	piLock (STD_IO_BUFFER_KEY);
	printf("%s\n", "[LOG] systemSetup");

	// sets up the wiringPi library
	if (wiringPiSetupGpio () < 0) {
		printf ("Unable to setup wiringPi\n");
		piUnlock (STD_IO_BUFFER_KEY);
		return -1;    }

	int i=0;
	for (i=0; i<4; i++){
		pinMode(gpio_col[i], OUTPUT);
	}
	for (i=0; i<7; i++){
		pinMode(gpio_row[i], OUTPUT);
		pullUpDnControl (gpio_row[i], PUD_DOWN ); // todas las filas inicialmente a pull down
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
			printf("\nTecla %c\n", teclaPulsada);

			switch(teclaPulsada) {
				case 'i':  // Mover raqueta a la izquierda
					piLock (FLAGS_KEY);
					flags |= FLAG_RAQUETA_IZQUIERDA;
					piUnlock (FLAGS_KEY);
					break;

				case 'o':  // Mover Raqueta a la derecha
					piLock (FLAGS_KEY);
					flags |= FLAG_RAQUETA_DERECHA;
					piUnlock (FLAGS_KEY);
					break;

				case 'p':  // Mover Raqueta a la derecha
					piLock (FLAGS_KEY);
					flags |= FLAG_PELOTA;
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

//------------------------------------------------------------------
// FUNCION PRINCIPAL MAIN
//------------------------------------------------------------------

int main ()
{
	printf("%s\n", "[LOG] main");
	unsigned int next;

	// Maquina de estados: lista de transiciones
	// {EstadoOrigen,FunciónDeEntrada,EstadoDestino,FunciónDeSalida}
	fsm_trans_t arkano_tabla[] = {
		{WAIT_START, comprueba_tecla_pulsada, WAIT_PUSH, InicializaJuego},
		{WAIT_PUSH, comprueba_tecla_pelota, WAIT_PUSH, MovimientoPelota},
		{WAIT_PUSH, comprueba_tecla_raqueta_izquierda, WAIT_PUSH, MueveRaquetaIzquierda},
		{WAIT_PUSH, comprueba_tecla_raqueta_derecha, WAIT_PUSH, MueveRaquetaDerecha},
		//{WAIT_PUSH, comprueba_joystick, WAIT_PUSH, ControlJoystick},
		{WAIT_PUSH, comprueba_final_juego, WAIT_END, FinalJuego},
		{WAIT_END, comprueba_tecla_pulsada, WAIT_PUSH, ReseteaJuego},
		{ -1, NULL, -1, NULL }, 
	};

	fsm_trans_t refresco[] = {
		{REFRESCO_COL, timer_finished, REFRESCO_COL, col_x },
		{ -1, NULL, -1, NULL }, };

	fsm_t* arkano_fsm = fsm_new (WAIT_START, arkano_tabla, NULL);
	fsm_t* refresco_fsm = fsm_new(REFRESCO_COL, refresco, NULL);
	// Configuracion e inicializacion del sistema
	fsm_setup (arkano_fsm);
	system_setup();
	next = millis();
	while (1) {
		fsm_fire (arkano_fsm);
		fsm_fire (refresco_fsm);
		next += CLK_MS;
		delay_until (next);
	}
	fsm_destroy (arkano_fsm);
	fsm_destroy (refresco_fsm);
}
