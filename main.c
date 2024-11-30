#include <stdio.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <stdbool.h>

#define SCREEN_W 800
#define SCREEN_H 600
#define PLAYER_W 50
#define PLAYER_H 30
#define BULLET_W 5
#define BULLET_H 10
#define ALIEN_W 40
#define ALIEN_H 30

int main() {
    if (!al_init()) {
        fprintf(stderr, "Falha ao inicializar Allegro.\n");
        return -1;
    }

    if (!al_install_keyboard()) {
        fprintf(stderr, "Falha ao inicializar o teclado.\n");
        return -1;
    }

    if (!al_init_primitives_addon()) {
        fprintf(stderr, "Falha ao inicializar o addon de primitivas.\n");
        return -1;
    }



    ALLEGRO_DISPLAY *display = al_create_display(SCREEN_W, SCREEN_H);
    if (!display) {
        fprintf(stderr, "Falha ao criar display.\n");
        return -1;
    }

    ALLEGRO_FONT *font = al_create_builtin_font();
    ALLEGRO_EVENT_QUEUE *event_queue = al_create_event_queue();
    ALLEGRO_TIMER *timer = al_create_timer(1.0 / 60);

    if (!event_queue || !timer || !font) {
        fprintf(stderr, "Falha ao criar recursos principais.\n");
        al_destroy_display(display);
        return -1;
    }

    al_register_event_source(event_queue, al_get_display_event_source(display));
    al_register_event_source(event_queue, al_get_timer_event_source(timer));
    al_register_event_source(event_queue, al_get_keyboard_event_source());


    //definindo fun��es primarias
    bool running = true;
    bool redraw = true;
    float player_x = SCREEN_W / 2.0 - PLAYER_W / 2.0;
    float player_y = SCREEN_H - PLAYER_H - 10;
    int score = 0;



    al_start_timer(timer);

    while (running) {
        ALLEGRO_EVENT event;
        al_wait_for_event(event_queue, &event);

        /* inputs */

         /*fechar jogo ao apertar esc*/
         if (event.keyboard.keycode == ALLEGRO_KEY_ESCAPE){
            running = false;
         }

        /* movimentando player (letf - right*/
        if (event.keyboard.keycode == ALLEGRO_KEY_LEFT){
            player_x -= 10;
        }
        if (event.keyboard.keycode == ALLEGRO_KEY_RIGHT){
            player_x += 10;
        }

        /* Tiro player(APERTAR TECLA UP)*/
        bool bullet_active; // bala deveria ser desenhada?
        int bullet_y;
        int bullet_x;
        int bullet_speed = 10; // velocidade da bala
        if (event.keyboard.keycode == ALLEGRO_KEY_UP){
            bullet_y = player_y;
            bullet_x = player_x + 25;
            bullet_active = true;
            }


              /*desenhar coisas*/
//         desenhando background
            al_clear_to_color(al_map_rgb(0, 0, 0));

//          desenhando player
            al_draw_filled_rectangle(player_x, player_y, player_x + PLAYER_W, player_y + PLAYER_H, al_map_rgb(0, 255, 0));

//          desenhando bala
        if(bullet_active == true){
            al_draw_filled_circle(bullet_x, bullet_y, BULLET_H, al_map_rgb(255,255,255));
            bullet_y -= bullet_speed;
            if (bullet_y <10){
                bullet_active = false;
                bullet_y = player_y;
            }
        }

//          pontua��o
            al_draw_textf(font, al_map_rgb(255, 255, 255), 10, 10, 0, "Pontos: %d", score);
            al_draw_textf(font, al_map_rgb(255, 255, 255), 700, 10, 0, "ESC TO EXIT");
            al_flip_display();

    }

    al_destroy_font(font);
    al_destroy_display(display);
    al_destroy_timer(timer);
    al_destroy_event_queue(event_queue);

    return 0;
}
