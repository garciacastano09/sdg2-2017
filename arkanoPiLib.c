#include "arkanoPiLib.h"
#include <wiringPi.h>

int ladrillos_basico[MATRIZ_ANCHO][MATRIZ_ALTO] = {
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

int mensaje_inicial[MATRIZ_ANCHO][MATRIZ_ALTO] = {
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

//------------------------------------------------------
// FUNCIONES DE INICIALIZACION / RESET
//------------------------------------------------------

void ReseteaMatriz(tipo_pantalla *p_pantalla) {
	printf("%s\n", "[LOG] ReseteaMatriz");
	int i, j = 0;
	for(i=0;i<MATRIZ_ANCHO;i++) {
		for(j=0;j<MATRIZ_ALTO;j++) {
			piLock(PANTALLA_KEY);
			p_pantalla->matriz[i][j] = 0;
			piUnlock(PANTALLA_KEY);
		}
	}
}

void ReseteaLadrillos(tipo_pantalla *p_ladrillos) {
	printf("%s\n", "[LOG] ReseteaLadrillos");
	int i, j = 0;
	for(i=0;i<MATRIZ_ANCHO;i++) {
		for(j=0;j<MATRIZ_ALTO;j++) {
			p_ladrillos->matriz[i][j] = ladrillos_basico[i][j];
		}
	}
}

void ReseteaPelota(tipo_pelota *p_pelota) {
	// Pelota inicialmente en el centro de la pantalla
	printf("%s\n", "[LOG] ReseteaPelota");
	p_pelota->x = MATRIZ_ANCHO/2 - 1;
	p_pelota->y = MATRIZ_ALTO/2;

	// Trayectoria inicial
	p_pelota->y = MATRIZ_ALTO/2-1;
	p_pelota->xv = -1;
	p_pelota->yv = 1;
	p_pelota->xv = 0;
}

void ReseteaRaqueta(tipo_raqueta *p_raqueta) {
	printf("%s\n", "[LOG] ReseteaRaqueta");
	// Raqueta inicialmente en el centro de la pantalla
	p_raqueta->x = MATRIZ_ANCHO/2 - p_raqueta->ancho/2;
	p_raqueta->y = MATRIZ_ALTO - 1;
	p_raqueta->ancho = RAQUETA_ANCHO;
	p_raqueta->alto = RAQUETA_ALTO;
}

void PintaMensajeInicial(tipo_pantalla *p_pantalla){
	printf("%s\n", "[LOG] RellenaMatriz");
	int i, j = 0;
	for(i=0;i<MATRIZ_ANCHO;i++) {
		for(j=0;j<MATRIZ_ALTO;j++) {
			piLock(PANTALLA_KEY);
			p_pantalla->matriz[i][j] = mensaje_inicial[i][j];
			piUnlock(PANTALLA_KEY);
		}
	}
}

//------------------------------------------------------
// FUNCIONES DE VISUALIZACION (ACTUALIZACION DEL OBJETO PANTALLA QUE LUEGO USARA EL DISPLAY)
//------------------------------------------------------
// void PintaPantallaPorTerminal (...): metodo encargado de mostrar
// el contenido o la ocupaci�n de la matriz de leds en la ventana de
// terminal o consola. Este m�todo sera fundamental para facilitar
// la labor de depuraci�n de errores (por ejemplo, en la programaci�n
// de los diferentes movimientos tanto de la raqueta como de la pelota).
void PintaPantallaPorTerminal  (tipo_pantalla *p_pantalla) {
	printf("%s\n", "[LOG] PintaPantallaPorTerminal");
	printf("%s\n", "---PANTALLA---");
	int i, j = 0;
	for(j=0;j<MATRIZ_ALTO;j++) {
		for(i=0;i<MATRIZ_ANCHO;i++) {
			printf("%d ", p_pantalla->matriz[i][j]);
		}
		printf("%s\n", "");
	}
}

// void PintaLadrillos(...): funcion encargada de �pintar� los ladrillos
// en sus correspondientes posiciones dentro del �rea de juego
void PintaLadrillos(tipo_pantalla *p_ladrillos, tipo_pantalla *p_pantalla) {
	printf("%s\n", "[LOG] PintaLadrillos");
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

// void PintaRaqueta(...): funcion encargada de �pintar� la raqueta
// en su posicion correspondiente dentro del �rea de juego
void PintaRaqueta(tipo_raqueta *p_raqueta, tipo_pantalla *p_pantalla) {
	printf("%s\n", "[LOG] PintaRaqueta");
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

// void PintaPelota(...): funcion encargada de �pintar� la pelota
// en su posicion correspondiente dentro del �rea de juego
void PintaPelota(tipo_pelota *p_pelota, tipo_pantalla *p_pantalla) {
	printf("%s\n", "[LOG] PintaPelota");
	if( (p_pelota->x >= 0) && (p_pelota->x < MATRIZ_ANCHO) ) {
		if( (p_pelota->y >= 0) && (p_pelota->y < MATRIZ_ALTO) ) {
			piLock(PANTALLA_KEY);
			p_pantalla->matriz[p_pelota->x][p_pelota->y] = 1;
			piUnlock(PANTALLA_KEY);
		}
		else {
			printf("\n\nPROBLEMAS!!!! posicion y=%d de la pelota INVALIDA!!!\n\n", p_pelota->y);
			fflush(stdout);
		}
	}
	else {
		printf("\n\nPROBLEMAS!!!! posicion x=%d de la pelota INVALIDA!!!\n\n", p_pelota->x);
		fflush(stdout);
	}
}

// void ActualizaPantalla(...): metodo cuya ejecucion estara ligada a
// cualquiera de los movimientos de la raqueta o de la pelota y que
// sera el encargado de actualizar convenientemente la estructura de datos
// en memoria que representa el �rea de juego y su correspondiente estado.
void ActualizaPantalla(tipo_arkanoPi* p_arkanoPi) {
	printf("%s\n", "[LOG] ActualizaPantalla");
    // Borro toda la pantalla
	ReseteaMatriz((tipo_pantalla*)(&(p_arkanoPi->pantalla)));
	// Pinta de nuevo los valores de pantalla, ladrillos y raqueta que hay en arkanoPi
	PintaPelota((tipo_pelota*)(&(p_arkanoPi->pelota)), (tipo_pantalla*)(&(p_arkanoPi->pantalla)));
	PintaRaqueta((tipo_raqueta*)(&(p_arkanoPi->raqueta)), (tipo_pantalla*)(&(p_arkanoPi->pantalla)));
	PintaLadrillos((tipo_pantalla*)(&(p_arkanoPi->ladrillos)), (tipo_pantalla*)(&(p_arkanoPi->pantalla)));
}

// void InicializaArkanoPi(...): metodo encargado de la inicializaci�n
// de toda variable o estructura de datos especificamente ligada al
// desarrollo del juego y su visualizacion.
void InicializaArkanoPi(tipo_arkanoPi *p_arkanoPi) {
	printf("%s\n", "[LOG] InicializaArkanoPi");
	ReseteaMatriz((tipo_pantalla*)(&(p_arkanoPi->pantalla)));
	ReseteaLadrillos((tipo_pantalla*)(&(p_arkanoPi->ladrillos)));
	ReseteaPelota((tipo_pelota*)(&(p_arkanoPi->pelota)));
	ReseteaRaqueta((tipo_raqueta*)(&(p_arkanoPi->raqueta)));
}

// int CalculaLadrillosRestantes(...): funci�n encargada de evaluar
// el estado de ocupacion del area de juego por los ladrillos y
// devolver el numero de estos
int CalculaLadrillosRestantes(tipo_pantalla *p_ladrillos) {
	printf("%s\n", "[LOG] CalculaLadrillosRestantes");
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
