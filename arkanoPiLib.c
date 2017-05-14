#include "arkanoPiLib.h"
#include "arkanoPi.h"

int ladrillos_basico[MATRIZ_ANCHO][MATRIZ_ALTO] = { // Configuracion inicial de los ladrillos del juego
	{1,1,0,0,0,0,0},
	{1,1,0,0,0,0,0},
	{1,1,0,0,0,0,0},
	{1,1,0,0,0,0,0},
	{1,1,0,0,0,0,0},
	{1,1,0,0,0,0,0},
	{1,1,0,0,0,0,0},
	{1,1,0,0,0,0,0},
	{1,1,0,0,0,0,0},
	{1,1,0,0,0,0,0},
};

int mensaje_inicial[MATRIZ_ANCHO][MATRIZ_ALTO] = { // Mensaje inicial que transcribe "HOLA"
	{0,0,0,0,1,1,1},
	{1,1,1,0,0,0,1},
	{0,1,0,0,0,0,1},
	{1,1,1,0,0,0,0},
	{0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0},
	{0,0,0,0,1,1,1},
	{1,1,1,0,1,1,0},
	{1,0,1,0,1,1,1},
	{1,1,1,0,0,0,0}
};

int mensaje_victoria[MATRIZ_ANCHO][MATRIZ_ALTO] = { // Mensaje de victoria que transcribe "WIN"
	{1,1,1,1,1,1,1},
	{0,0,0,0,1,1,1},
	{1,1,1,1,1,1,1},
	{0,0,0,0,0,0,0},
	{1,1,1,1,1,1,1},
	{0,0,0,0,0,0,0},
	{1,1,1,1,1,1,1},
	{0,1,1,0,0,0,0},
	{0,0,0,0,1,1,0},
	{1,1,1,1,1,1,1},
};

int mensaje_derrota[MATRIZ_ANCHO][MATRIZ_ALTO] = { // Mensaje de derrota que transcribe "LOSE"
	{1,1,1,1,1,1,1},
	{0,0,0,0,0,0,1},
	{1,1,1,1,1,1,1},
	{1,1,1,1,1,1,1},
	{0,0,0,0,0,0,0},
	{1,1,1,1,0,0,1},
	{1,0,0,1,1,1,1},
	{0,0,0,0,0,0,0},
	{1,1,1,1,1,1,1},
	{1,0,0,1,0,0,1},
};

//------------------------------------------------------
// FUNCIONES DE INICIALIZACION / RESET
//------------------------------------------------------
void reseteaMatriz(tipo_pantalla* p_pantalla) {
	/*
	Funcion encargada de poner a 0 todas las casillas de una pantalla
	*/
	piLock (STD_IO_BUFFER_KEY);
	printf("%s\n", "[LOG] ReseteaMatriz");
	piUnlock (STD_IO_BUFFER_KEY);

	int i, j = 0;
	for(i=0;i<MATRIZ_ANCHO;i++) {
		for(j=0;j<MATRIZ_ALTO;j++) {
			piLock(PANTALLA_KEY);
			p_pantalla->matriz[i][j] = 0;
			piUnlock(PANTALLA_KEY);
		}
	}
}

void reseteaLadrillos(tipo_pantalla* p_ladrillos) {
	/*
	Funcion encargada de poner los ladrillos como indica la configuracion inicial
	*/
	piLock (STD_IO_BUFFER_KEY);
	printf("%s\n", "[LOG] ReseteaLadrillos");
	piUnlock (STD_IO_BUFFER_KEY);

	int i, j = 0;
	for(i=0;i<MATRIZ_ANCHO;i++) {
		for(j=0;j<MATRIZ_ALTO;j++) {
			p_ladrillos->matriz[i][j] = ladrillos_basico[i][j];
		}
	}
}

void reseteaPelota(tipo_pelota* p_pelota) {
	/*
	Funcion encargada de configurar inicialmente la pelota
	*/
	piLock (STD_IO_BUFFER_KEY);
	printf("%s\n", "[LOG] ReseteaPelota");
	piUnlock (STD_IO_BUFFER_KEY);

	p_pelota->x = MATRIZ_ANCHO/2 - 1; 	// Pelota inicialmente en el centro de la pantalla
	p_pelota->y = MATRIZ_ALTO/2;

	p_pelota->y = MATRIZ_ALTO/2-1; 	// Trayectoria inicial
	p_pelota->xv = -1;
	p_pelota->yv = 1;
	p_pelota->xv = 0;
}

