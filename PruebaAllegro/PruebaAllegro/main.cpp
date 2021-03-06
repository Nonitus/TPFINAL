#include <stdio.h>
#include <stdlib.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_primitives.h>

//Defines

#define SCREEN_W  1920
#define SCREEN_H  1000
#define ColorLetra al_map_rgb(50, 187, 164)
#define ColorNotas al_map_rgb(30, 157, 164)
#define ColorTitulo al_map_rgb(255, 255, 255)
#define ColorNros al_map_rgb(0,51,153)
#define ColorLinea al_map_rgb(0,0,0)
#define ButRunIzq  (SCREEN_W / 2 - 100)
#define ButRunDer  (SCREEN_W / 2 + 100)
#define ButRunTop  (SCREEN_H / 2 - 100 - 10)
#define ButRunBot  (SCREEN_H / 2 - 100 + 90)
#define ButSalirIzq  (SCREEN_W / 2 - 70)
#define ButSalirDer (SCREEN_W / 2 + 70)
#define ButSalirTop  (SCREEN_H / 2 + 140 - 40)
#define ButSalirBot  (SCREEN_H / 2 + 140 + 40)
#define ButSalir2Izq  (SCREEN_W * 0.9 - 80)
#define ButSalir2Der (SCREEN_W * 0.9 + 80)
#define ButSalir2Top  (SCREEN_H * 0.9 + 80)
#define ButSalir2Bot  (SCREEN_H * 0.9 - 80)
#define ButCalcIzq  (SCREEN_W * 0.9 - 80)
#define ButCalcDer (SCREEN_W * 0.9 + 80)
#define ButCalcTop  (SCREEN_H * 0.75 + 80)
#define ButCalcBot  (SCREEN_H * 0.75 - 80)
#define CantidadRes 50
#define MargenError 300
#define MargenDist 3000

//Estructuras

typedef struct {
	float coordX1;
	float coordX2;
	float coordY;
	int Valor;
} resistor;

typedef struct {
	float x;
	float y;
	int NroRes;
	bool borne; // 0=der--1=izq
} coordenadas;

typedef struct {
	int der;
	int izq;
}TERMINAL;


//Proto de funciones
void Fondo(ALLEGRO_FONT *font, ALLEGRO_FONT *notas);
void crearCoord(float coordInicX, float coordInicY, int ValorResis);
void dibujoActual(int Tecla, ALLEGRO_DISPLAY *display, int ValorResis);
coordenadas buscarCoinc(float bouncer_x, float bouncer_y);
void do_nothing(void);
bool distResis(float bouncer_x, float bouncer_y);
int ascii2int(void);
char* char2int(int numero);

//Variables globales
const float FPS = 60;
const int BOUNCER_SIZE = 32;
int Estado = 0;
int EstadoTecla;
int EstadoLinea;
bool FirstTime = 0;
resistor ResisCoord[CantidadRes];
char NroAscii[10];
//MATRIZ DE PRUEBA
TERMINAL nodos[CantidadRes][CantidadRes];


