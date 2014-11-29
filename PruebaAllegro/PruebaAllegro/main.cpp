#include <stdio.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_primitives.h>

#define SCREEN_W  1920
#define SCREEN_H  1000
#define ColorLetra al_map_rgb(50, 187, 164)
#define ColorNotas al_map_rgb(30, 157, 164)
#define ColorTitulo al_map_rgb(255, 255, 255)
#define ColorLinea al_map_rgb(0,0,0)
#define ButRunIzq  ( SCREEN_W / 2 - 100)
#define ButRunDer  ( SCREEN_W / 2 + 100)
#define ButRunTop  (SCREEN_H / 2 - 100 - 40)
#define ButRunBot  (SCREEN_H / 2 - 100 + 60)
#define ButSalirIzq  ( SCREEN_W / 2 - 50)
#define ButSalirDer ( SCREEN_W / 2 + 50)
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

typedef struct {
	float coordX1;
	float coordX2;
	float coordY;
} coordenadas;

void Fondo(void);
void crearCoord(float coordInicX, float coordInicY);
void dibujoActual(int Tecla);

const float FPS = 60;
const int BOUNCER_SIZE = 32;
int Estado = 0;
int EstadoTecla;
int EstadoLinea;
bool FirstTime = 0;
ALLEGRO_DISPLAY *display = NULL;

int main(int argc, char **argv)
{
	
	ALLEGRO_EVENT_QUEUE *event_queue = NULL;
	ALLEGRO_TIMER *timer = NULL;
	ALLEGRO_BITMAP *bouncer = NULL;
	ALLEGRO_BITMAP *image = NULL;
	ALLEGRO_BITMAP *Resis = NULL;
	float bouncer_x = SCREEN_W / 2.0 - BOUNCER_SIZE / 2.0;
	float bouncer_y = SCREEN_H / 2.0 - BOUNCER_SIZE / 2.0;
	bool redraw = true;
	float x1, x2, y1, y2;
	//Todos las partes a usar

	if (!al_init()) {
		fprintf(stderr, "failed to initialize allegro!\n");
		return -1;
	}

	if (!al_install_mouse()) {
		fprintf(stderr, "failed to initialize the mouse!\n");
		return -1;
	}

	timer = al_create_timer(1.0 / FPS);
	if (!timer) {
		fprintf(stderr, "failed to create timer!\n");
		return -1;
	}

	display = al_create_display(SCREEN_W, SCREEN_H);
	if (!display) {
		fprintf(stderr, "failed to create display!\n");
		al_destroy_timer(timer);
		return -1;
	}

	bouncer = al_create_bitmap(BOUNCER_SIZE, BOUNCER_SIZE);
	if (!bouncer) {
		fprintf(stderr, "failed to create bouncer bitmap!\n");
		al_destroy_display(display);
		al_destroy_timer(timer);
		return -1;
	}

	if (!al_init_image_addon()) {
		al_show_native_message_box(display, "Error", "Error", "Failed to initialize al_init_image_addon!",
			NULL, ALLEGRO_MESSAGEBOX_ERROR);
		return 0;
	}

	al_set_target_bitmap(bouncer);
	al_clear_to_color(al_map_rgb(150, 150, 150));
	al_set_target_bitmap(al_get_backbuffer(display));
	event_queue = al_create_event_queue();
	image = al_load_bitmap("Extras/Cursor.png");
	Resis = al_load_bitmap("Extras/Resis.png");

	if (!al_install_keyboard()) {
		fprintf(stderr, "failed to initialize the keyboard!\n");
		return -1;
	}

	if (!event_queue) {
		fprintf(stderr, "failed to create event_queue!\n");
		al_destroy_bitmap(bouncer);
		al_destroy_display(display);
		al_destroy_timer(timer);
		return -1;
	}

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
	//al_hide_mouse_cursor(display);

	Fondo();


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
		}
		else if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP) {
			if (Estado == 0) {
				if (bouncer_x < ButSalirDer && bouncer_x > ButSalirIzq)
					if (bouncer_y < ButSalirBot && bouncer_y > ButSalirTop)
						break;

				if (bouncer_x< ButRunDer && bouncer_x> ButRunIzq)
					if (bouncer_y < ButRunBot && bouncer_y > ButRunTop) {
						Estado = 1;
						Fondo();
					}
			}

			if (Estado == 1) {
				if (bouncer_x < ButSalir2Der && bouncer_x > ButSalir2Izq)
					if (bouncer_y < ButSalir2Top && bouncer_y > ButSalir2Bot)
						break;

				if (FirstTime == 0)
					FirstTime = 1;

				else if (EstadoTecla == 1) {
					al_draw_bitmap(Resis, bouncer_x, bouncer_y - 10, 0);
					crearCoord(bouncer_x,bouncer_y);
				}

				else if (EstadoTecla == 2) {

					if (EstadoLinea == 0) {
						x1 = bouncer_x;
						y1 = bouncer_y;
						EstadoLinea = 1;
					}
					else if (EstadoLinea == 1) {
						x2 = bouncer_x;
						y2 = bouncer_y;
						EstadoLinea = 0;
						al_draw_line(x1, y1, x2, y2, ColorLinea, 3);

					}



				}
			}
			al_flip_display();
		}

		else if (ev.type == ALLEGRO_EVENT_KEY_UP) {
			switch (ev.keyboard.keycode) {
			case ALLEGRO_KEY_R:
				EstadoTecla = 1;
				break;
			case ALLEGRO_KEY_L:
				EstadoTecla = 2;
				break;
			}
			dibujoActual(EstadoTecla);
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

void Fondo(void) {
	al_init_font_addon();
	al_init_ttf_addon();
	ALLEGRO_FONT *font = al_load_ttf_font("Extras/SouthernAire_Personal_Use_Only.ttf", 90, 0);
	ALLEGRO_FONT *notas = al_load_ttf_font("Extras/NormalSometimes-Regular.ttf", 20, 0);
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
		al_draw_text(notas,ColorTitulo, SCREEN_W * 0.03, SCREEN_H * 0.95, ALLEGRO_ALIGN_LEFT, "Presione R para resistencias y L para conectores");
		break;
	}
	al_flip_display();
	return;
}

