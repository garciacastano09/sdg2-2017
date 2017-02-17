
#include "arkanoPi_1.h"

static volatile tipo_juego juego;

//------------------------------------------------------
// FUNCIONES DE ACCION
//------------------------------------------------------

// void InicializaJuego (void): funcion encargada de llevar a cabo
// la oportuna inicialización de toda variable o estructura de datos
// que resulte necesaria para el desarrollo del juego.
void InicializaJuego (void) {
	printf("%s\n", "[LOG] InicializaJuego");
	// TODO Falta algo aqui?
	InicializaArkanoPi();
	juego.estado = WAIT_START;
	juego.teclaPulsada = NULL;
}

// void MueveRaquetaIzquierda (void): funcion encargada de ejecutar
// el movimiento hacia la izquierda contemplado para la raqueta.
// Debe garantizar la viabilidad del mismo mediante la comprobación
// de que la nueva posición correspondiente a la raqueta no suponga
// que ésta rebase o exceda los límites definidos para el área de juego
// (i.e. al menos uno de los leds que componen la raqueta debe permanecer
// visible durante todo el transcurso de la partida).
void MueveRaquetaIzquierda (void) {
	printf("%s\n", "[LOG] MueveRaquetaIzquierda");
	juego.arkanoPi.raqueta-> y=6;
	if(juego.arkanoPi.raqueta->x < MIN_X_RAQUETA+1){
		printf("%s\n", "[LOG] MueveRaquetaIzquierda: Movimiento a la izquierda imposible");
		return;
	}
	else{
		juego.arkanoPi.raqueta->x--;
		PintaRaqueta((tipo_raqueta*)(&(juego.arkanoPi->raqueta)), (tipo_pantalla*)(&(juego.arkanoPi->pantalla)));
	}
}

// void MueveRaquetaDerecha (void): función similar a la anterior
// encargada del movimiento hacia la derecha.
void MueveRaquetaDerecha (void) {
	printf("%s\n", "[LOG] MueveRaquetaDerecha");
	juego.arkanoPi.raqueta-> y=6
	if(juego.arkanoPi.raqueta->x > MAX_X_RAQUETA-1){
		printf("%s\n", "[LOG] MueveRaquetaIzquierda: Movimiento a la izquierda imposible");
		return;
	}
	else{
		juego.arkanoPi.raqueta->x++;
		PintaRaqueta((tipo_raqueta*)(&(juego.arkanoPi->raqueta)), (tipo_pantalla*)(&(juego.arkanoPi->pantalla)));
	}

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
	printf("%s\n", "[LOG] MovimientoPelota");
	// Elegimos lo que se hace segun el tipo de rebote
	flag_rebote = ObtenerTipoDeRebote();
	switch(flag_rebote) {
		// Caso en que no existe rebote porque la siguiente casilla esta vacia
	    case NO_REBOTE:
	    	DesplazarPelota();
			break;	
	    // Caso en que se choca contra la raqueta
	    case REBOTE_RAQUETA:
	    	RebotarEnRaqueta();	    	
	    	break;	
		// Caso en que se choca contra un lateral: izquierda o derecha
	    case REBOTE_LATERAL:
	    	ReboteLateral();
	        break;	 
		// Caso en que se choca contra un ladrillo
	    case REBOTE_LADRILLO:
	    	RebotarEnLadrillo():
	    	ComprobarExito();
	        break;
	    // Caso en que se choca contra el techo (cuando ya no hay ladrillos debajo)
	    case REBOTE_TECHO:
	    	ReboteSuperior();
	        break;	  
	    // Caso en que se choca contra un el limite inferior (pierdes la partida)
	    case REBOTE_PERDIDA:
	    	// TODO diferencia entre ganar y perder la partida
	    	FinalJuego();
	        break;  
	}	
}

// void FinalJuego (void): función encargada de mostrar en la ventana de
// terminal los mensajes necesarios para informar acerca del resultado del juego.
void FinalJuego (void) {
	printf("%s\n", "[LOG] FinalJuego");
	// TODO poner mensaje de fin de juego

	// Fijar el estado del jeugo a END
	juego.estado = WAIT_END;
}