int main(int argc, char **argv)
{
	ALLEGRO_DISPLAY *display = NULL;
	ALLEGRO_EVENT_QUEUE *event_queue = NULL;
	ALLEGRO_TIMER *timer = NULL;
	ALLEGRO_BITMAP *bouncer = NULL;
	ALLEGRO_BITMAP *image = NULL;
	ALLEGRO_BITMAP *Resis = NULL;
	ALLEGRO_FONT *font = NULL;
	ALLEGRO_FONT *notas = NULL;
	ALLEGRO_FONT *nros = NULL;
	float bouncer_x = SCREEN_W / 2.0 - BOUNCER_SIZE / 2.0;
	float bouncer_y = SCREEN_H / 2.0 - BOUNCER_SIZE / 2.0;
	bool redraw = true;
	int ValorResis = 1;
	coordenadas puntoSelec1;
	coordenadas puntoSelec2;
	int v = 10;
	float aproxx, aproxy;

	//Todos las partes a usar
	al_init();
	al_install_mouse();
	timer = al_create_timer(1.0 / FPS);
	display = al_create_display(SCREEN_W, SCREEN_H);
	bouncer = al_create_bitmap(5000, 200);
	al_init_image_addon();
	al_set_target_bitmap(bouncer);
	al_clear_to_color(al_map_rgb(50, 50, 50));
	al_set_target_bitmap(al_get_backbuffer(display));
	event_queue = al_create_event_queue();
	image = al_load_bitmap("Extras/Cursor.png");
	Resis = al_load_bitmap("Extras/Resis.png");
	al_install_keyboard();
	al_init_font_addon();
	al_init_ttf_addon();
	font = al_load_ttf_font("Extras/SouthernAire_Personal_Use_Only.ttf", 90, 0);
	notas = al_load_ttf_font("Extras/NormalSometimes-Regular.ttf", 20, 0);
	nros = al_load_ttf_font("Extras/Big-Quicksand.ttf", 20, 0);

	//Registro Proveedores de ventos

	al_register_event_source(event_queue, al_get_display_event_source(display));
	al_register_event_source(event_queue, al_get_timer_event_source(timer));
	al_register_event_source(event_queue, al_get_mouse_event_source());
	al_register_event_source(event_queue, al_get_keyboard_event_source());
	al_init_primitives_addon();



	//Inicializo Pantalla

	al_clear_to_color(al_map_rgb(0, 0, 0));
	al_flip_display();
	al_start_timer(timer);
	Fondo(font, notas);


	//Aca arrancan los eventos

	while (1)
	{
		ALLEGRO_EVENT ev;
		al_wait_for_event(event_queue, &ev);

		if (ev.type == ALLEGRO_EVENT_TIMER) {
			redraw = true;
		}
		else if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
			break;
		}
		else if (ev.type == ALLEGRO_EVENT_MOUSE_AXES ||
			ev.type == ALLEGRO_EVENT_MOUSE_ENTER_DISPLAY) {

			bouncer_x = ev.mouse.x;
			bouncer_y = ev.mouse.y;

			//Cambio de mouse
			if (Estado == 0)
				if ((bouncer_x < ButSalirDer && bouncer_x > ButSalirIzq) && (bouncer_y < ButSalirBot && bouncer_y > ButSalirTop)) {
					al_draw_bitmap(bouncer, ButSalirIzq, ButSalirTop, 0);
					Fondo(font, notas);
					al_hide_mouse_cursor(display);
					al_draw_bitmap(image, bouncer_x, bouncer_y, 0);
					al_flip_display();
				}
				else if ((bouncer_x < ButRunDer && bouncer_x > ButRunIzq) && (bouncer_y < ButRunBot && bouncer_y > ButRunTop)) {
					al_draw_bitmap(bouncer, ButRunIzq, ButRunTop, 0);
					Fondo(font, notas);
					al_hide_mouse_cursor(display);
					al_draw_bitmap(image, bouncer_x, bouncer_y, 0);
					al_flip_display();
				}

				else {
					Fondo(font, notas);
					al_show_mouse_cursor(display);
				}

				if (Estado == 1)
					al_show_mouse_cursor(display);



		}
		else if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP) {
			if (Estado == 0) {
				if (bouncer_x < ButSalirDer && bouncer_x > ButSalirIzq)
					if (bouncer_y < ButSalirBot && bouncer_y > ButSalirTop)
						break;

				if (bouncer_x< ButRunDer && bouncer_x> ButRunIzq)
					if (bouncer_y < ButRunBot && bouncer_y > ButRunTop) {
						Estado = 1;
						Fondo(font, notas);
					}
			}

			if (Estado == 1) {
				if (bouncer_x < ButSalir2Der && bouncer_x > ButSalir2Izq)
					if (bouncer_y < ButSalir2Top && bouncer_y > ButSalir2Bot)
						break;
				// Aca meto el boton CALCULAR

				if (bouncer_x < ButCalcDer && bouncer_x > ButCalcIzq)
					if (bouncer_y < ButCalcTop && bouncer_y > ButCalcBot) {
						int j, k,l;

						//IMPRIMO

						for (j = 0; j < CantidadRes; j++) {
							for (k = 0; k < CantidadRes; k++) {
								printf("%d ", nodos[j][k].izq);
								printf("%d \t", nodos[j][k].der);
							}
							printf("\n");
						}
						printf("\n");

						//Muestro todas las conexiones


						for (j = 0; j <= CantidadRes; j++) {
							for (k = 0; k < CantidadRes; k++) {
								if (nodos[j][k].izq == 1) {	//miro si hay un uno a la izq de cada casilla
									if (nodos[k][j].izq == 1) { // miro si estoy conectado a la pata izq de algo
										for (l = 0; l < CantidadRes; l++) {//Busco conexiones de r2 por mismo borne que r1
											if (nodos[k][l].izq == 1) {
												if (l != j) {
													nodos[j][l].izq = 1;
												}
											}
										}

									}
									else {
										for (l = 0; l < CantidadRes; l++) {
											if (nodos[k][l].der == 1) {
												if (j != l) {
													nodos[j][l].izq = 1;
												}
											}
										}
									}
								}

								if (nodos[j][k].der == 1) {// Si no hay un uno a la izq, lo busco a la derecha
									if (nodos[k][j].izq == 1) {
										for (l = 0; l < CantidadRes; l++) {
											if (nodos[k][l].izq == 1) {
												if (l != j) {
													nodos[j][l].der = 1;
												}
											}
										}
									}
									else {
										for (l = 0; l < CantidadRes; l++) {
											if (nodos[k][l].der == 1) {
												if (j != l) {
													nodos[j][l].der = 1;
												}
											}
										}
									}

								}
							}
						}

						for (j = CantidadRes; j >= 0; j--) {
							for (k = CantidadRes; k > 0; k--) {
								if (nodos[j][k].izq == 1) {	//miro si hay un uno a la izq de cada casilla
									if (nodos[k][j].izq == 1) { // miro si estoy conectado a la pata izq de algo
										for (l = 0; l < CantidadRes; l++) {//Busco conexiones de r2 por mismo borne que r1
											if (nodos[k][l].izq == 1) {
												if (l != j) {
													nodos[j][l].izq = 1;
												}
											}
										}

									}
									else {
										for (l = 0; l < CantidadRes; l++) {
											if (nodos[k][l].der == 1) {
												if (j != l) {
													nodos[j][l].izq = 1;
												}
											}
										}
									}
								}

								else if (nodos[j][k].der == 1) {// Si no hay un uno a la izq, lo busco a la derecha
									if (nodos[k][j].izq == 1) {
										for (l = 0; l < CantidadRes; l++) {
											if (nodos[k][l].izq == 1) {
												if (l != j) {
													nodos[j][l].der = 1;
												}
											}
										}
									}
									else {
										for (l = 0; l < CantidadRes; l++) {
											if (nodos[k][l].der == 1) {
												if (j != l) {
													nodos[j][l].der = 1;
												}
											}
										}
									}

								}
							}
						}


		



						//IMPRIMO 

						for (j = 0; j < CantidadRes; j++) {
							for (k = 0; k < CantidadRes; k++) {
								printf("%d ", nodos[j][k].izq);
								printf("%d \t", nodos[j][k].der);
							}
							printf("\n");
						}
					}

				if (FirstTime == 0)
					FirstTime = 1;

				else if (EstadoTecla == 1) {
					if (distResis(bouncer_x, bouncer_y)) {
						for (aproxx = 0; aproxx <= 1920; aproxx += 80) {
							if (((bouncer_x - aproxx) < 40) && ((bouncer_x - aproxx) > -40))
								bouncer_x = aproxx;
						}
						for (aproxy = 0; aproxy <= 1080; aproxy += 80) {
							if (((bouncer_y - aproxy) < 40) && ((bouncer_y - aproxy) > -40))
								bouncer_y = aproxy;
						}
						al_draw_bitmap(Resis, bouncer_x+2, bouncer_y - 11, 0);
						crearCoord(bouncer_x, bouncer_y, ValorResis);
						al_draw_text(nros, ColorLetra, bouncer_x + 38, bouncer_y - 25, ALLEGRO_ALIGN_CENTER, NroAscii);

					}

					//Aca dibujo la resistencia
				}

				else if (EstadoTecla == 2) {

					if (EstadoLinea == 0) {

						puntoSelec1 = buscarCoinc(bouncer_x, bouncer_y);
						EstadoLinea = 1;
					}
					else if (EstadoLinea == 1) {
						puntoSelec2 = buscarCoinc(bouncer_x, bouncer_y);
						EstadoLinea = 0;
						if (puntoSelec1.x == 0 || puntoSelec1.y == 0 || puntoSelec2.y == 0 || puntoSelec2.y == 0)
							do_nothing();
						else {
							al_draw_line(puntoSelec1.x, puntoSelec1.y, puntoSelec2.x, puntoSelec2.y, ColorLinea, 3);
							if (puntoSelec1.borne == 0)
								nodos[puntoSelec1.NroRes][puntoSelec2.NroRes].der = 1;
							else
								nodos[puntoSelec1.NroRes][puntoSelec2.NroRes].izq = 1;
							if (puntoSelec2.borne == 0)
								nodos[puntoSelec2.NroRes][puntoSelec1.NroRes].der = 1;
							else
								nodos[puntoSelec2.NroRes][puntoSelec1.NroRes].izq = 1;
							//Aca dibujo la linea
						}
					}



				}
			}
			al_flip_display();
		}

		else if (ev.type == ALLEGRO_EVENT_KEY_UP) {
			switch (ev.keyboard.keycode) {
			case ALLEGRO_KEY_R:
				EstadoTecla = 1;
				printf("Ingrese el valor de la resistencia ");
				//DEBUG
				//ValorResis = ascii2int();
				ValorResis++;
				break;
			case ALLEGRO_KEY_L:
				EstadoTecla = 2;
				break;
			}
			dibujoActual(EstadoTecla, display, ValorResis);
		}



		if (redraw && al_is_event_queue_empty(event_queue)) {
			redraw = false;
			//al_draw_bitmap(bouncer, bouncer_x - BOUNCER_SIZE/2, bouncer_y - BOUNCER_SIZE/2, 0); 
			//al_draw_bitmap(image, bouncer_x, bouncer_y, 0);
			//al_flip_display();
		}
	}

	al_destroy_bitmap(bouncer);
	al_destroy_timer(timer);
	al_destroy_display(display);
	al_destroy_event_queue(event_queue);

	return 0;
}

