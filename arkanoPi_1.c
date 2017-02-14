
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
	ReseteaRaqueta(&(juego.arkanoPi.raqueta));
	ReseteaMatriz(&(juego.arkanoPi.pantalla));
	ReseteaLadrillos(&(juego.arkanoPi.ladrillos));
	ReseteaPelota(&(juego.arkanoPi.pelota));
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
		//ActualizaPantalla((tipo_arkanoPi*)(&(juego->arkanoPi)));
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
		//ActualizaPantalla((tipo_arkanoPi*)(&(juego->arkanoPi)))
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
	if(nuevo_x == MATRIZ_ANCHO || nuevo_x == -1 || nuevo_y == MATRIZ_ALTO){
		// Caso de limite de pantalla
		printf("%s\n", "[LOG] ProximoMovimiento: REBOTE LIMITE");
		return REBOTE_LIMITE;
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
void ReboteSuperior(void){
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
	// TODO
   	DesplazarPelota();
	return;
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
	// Checkear trayectoria
	// Acutualizar posicion de la pelota
	// Actualizar trayectoria
	printf("%s\n", "[LOG] MovimientoPelota");
	flag_rebote = ObtenerTipoDeRebote();
	switch(flag_rebote) {
	    case NO_REBOTE:
	    	DesplazarPelota();
			break;		
	    case REBOTE_LADRILLO:
	    	RebotarEnLadrillo():
	        break;
	    case REBOTE_RAQUETA:
	    	RebotarEnRaqueta();	    	
	    	break;
	    case REBOTE_PERDIDA:
	    	FinalJuego();
	        break;
	    case REBOTE_DERECHO:
	    	ReboteDerecho();
	        break;	  
	    case REBOTE_IZQUIERDO:
	    	ReboteIzquierdo();
	        break;	
	    case REBOTE_SUPERIOR:
	    	ReboteSuperior();
	        break;	    
	}
	
}

// void FinalJuego (void): función encargada de mostrar en la ventana de
// terminal los mensajes necesarios para informar acerca del resultado del juego.
void FinalJuego (void) {
	// A completar por el alumno...

}

//void ReseteaJuego (void): función encargada de llevar a cabo la
// reinicialización de cuantas variables o estructuras resulten
// necesarias para dar comienzo a una nueva partida.
void ReseteaJuego (void) {
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
	// A completar por el alumno...

}

int main ()
{
	// Configuracion e inicializacion del sistema
	// A completar por el alumno...

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
					case 'i':
						// A completar por el alumno...

						break;

					case 'o':
						// A completar por el alumno...

						break;

					case 'p':
						// A completar por el alumno...

						break;

					case 'q':
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
