#include <stdio.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>

#define ColorLetra al_map_rgb(50, 187, 164)

void Fondo(void);

const float FPS = 60;
const int SCREEN_W = 1920;
const int SCREEN_H = 1000;
const int BOUNCER_SIZE = 32;
const int ButRunIzq = (1920 / 2 - 50);
const int ButRunDer = (1920 / 2 + 50);
const int ButRunTop = (1080 / 2 - 100 - 36);
const int ButRunBot = (1080 / 2 - 100 + 36);
const int ButSalirIzq = (1920 / 2 - 50);
const int ButSalirDer = (1920 / 2 + 50);
const int ButSalirTop = (1080 / 2  + 100 - 36);
const int ButSalirBot = (1080 / 2  + 100 + 36);
int Estado = 0;
bool FirstTime = 0; 

int main(int argc, char **argv)
{
	ALLEGRO_DISPLAY *display = NULL;
	ALLEGRO_EVENT_QUEUE *event_queue = NULL;
	ALLEGRO_TIMER *timer = NULL;
	ALLEGRO_BITMAP *bouncer = NULL;
	ALLEGRO_BITMAP *image = NULL;
	ALLEGRO_BITMAP *image2 = NULL;
	float bouncer_x = SCREEN_W / 2.0 - BOUNCER_SIZE / 2.0;
	float bouncer_y = SCREEN_H / 2.0 - BOUNCER_SIZE / 2.0;
	bool redraw = true;

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
	image2 = al_load_bitmap("Extras/Resis.png");





	if (!event_queue) {
		fprintf(stderr, "failed to create event_queue!\n");
		al_destroy_bitmap(bouncer);
		al_destroy_display(display);
		al_destroy_timer(timer);
		return -1;
	}

	al_register_event_source(event_queue, al_get_display_event_source(display));
	al_register_event_source(event_queue, al_get_timer_event_source(timer));
	al_register_event_source(event_queue, al_get_mouse_event_source());
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
						Estado=1;
						Fondo();
					}
			}
			if (Estado == 1) {
				al_rest(2);
				Estado = 2;
			}

			if (Estado == 2) {
				if (FirstTime == 0)
					FirstTime = 1;
				else {
					al_draw_bitmap(image2, bouncer_x, bouncer_y, 0);
					al_flip_display();
				}
			}
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
	al_init_font_addon(); // initialize the font addon
	al_init_ttf_addon();// initialize the ttf (True Type Font) addon
	ALLEGRO_FONT *font = al_load_ttf_font("Extras/SouthernAire_Personal_Use_Only.ttf", 72, 0);
	al_clear_to_color(al_map_rgb(50, 50, 50)); //COLOR DE FONDO DE TODO
	switch (Estado) {
		case  0:
			al_draw_text(font, ColorLetra, SCREEN_W / 2, SCREEN_H / 2 - 100, ALLEGRO_ALIGN_CENTRE, "Comenzar");
			al_draw_text(font, ColorLetra, SCREEN_W / 2, SCREEN_H / 2 + 100, ALLEGRO_ALIGN_CENTRE, "Salir");
			break;
		case 1:
			al_draw_text(font, ColorLetra, SCREEN_W / 2, 100, ALLEGRO_ALIGN_CENTRE, "Ingrese Resistencias");
			break;
	}
	al_flip_display();
	return;
}