//void ReseteaJuego (void): función encargada de llevar a cabo la
// reinicialización de cuantas variables o estructuras resulten
// necesarias para dar comienzo a una nueva partida.
void ReseteaJuego (void) {
	printf("%s\n", "[LOG] ReseteaJuego");
	// TODO ¿Diferencia con InicializaJuego?
	InicializaJuego();
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
int systemSetup (void) {
	printf("%s\n", "[LOG] systemSetup");
	// TODO ???
}

int main ()
{
	printf("%s\n", "[LOG] main");

	// Configuracion e inicializacion del sistema
	InicializaJuego();
	systemSetup();
	while (1) {
		if(kbhit()) { // Funcion que detecta si se ha producido pulsacion de tecla alguna
			juego.teclaPulsada = kbread(); // Funcion que devuelve la tecla pulsada

			// Interpretacion de las pulsaciones para cada posible estado del sistema
			if( juego.estado == WAIT_START ) { // Cualquier pulsacion da comienzo al juego...
				// Descomente ambas lineas y sustituya cada etiqueta XXXXXXX por lo que corresponda en cada caso...
				// XXXXXXX();
				// juego.estado = XXXXXXX;
			}
			else if( juego.estado == WAIT_END ) { // Cualquier nos devuelve al estado inicial...
				// Descomente ambas lineas y sustituya cada etiqueta XXXXXXX por lo que corresponda en cada caso...
				// XXXXXXX();
				// juego.estado = XXXXXXX;
			}
			else { // Si estamos jugando...
				switch(juego.teclaPulsada) {
					case 'i': // Mover raqueta a la izquierda
						// A completar por el alumno...
						MueveRaquetaIzquierda();
						ActualizaPantalla((tipo_arkanoPi*)(&(juego->arkanoPi)));
						break;

					case 'o': // Mover Raqueta a la derecha
						MueveRaquetaDerecha();
						ActualizaPantalla((tipo_arkanoPi*)(&(juego->arkanoPi)));
						break;

					case 'p': // Mover pelota
						MovimientoPelota();
						ActualizaPantalla((tipo_arkanoPi*)(&(juego->arkanoPi)));
						break;

					case 'q': // Salir
						exit(0);
						break;

					default:
						printf("INVALID KEY!!!\n");
						break;
				}
			}
		}
	}
}

//---------------------
// FUNCIONES SUPPORT
//---------------------

void ComprobarExito(void){
	printf("%s\n", "[LOG] ComprobarExito");
	if(CalculaLadrillosRestantes() == 0){
		// Ganas
		// FinalJuego();
	}
}

// int ObtenerTipoDeRebote(void): verifica si la proxima casilla causará 
// un rebote y de qué tipo
int ObtenerTipoDeRebote(void){
	printf("%s\n", "[LOG] ProximoMovimiento");
	int nuevo_y = y + yv;
	int nuevo_x = x + xv;
	int raqueta_x = juego.arkanoPi.raqueta.x;
	if(nuevo_y == -1){
		// Caso de perdida de pelota
		printf("%s\n", "[LOG] ProximoMovimiento: REBOTE PERDIDA");
		return REBOTE_PERDIDA;
	}
	if(nuevo_x == MATRIZ_ANCHO || nuevo_x == -1){
		// Caso de lateral de la pantalla
		printf("%s\n", "[LOG] ProximoMovimiento: REBOTE LATERAL");
		return REBOTE_LIMITE;
	}
	if(nuevo_y == MATRIZ_ALTO){
		// Caso de techo de la pantalla
		printf("%s\n", "[LOG] ProximoMovimiento: REBOTE TECHO");
		return REBOTE_TECHO;
	}
	if(nuevo_x == raqueta_x || nuevo_x == raqueta_x + 1 || nuevo_x == raqueta_x - 1){
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
	juego.arkanoPi.pelota.x = juego.arkanoPi.pelota.x + juego.arkanoPi.pelota.xv
	juego.arkanoPi.pelota.y = juego.arkanoPi.pelota.y + juego.arkanoPi.pelota.yv
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
	nueva_posicion_x = juego.arkanoPi.pelota.x;
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

