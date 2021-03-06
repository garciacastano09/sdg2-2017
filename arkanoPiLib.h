
#ifndef _ARKANOPILIB_H_
#define _ARKANOPILIB_H_

#include <stdio.h>

// CONSTANTES DEL JUEGO
#define MATRIZ_ANCHO 	10
#define MATRIZ_ALTO 	7
#define LADRILLOS_ANCHO 10
#define LADRILLOS_ALTO 	2
#define RAQUETA_ANCHO 		3
#define RAQUETA_ALTO 		1
#define MAX_X_RAQUETA 		9
#define MIN_X_RAQUETA 		0
#define REBOTE_LADRILLO		94
#define REBOTE_RAQUETA		95
#define REBOTE_PERDIDA		96
#define REBOTE_TECHO		97
#define REBOTE_LATERAL		98
#define NO_REBOTE			99

//------------------------------------------------------------------
// STRUCTS
//------------------------------------------------------------------
typedef struct {
	// Posicion
	int x;
	int y;
	// Forma
	int ancho;
	int alto;
} tipo_raqueta;

typedef struct {
	// Posicion
	int x;
	int y;
	// Trayectoria
	int xv;
	int yv;
} tipo_pelota;

typedef struct {
	// Matriz de ocupaci�n de las distintas posiciones que conforman el display
	// (correspondiente al estado encendido/apagado de cada uno de los leds)
	int matriz[MATRIZ_ANCHO][MATRIZ_ALTO];
} tipo_pantalla;

typedef struct {
  tipo_pantalla ladrillos; // Notese que, por simplicidad, los ladrillos comparten tipo con la pantalla
  tipo_pantalla pantalla;
  tipo_raqueta raqueta;
  tipo_pelota pelota;
} tipo_arkanoPi;

extern tipo_pantalla pantalla_inicial;

//------------------------------------------------------
// FUNCIONES DE INICIALIZACION / RESET
//------------------------------------------------------
void reseteaMatriz(tipo_pantalla* p_pantalla);
void reseteaLadrillos(tipo_pantalla* p_ladrillos);
void reseteaPelota(tipo_pelota* p_pelota);
void reseteaRaqueta(tipo_raqueta* p_raqueta);

//------------------------------------------------------
// FUNCIONES DE VISUALIZACION (ACTUALIZACION DEL OBJETO PANTALLA QUE LUEGO USARA EL DISPLAY)
//------------------------------------------------------
void pintaMensajeInicial(tipo_pantalla* p_pantalla);
void pintaPantallaPorTerminal (tipo_pantalla* p_pantalla);
void pintaLadrillos(tipo_pantalla* p_ladrillos, tipo_pantalla* p_pantalla);
void pintaRaqueta(tipo_raqueta* p_raqueta, tipo_pantalla* p_pantalla);
void pintaPelota(tipo_pelota* p_pelota, tipo_pantalla* p_pantalla);
void actualizaPantalla(tipo_arkanoPi* p_arkanoPi);
void inicializaArkanoPi(tipo_arkanoPi* p_arkanoPi);
int calculaLadrillosRestantes(tipo_pantalla* p_ladrillos);
void pintaMensajeVictoria(tipo_pantalla* p_pantalla);
void pintaMensajeDerrota(tipo_pantalla* p_pantalla);

#endif /* _ARKANOPILIB_H_ */