void reseteaRaqueta(tipo_raqueta *p_raqueta) {
	/*
	Funcion encargada de configurar inicialmente la raqueta
	*/
	piLock (STD_IO_BUFFER_KEY);
	printf("%s\n", "[LOG] ReseteaRaqueta");
	piUnlock (STD_IO_BUFFER_KEY);

	p_raqueta->x = MATRIZ_ANCHO/2 - p_raqueta->ancho/2; // Raqueta inicialmente en el centro de la pantalla
	p_raqueta->y = MATRIZ_ALTO - 1;
	p_raqueta->ancho = RAQUETA_ANCHO;
	p_raqueta->alto = RAQUETA_ALTO;
}

void pintaMensajeInicial(tipo_pantalla* p_pantalla){
	/*
	Funcion encargada de pintar el mensaje inicial
	*/
	piLock (STD_IO_BUFFER_KEY);
	printf("%s\n", "[LOG] pintaMensajeInicial");
	piUnlock (STD_IO_BUFFER_KEY);

	int i, j = 0;
	for(i=0;i<MATRIZ_ANCHO;i++) {
		for(j=0;j<MATRIZ_ALTO;j++) {
			piLock(PANTALLA_KEY);
			p_pantalla->matriz[i][j] = mensaje_inicial[i][j];
			piUnlock(PANTALLA_KEY);
		}
	}
}

void pintaMensajeVictoria(tipo_pantalla* p_pantalla){
	/*
	Funcion encargada de pintar el mensaje de victoria
	*/
	piLock (STD_IO_BUFFER_KEY);
	printf("%s\n", "[LOG] pintaMensajeVictoria");
	piUnlock (STD_IO_BUFFER_KEY);

	int i, j = 0;
	for(i=0;i<MATRIZ_ANCHO;i++) {
		for(j=0;j<MATRIZ_ALTO;j++) {
			piLock(PANTALLA_KEY);
			p_pantalla->matriz[i][j] = mensaje_victoria[i][j];
			piUnlock(PANTALLA_KEY);
		}
	}
}

void pintaMensajeDerrota(tipo_pantalla* p_pantalla){
	/*
	Funcion encargada de pintar el mensaje de derrota
	*/
	piLock (STD_IO_BUFFER_KEY);
	printf("%s\n", "[LOG] pintaMensajeDerrota");
	piUnlock (STD_IO_BUFFER_KEY);

	int i, j = 0;
	for(i=0;i<MATRIZ_ANCHO;i++) {
		for(j=0;j<MATRIZ_ALTO;j++) {
			piLock(PANTALLA_KEY);
			p_pantalla->matriz[i][j] = mensaje_derrota[i][j];
			piUnlock(PANTALLA_KEY);
		}
	}
}
//------------------------------------------------------
// FUNCIONES DE VISUALIZACION
//------------------------------------------------------
void pintaPantallaPorTerminal  (tipo_pantalla* p_pantalla) {
	/*
	Metodo encargado de mostrar	el contenido o la ocupacion de la matriz de leds en la ventana de
	terminal.
	*/
	piLock (STD_IO_BUFFER_KEY);
	printf("%s\n", "[LOG] PintaPantallaPorTerminal");
	printf("%s\n", "---PANTALLA---");

	int i, j = 0;
	for(j=0;j<MATRIZ_ALTO;j++) {
		for(i=0;i<MATRIZ_ANCHO;i++) {
			printf("%d ", p_pantalla->matriz[i][j]);
		}
		printf("%s\n", "");
	}
	piUnlock (STD_IO_BUFFER_KEY);
}

void pintaLadrillos(tipo_pantalla* p_ladrillos, tipo_pantalla* p_pantalla) {
	/*
	Funcion encargada de pintar los ladrillos en sus correspondientes posiciones dentro del area de juego
	*/
	piLock (STD_IO_BUFFER_KEY);
	printf("%s\n", "[LOG] PintaLadrillos");
	piUnlock (STD_IO_BUFFER_KEY);

	int i, j = 0;
	for(i=0;i<MATRIZ_ANCHO;i++) {
		for(j=0;j<MATRIZ_ALTO;j++) {
			if(p_ladrillos->matriz[i][j] == 1){
				piLock(PANTALLA_KEY);
				p_pantalla->matriz[i][j] = 1;
				piUnlock(PANTALLA_KEY);
			}
		}
    }
}

void pintaRaqueta(tipo_raqueta* p_raqueta, tipo_pantalla* p_pantalla) {
	/*
	Funcion encargada de pintar la raqueta en su posicion correspondiente dentro del area de juego
	*/
	piLock (STD_IO_BUFFER_KEY);
	printf("%s\n", "[LOG] PintaRaqueta");
	piUnlock (STD_IO_BUFFER_KEY);

	piLock(PANTALLA_KEY);
	if(p_raqueta->x-1 >= 0 && p_raqueta->x-1 <= MATRIZ_ANCHO-1){
		p_pantalla->matriz[p_raqueta->x-1][MATRIZ_ALTO-1] = 1;
	}
	if(p_raqueta->x+1 >= 0 && p_raqueta->x+1 <= MATRIZ_ANCHO-1){
		p_pantalla->matriz[p_raqueta->x+1][MATRIZ_ALTO-1] = 1;
	}
	if(p_raqueta->x >= 0 && p_raqueta->x <= MATRIZ_ANCHO-1){
		p_pantalla->matriz[p_raqueta->x][MATRIZ_ALTO-1] = 1;
	}
	piUnlock(PANTALLA_KEY);
}

