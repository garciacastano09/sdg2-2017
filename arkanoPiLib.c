#include "arkanoPiLib.h"

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
		{0,0,0,0,0,0,0},
		{0,0,0,0,1,1,1},
		{1,1,1,0,1,1,0},
		{1,0,1,0,1,1,1},
		{1,1,1,0,0,0,0},
		{0,0,0,0,0,0,1},
		{1,1,1,0,0,0,1},
		{0,1,0,0,1,1,1},
		{1,1,1,0,0,0,0},
		{0,0,0,0,0,0,0},
};

//------------------------------------------------------
// FUNCIONES DE INICIALIZACION / RESET
//------------------------------------------------------

void ReseteaMatriz(tipo_pantalla *p_pantalla) {
	printf("%s\n", "[LOG] ReseteaMatriz")
	int i, j = 0;

	for(i=0;i<MATRIZ_ANCHO;i++) {
		for(j=0;j<MATRIZ_ALTO;j++) {
			p_pantalla->matriz[i][j] = 0;
		}
	}
}

void ReseteaLadrillos(tipo_pantalla *p_ladrillos) {
	printf("%s\n", "[LOG] ReseteaLadrillos")

	int i, j = 0;

	for(i=0;i<MATRIZ_ANCHO;i++) {
		for(j=0;j<MATRIZ_ALTO;j++) {
			p_ladrillos->matriz[i][j] = ladrillos_basico[i][j];
		}
	}
}

void ReseteaPelota(tipo_pelota *p_pelota) {
	// Pelota inicialmente en el centro de la pantalla
	printf("%s\n", "[LOG] ReseteaPelota")
	p_pelota->x = MATRIZ_ANCHO/2 - 1;
	p_pelota->y = MATRIZ_ALTO/2;

	// Trayectoria inicial
	p_pelota->y = MATRIZ_ALTO/2-1;
	p_pelota->xv = -1;
	p_pelota->yv = 1;
	p_pelota->xv = 0;
}

void ReseteaRaqueta(tipo_raqueta *p_raqueta) {
	printf("%s\n", "[LOG] ReseteaRaqueta")
	// Raqueta inicialmente en el centro de la pantalla
	p_raqueta->x = MATRIZ_ANCHO/2 - p_raqueta->ancho/2;
	p_raqueta->y = MATRIZ_ALTO - 1;
	p_raqueta->ancho = RAQUETA_ANCHO;
	p_raqueta->alto = RAQUETA_ALTO;
}

void RellenaMatriz(tipo_pantalla *p_pantalla, int[][] *nueva_matriz) {
	printf("%s\n", "[LOG] RellenaMatriz")
	int i, j = 0;

	for(i=0;i<MATRIZ_ANCHO;i++) {
		for(j=0;j<MATRIZ_ALTO;j++) {
			p_pantalla->matriz[i][j] = *nueva_matriz[i][j];
		}
	}
}

//------------------------------------------------------
// FUNCIONES DE VISUALIZACION (ACTUALIZACION DEL OBJETO PANTALLA QUE LUEGO USARA EL DISPLAY)
//------------------------------------------------------

// void PintaMensajeInicialPantalla (...): metodo encargado de aprovechar
// el display para presentar un mensaje de bienvenida al usuario
void PintaMensajeInicialPantalla (tipo_pantalla *p_pantalla, tipo_pantalla *p_pantalla_inicial) {
	printf("%s\n", "[LOG] PintaMensajeInicialPantalla");
	// TODO ¿Que es el objeto pantalla inicial?
	RellenaMatriz(p_pantalla, &(mensaje_inicial));
	PintaPantallaPorTerminal(p_pantalla);
}

// void PintaPantallaPorTerminal (...): metodo encargado de mostrar
// el contenido o la ocupación de la matriz de leds en la ventana de
// terminal o consola. Este método sera fundamental para facilitar
// la labor de depuración de errores (por ejemplo, en la programación
// de los diferentes movimientos tanto de la raqueta como de la pelota).
void PintaPantallaPorTerminal  (tipo_pantalla *p_pantalla) {
	printf("%s\n", "[LOG] PintaPantallaPorTerminal");
	printf("%s\n", "---PANTALLA---");
	for(i=0;i<MATRIZ_ANCHO;i++) {
		for(j=0;j<MATRIZ_ALTO;j++) {
			printf("%s\n", p_pantalla->matriz[i][j]);
		}
		printf("%s\n", "");
	}
}