void dibujoActual(int Tecla) {



	al_init_font_addon();
	al_init_ttf_addon();
	ALLEGRO_FONT *notas = al_load_ttf_font("Extras/NormalSometimes-Regular.ttf", 20, 0);


	ALLEGRO_BITMAP *bloqueador = NULL;
	bloqueador = al_create_bitmap(500, 22);
	al_set_target_bitmap(bloqueador);
	al_clear_to_color(al_map_rgb(50,50, 50));
	al_set_target_bitmap(al_get_backbuffer(display));


	al_draw_bitmap(bloqueador, SCREEN_W * 0.03, SCREEN_H * 0.90,0);
	al_flip_display();

	switch (Tecla) {
	case  1:
		al_draw_text(notas, ColorNotas, SCREEN_W * 0.03, SCREEN_H * 0.90, ALLEGRO_ALIGN_LEFT, "Estado Actual Resistencia");
		break;
	case 2:
		al_draw_text(notas, ColorNotas, SCREEN_W * 0.03, SCREEN_H * 0.90, ALLEGRO_ALIGN_LEFT, "Estado Actual Conductor");
		break;
	}
	al_flip_display();
}

void crearCoord(float coordInicX, float coordInicY) {
	static int ResisNum;
	static coordenadas ResisCoord[10];
	ResisCoord[ResisNum].coordX1 = coordInicX;
	ResisCoord[ResisNum].coordX2 = coordInicX + 75;
	ResisCoord[ResisNum].coordY = coordInicY;
	ResisNum++;
}