void pintaPelota(tipo_pelota* p_pelota, tipo_pantalla* p_pantalla) {
	/*
	Funcion encargada de pintar la pelota en su posicion correspondiente dentro del area de juego
	*/
	piLock (STD_IO_BUFFER_KEY);
	printf("%s\n", "[LOG] PintaPelota");
	piUnlock (STD_IO_BUFFER_KEY);

	if( (p_pelota->x >= 0) && (p_pelota->x < MATRIZ_ANCHO) ) {
		if( (p_pelota->y >= 0) && (p_pelota->y < MATRIZ_ALTO) ) {
			piLock(PANTALLA_KEY);
			p_pantalla->matriz[p_pelota->x][p_pelota->y] = 1;
			piUnlock(PANTALLA_KEY);
		}
		else {
			piLock (STD_IO_BUFFER_KEY);
			printf("\n\nPROBLEMAS!!!! posicion y=%d de la pelota INVALIDA!!!\n\n", p_pelota->y);
			fflush(stdout);
			piUnlock (STD_IO_BUFFER_KEY);
		}
	}
	else {
		piLock (STD_IO_BUFFER_KEY);
		printf("\n\nPROBLEMAS!!!! posicion x=%d de la pelota INVALIDA!!!\n\n", p_pelota->x);
		fflush(stdout);
		piUnlock (STD_IO_BUFFER_KEY);
	}
}

void actualizaPantalla(tipo_arkanoPi* p_arkanoPi) {
	/*
	Metodo cuya ejecucion estara ligada a
	cualquiera de los movimientos de la raqueta o de la pelota y que
	sera el encargado de actualizar convenientemente la estructura de datos
	en memoria que representa el area de juego y su correspondiente estado.
	*/
	piLock (STD_IO_BUFFER_KEY);
	printf("%s\n", "[LOG] ActualizaPantalla");
	piUnlock (STD_IO_BUFFER_KEY);
	reseteaMatriz((tipo_pantalla*)(&(p_arkanoPi->pantalla))); // Borro toda la pantalla
	// Pinta de nuevo los valores de pantalla, ladrillos y raqueta que hay en arkanoPi
	pintaPelota((tipo_pelota*)(&(p_arkanoPi->pelota)), (tipo_pantalla*)(&(p_arkanoPi->pantalla)));
	pintaRaqueta((tipo_raqueta*)(&(p_arkanoPi->raqueta)), (tipo_pantalla*)(&(p_arkanoPi->pantalla)));
	pintaLadrillos((tipo_pantalla*)(&(p_arkanoPi->ladrillos)), (tipo_pantalla*)(&(p_arkanoPi->pantalla)));
}

void inicializaArkanoPi(tipo_arkanoPi* p_arkanoPi) {
	/*
	Metodo encargado de la inicializacion de toda variable o estructura de datos especificamente ligada al
	desarrollo del juego y su visualizacion.
	*/
	piLock (STD_IO_BUFFER_KEY);
	printf("%s\n", "[LOG] InicializaArkanoPi");
	piUnlock (STD_IO_BUFFER_KEY);

	reseteaMatriz((tipo_pantalla*)(&(p_arkanoPi->pantalla)));
	reseteaLadrillos((tipo_pantalla*)(&(p_arkanoPi->ladrillos)));
	reseteaPelota((tipo_pelota*)(&(p_arkanoPi->pelota)));
	reseteaRaqueta((tipo_raqueta*)(&(p_arkanoPi->raqueta)));
}

int calculaLadrillosRestantes(tipo_pantalla* p_ladrillos) {
	/*
	Funcion encargada de evaluar el estado de ocupacion del area de juego por los ladrillos y devolver
	el numero de estos 
	*/
	piLock (STD_IO_BUFFER_KEY);
	printf("%s\n", "[LOG] CalculaLadrillosRestantes");
	piUnlock (STD_IO_BUFFER_KEY);

	int num_ladrillos_restantes = 0;
	int i, j = 0;
	for(i=0;i<MATRIZ_ANCHO;i++) {
		for(j=0;j<MATRIZ_ALTO;j++) {
			if(p_ladrillos->matriz[i][j] == 1){
				num_ladrillos_restantes++;
			}
		}
    }
	return num_ladrillos_restantes;
}