// void PintaLadrillos(...): funcion encargada de “pintar” los ladrillos
// en sus correspondientes posiciones dentro del área de juego
void PintaLadrillos(tipo_pantalla *p_ladrillos, tipo_pantalla *p_pantalla) {
	printf("%s\n", "[LOG] PintaLadrillos");
	int i, j = 0;
	for(i=0;i<MATRIZ_ANCHO;i++) {
		for(j=0;j<MATRIZ_ALTO;j++) {
			p_pantalla->matriz[i][j] = p_ladrillos->matriz[i][j];
		}
    }
}

// void PintaRaqueta(...): funcion encargada de “pintar” la raqueta
// en su posicion correspondiente dentro del área de juego
void PintaRaqueta(tipo_raqueta *p_raqueta, tipo_pantalla *p_pantalla) {
	printf("%s\n", "[LOG] PintaRaqueta");
	int i, j = 0;
	for(i=0;i<RAQUETA_ANCHO;i++) {
		for(j=0;j<RAQUETA_ALTO;j++) {
			if (( (p_raqueta->x+i >= 0) && (p_raqueta->x+i < MATRIZ_ANCHO) ) &&
					( (p_raqueta->y+j >= 0) && (p_raqueta->y+j < MATRIZ_ALTO) ))
				p_pantalla->matriz[p_raqueta->x+i][p_raqueta->y+j] = 1;
		}
	}
}

// void PintaPelota(...): funcion encargada de “pintar” la pelota
// en su posicion correspondiente dentro del área de juego
void PintaPelota(tipo_pelota *p_pelota, tipo_pantalla *p_pantalla) {
	printf("%s\n", "[LOG] PintaPelota");
	if( (p_pelota->x >= 0) && (p_pelota->x < MATRIZ_ANCHO) ) {
		if( (p_pelota->y >= 0) && (p_pelota->y < MATRIZ_ALTO) ) {
			p_pantalla->matriz[p_pelota->x][p_pelota->y] = 1;
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
// en memoria que representa el área de juego y su correspondiente estado.
void ActualizaPantalla(tipo_arkanoPi* p_arkanoPi) {
	printf("%s\n", "[LOG] ActualizaPantalla");
    // Borro toda la pantalla
	ReseteaMatriz((tipo_pantalla*)(&(p_arkanoPi->pantalla)));
	// Pinta de nuevo los valores de pantalla, ladrillos y raqueta que hay en arkanoPi
	PintaPelota((tipo_pelota*)(&(p_arkanoPi->pelota)), (tipo_pantalla*)(&(p_arkanoPi->pantalla)));
	PintaRaqueta((tipo_raqueta*)(&(p_arkanoPi->raqueta)), (tipo_pantalla*)(&(p_arkanoPi->pantalla)));
	PintaLadrillos((tipo_ladrillos*)(&(p_arkanoPi->ladrillos)), (tipo_pantalla*)(&(p_arkanoPi->pantalla)));
}

// void InicializaArkanoPi(...): metodo encargado de la inicialización
// de toda variable o estructura de datos especificamente ligada al
// desarrollo del juego y su visualizacion.
void InicializaArkanoPi(tipo_arkanoPi *p_arkanoPi) {
	printf("%s\n", "[LOG] InicializaArkanoPi");
	// TODO ¿Que diferencia hay entre este metodo y InicializaJuego? ¿Que mas hace falta?
	ReseteaMatriz((tipo_pantalla*)(&(p_arkanoPi->pantalla)));
	ReseteaLadrillos((tipo_pantalla*)(&(p_arkanoPi->ladrillos)));
	ReseteaPelota((tipo_pelota*)(&(p_arkanoPi->pelota)));
	ReseteaRaqueta((tipo_raqueta*)(&(p_arkanoPi->raqueta)));
}

// int CalculaLadrillosRestantes(...): función encargada de evaluar
// el estado de ocupacion del area de juego por los ladrillos y
// devolver el numero de estos
int CalculaLadrillosRestantes(tipo_pantalla *p_ladrillos) {
	printf("%s\n", "[LOG] CalculaLadrillosRestantes");
	int num_ladrillos_restantes = 0;

	for(i=0;i<MATRIZ_ANCHO;i++) {
		for(j=0;j<MATRIZ_ALTO;j++) {
			if(p_ladrillos->matriz[i][j] == 1){
				num_ladrillos_restantes++;
			}
		}
    }
	return num_ladrillos_restantes;
}