void Fondo(ALLEGRO_FONT *font, ALLEGRO_FONT *notas) {
	al_clear_to_color(al_map_rgb(50, 50, 50)); //COLOR DE FONDO DE TODO
	switch (Estado) {
	case  0:
		al_draw_text(font, ColorTitulo, SCREEN_W / 2, SCREEN_H / 2 - 300, ALLEGRO_ALIGN_CENTRE, "TP     FINAL      PROGRAMACION");
		al_draw_text(font, ColorLetra, SCREEN_W / 2, SCREEN_H / 2 - 100, ALLEGRO_ALIGN_CENTRE, "Comenzar");
		al_draw_text(font, ColorLetra, SCREEN_W / 2, SCREEN_H / 2 + 100, ALLEGRO_ALIGN_CENTRE, "Salir");
		break;
	case 1:
		al_draw_text(font, ColorLetra, SCREEN_W / 2, SCREEN_H * 0.1, ALLEGRO_ALIGN_CENTRE, "Ingrese Resistencias");
		al_draw_text(font, ColorTitulo, SCREEN_W * 0.9, SCREEN_H * 0.75, ALLEGRO_ALIGN_CENTRE, "Calcular");
		al_draw_text(font, ColorLetra, SCREEN_W * 0.9, SCREEN_H * 0.85, ALLEGRO_ALIGN_CENTRE, "Salir");
		al_draw_text(font, ColorLetra, SCREEN_W * 0.2, SCREEN_H * 0.5, ALLEGRO_ALIGN_CENTRE, "Inicio");
		al_draw_text(font, ColorLetra, SCREEN_W * 0.8, SCREEN_H * 0.5, ALLEGRO_ALIGN_CENTRE, "Fin");
		al_draw_line(480, 560, 560, 560, al_map_rgb(0, 0, 0),3);
		al_draw_line(1360, 560, 1440, 560, al_map_rgb(0, 0, 0), 3);
		float ptox, ptoy;
		for (ptox = 0; ptox <= 1920; ptox += 80) {
			for (ptoy = 0; ptoy <= 1080; ptoy += 80) 
				al_draw_pixel(ptox, ptoy, ColorLetra);
			
		}
		al_draw_filled_circle(560, 560, 4, al_map_rgb(0, 0, 0));
		al_draw_filled_circle(1360, 560, 4, al_map_rgb(0, 0, 0));
		al_draw_text(notas, ColorTitulo, SCREEN_W * 0.03, SCREEN_H * 0.95, ALLEGRO_ALIGN_LEFT, "Presione R para resistencias y L para conectores");
		break;
	}

	al_flip_display();
	return;
}
void dibujoActual(int Tecla, ALLEGRO_DISPLAY *display, int ValorResis) {

	al_init_font_addon();
	al_init_ttf_addon();
	ALLEGRO_FONT *notas = al_load_ttf_font("Extras/NormalSometimes-Regular.ttf", 20, 0);
	ALLEGRO_FONT *nros = al_load_ttf_font("Extras/Big-Quicksand.ttf", 20, 0);

	ALLEGRO_BITMAP *bloqueador = NULL;
	bloqueador = al_create_bitmap(600, 22);
	al_set_target_bitmap(bloqueador);
	al_clear_to_color(al_map_rgb(50, 50, 50));
	al_set_target_bitmap(al_get_backbuffer(display));


	al_draw_bitmap(bloqueador, SCREEN_W * 0.03, SCREEN_H * 0.90, 0);
	al_flip_display();


	switch (Tecla) {
	case  1:
		al_draw_text(notas, ColorNotas, SCREEN_W * 0.03, SCREEN_H * 0.90, ALLEGRO_ALIGN_LEFT, "Estado Actual Resistencia            ohms");
		al_draw_text(nros, ColorNotas, SCREEN_W * 0.2, SCREEN_H * 0.90, ALLEGRO_ALIGN_LEFT, NroAscii);
		break;
	case 2:
		al_draw_text(notas, ColorNotas, SCREEN_W * 0.03, SCREEN_H * 0.90, ALLEGRO_ALIGN_LEFT, "Estado Actual Conductor");
		break;
	}
	al_flip_display();
}
void crearCoord(float coordInicX, float coordInicY, int ValorResis) {
	int contx, conty;
	int pto = 0;
	static int ResisNum;
	ResisCoord[ResisNum].coordX1 = coordInicX;
	ResisCoord[ResisNum].coordX2 = coordInicX + 75;
	ResisCoord[ResisNum].coordY = coordInicY;
	ResisCoord[ResisNum].Valor = ValorResis;
	//Veo propiedades de las resis, para debug
	printf("Resistencia Numero %d, Coord X1 %f, Coord X2 %f, Coord Y1 %f, Valor Resistivo %d \n", ResisNum, ResisCoord[ResisNum].coordX1, ResisCoord[ResisNum].coordX2, ResisCoord[ResisNum].coordY, ResisCoord[ResisNum].Valor);
	for (contx = 0; contx < 1920; contx++)
		for (conty = 0; conty < 1080; conty++)
			if (((coordInicX - contx * 80) == 0) && ((coordInicY - conty * 80) == 0))
				pto = 10*conty + contx;
	printf("Resistencia conectada al pto %d \n",pto);
	ResisNum++;
}
coordenadas buscarCoinc(float bouncer_x, float bouncer_y) {
	coordenadas Devuelvo;
	Devuelvo.x = 0;
	Devuelvo.y = 0;
	Devuelvo.NroRes = -1;
	int i;
	for (i = 0; i <= CantidadRes; i++)
		if (((bouncer_x - ResisCoord[i].coordX1)*(bouncer_x - ResisCoord[i].coordX1) + ((bouncer_y - ResisCoord[i].coordY)*(bouncer_y - ResisCoord[i].coordY))) < MargenError) {
			Devuelvo.x = ResisCoord[i].coordX1;
			Devuelvo.y = ResisCoord[i].coordY;
			Devuelvo.NroRes = i;
			Devuelvo.borne = 1;
		}
		else if (((bouncer_x - ResisCoord[i].coordX2)*(bouncer_x - ResisCoord[i].coordX2) + ((bouncer_y - ResisCoord[i].coordY)*(bouncer_y - ResisCoord[i].coordY))) < MargenError) {
			Devuelvo.x = ResisCoord[i].coordX2;
			Devuelvo.y = ResisCoord[i].coordY;
			Devuelvo.NroRes = i;
			Devuelvo.borne = 0;
		}

		return Devuelvo;
}
bool distResis(float bouncer_x, float bouncer_y) {
	int i;
	for (i = 0; i <= CantidadRes; i++) {
		if (((bouncer_x - ResisCoord[i].coordX1)*(bouncer_x - ResisCoord[i].coordX1) + ((bouncer_y - ResisCoord[i].coordY)*(bouncer_y - ResisCoord[i].coordY))) < MargenDist) {
			return 0;
		}
		else if (((bouncer_x - ResisCoord[i].coordX2)*(bouncer_x - ResisCoord[i].coordX2) + ((bouncer_y - ResisCoord[i].coordY)*(bouncer_y - ResisCoord[i].coordY))) < MargenDist) {
			return 0;
		}
	}
	return 1;
}
void do_nothing(void) {
	return;
}
int ascii2int(void)
{
	int num = 0;
	int c, i = 0;
	while ((c = getchar()) != '\n')
	{
		c -= 48;
		if (c >= 0 && c <= 9) {
			num = ((num * 10) + c);
			NroAscii[i] = c + 48;
			i++;
		}
		else
		{
			printf("NaN\n");
			break;
		}
	}
	NroAscii[i] = 0;
	return (num);
}
