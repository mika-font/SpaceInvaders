#include <stdio.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>

#define SCREEN_W 900
#define SCREEN_H 700
#define PLAYER_W 100
#define PLAYER_H 30
#define BULLET_W 10
#define ENEMY_W 40
#define ENEMY_H 30
#define INDIC_PROJ 200
#define ENEMY_COUNT 20

struct Bullets {
    float x, y;  // Posição do tiro
    float speed; // Velocidade do tiro
    bool active; // Se o tiro está ativo
};

struct Enemy {
    float x, y;  // Posição do inimigo
    bool active; // Se o inimigo está ativo
};

int main(int argc, char **argv) {
    // Ponteiros ferramentas
    ALLEGRO_DISPLAY *display = NULL;
    ALLEGRO_BITMAP *icon = NULL;
    ALLEGRO_FONT *font = NULL;
    ALLEGRO_TIMER *timer = NULL;
    ALLEGRO_EVENT_QUEUE *event_queue = NULL;

    // Inicializadores do ambiente
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

    if (!al_init_image_addon()) {
        fprintf(stderr, "Falha ao inicializar o addon da imagem.\n");
        return -1;
    }

    al_set_new_display_flags(ALLEGRO_WINDOWED);
    display = al_create_display(SCREEN_W, SCREEN_H);
    if (!display) {
        fprintf(stderr, "Falha ao criar display.\n");
        return -1;
    }

    icon = al_load_bitmap("assets/Icon/icon_window.png");
    if (!icon) {
        fprintf(stderr, "Falha ao carregar o ícone.\n");
        al_destroy_display(display);
        return -1;
    }
    al_set_display_icon(display, icon);
    al_set_window_title(display, "Space Invaders");

    font = al_create_builtin_font();
    timer = al_create_timer(1.0 / 60);
    event_queue = al_create_event_queue();
    if (!event_queue || !timer || !font) {
        fprintf(stderr, "Falha ao criar recursos principais.\n");
        al_destroy_display(display);
        al_destroy_bitmap(icon);
        return -1;
    }

    al_register_event_source(event_queue, al_get_display_event_source(display));
    al_register_event_source(event_queue, al_get_timer_event_source(timer));
    al_register_event_source(event_queue, al_get_keyboard_event_source());
    al_start_timer(timer);

    // Ponteiros Sprites
    ALLEGRO_BITMAP *background = NULL;
    ALLEGRO_BITMAP *nave_jogador = NULL;
    ALLEGRO_BITMAP *projetil_jogador = NULL;
    ALLEGRO_BITMAP *fogo = NULL;
    ALLEGRO_BITMAP *inimigo_sprite = NULL;

    // Carregar sprites
    background = al_load_bitmap("assets/Backgrounds/war.png");
    nave_jogador = al_load_bitmap("assets/PNG/nave_verde.png");
    projetil_jogador = al_load_bitmap("assets/PNG/Lasers/laserBlue01.png");
    fogo = al_load_bitmap("assets/PNG/chama_azul.png");
    inimigo_sprite = al_load_bitmap("assets/PNG/Enemies/inimigo_azul.png");

    if (!background || !nave_jogador || !projetil_jogador || !fogo || !inimigo_sprite) {
        fprintf(stderr, "Falha ao carregar sprites.\n");
        al_destroy_display(display);
        return -1;
    }

    // Definindo variáveis e estruturas
    struct Bullets bullets[INDIC_PROJ] = {0};
    struct Enemy enemies[ENEMY_COUNT] = {0};

    int bullet_speed = 10;
    int score = 0;
    int player_speed = 10;
    int bullet_index = 0;
    int cooldown = 0;
    int cooldown_frames = 20;
    float player_x = SCREEN_W / 2.0 - PLAYER_W / 2.0;
    float player_y = SCREEN_H - PLAYER_H - 10;
    bool running = true;

    bool key_left = false, key_right = false;

    // Configurar inimigos
    float enemy_speed = 1.0;
    int enemy_direction = -1; // Direção inicial (esquerda)
    

    for (int i = 0; i < ENEMY_COUNT; i++) {
        
    for (int contador = 0; contador < 5; contador++){

        int posição = i * 5 + contador;
        enemies[posição].x = 100 + contador * 100;
        enemies[posição].y = i * 100 + 50;  

    }
        enemies[i].active = true;
       
    }
       

    while (running) {
        ALLEGRO_EVENT event;
        al_wait_for_event(event_queue, &event);

        if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            running = false;
        }

        if (event.type == ALLEGRO_EVENT_KEY_DOWN) {
            if (event.keyboard.keycode == ALLEGRO_KEY_ESCAPE) {
                running = false;
            }
            if (event.keyboard.keycode == ALLEGRO_KEY_LEFT) {
                key_left = true;
            }
            if (event.keyboard.keycode == ALLEGRO_KEY_RIGHT) {
                key_right = true;
            }
            if (event.keyboard.keycode == ALLEGRO_KEY_SPACE && cooldown == 0) {
                bullets[bullet_index].x = player_x + 40;
                bullets[bullet_index].y = player_y - 90;
                bullets[bullet_index].speed = -bullet_speed;
                bullets[bullet_index].active = true;

                bullet_index = (bullet_index + 1) % INDIC_PROJ;
                cooldown = cooldown_frames;
            }
        }
        if (event.type == ALLEGRO_EVENT_KEY_UP) {
            if (event.keyboard.keycode == ALLEGRO_KEY_LEFT) {
                key_left = false;
            }
            if (event.keyboard.keycode == ALLEGRO_KEY_RIGHT) {
                key_right = false;
            }
        }

        if (event.type == ALLEGRO_EVENT_TIMER) {
            if (key_left && player_x > 0) {
                player_x -= player_speed;
            }
            if (key_right && player_x < SCREEN_W - PLAYER_W) {
                player_x += player_speed;
            }

            for (int i = 0; i < INDIC_PROJ; i++) {
                if (bullets[i].active) {
                    bullets[i].y += bullets[i].speed;
                    if (bullets[i].y < 0) {
                        bullets[i].active = false;
                    }
                }
            }

            // Mover inimigos
            for (int i = 0; i < ENEMY_COUNT; i++) {
                enemies[i].x += enemy_speed * enemy_direction;
                enemies[i].y += 0.01; 
            }

            // Verificar limites para alterar a direção
            if (enemies[0].x <= 0 || enemies[ENEMY_COUNT - 1].x + ENEMY_W >= SCREEN_W) {
                enemy_direction *= -1;
            }
        }

        if (al_is_event_queue_empty(event_queue)) {
            al_draw_bitmap(background, 0, 0, 0);
            al_draw_scaled_bitmap(fogo, 0, 0, al_get_bitmap_width(fogo), al_get_bitmap_height(fogo),
                                  player_x + 35, player_y - 2, 20, 20, 0);
            al_draw_scaled_bitmap(nave_jogador, 0, 0, al_get_bitmap_width(nave_jogador),
                                  al_get_bitmap_height(nave_jogador), player_x, player_y, 75, 75, 0);

            for (int j = 0; j < INDIC_PROJ; j++) {
                if (bullets[j].active) {
                    al_draw_scaled_bitmap(projetil_jogador, 0, 0, al_get_bitmap_width(projetil_jogador),
                                          al_get_bitmap_height(projetil_jogador), bullets[j].x, bullets[j].y, 10, 10, 0);
                }

                if (bullets[j].x > 100 && bullets[j].x < enemies[0].x ){

                    if (bullets[j].y > 0 && bullets[j].y < 50){


                        enemies[0].active = false;
                    }
                }
 
            }

            for (int i = 0; i < ENEMY_COUNT; i++) {
                if (enemies[i].active) {
                    al_draw_scaled_bitmap(inimigo_sprite, 0, 0, al_get_bitmap_width(inimigo_sprite),
                                          al_get_bitmap_height(inimigo_sprite), enemies[i].x, enemies[i].y, 50, 50, 0);
                }
   
                
            }


            if (enemies[0].active == false && score == 0){

                score += 100;
            
            }

            
            al_draw_textf(font, al_map_rgb(255, 255, 255), 10, 10, 0, "Pontos: %d", score);
            al_draw_textf(font, al_map_rgb(255, 255, 255), 700, 10, 0, "ESC TO EXIT");
            al_flip_display();
        }

        if (cooldown > 0) {
            cooldown--;
        }
    }

    al_destroy_bitmap(background);
    al_destroy_bitmap(nave_jogador);
    al_destroy_bitmap(projetil_jogador);
    al_destroy_bitmap(fogo);
    al_destroy_bitmap(inimigo_sprite);
    al_destroy_bitmap(icon);
    al_destroy_font(font);
    al_destroy_display(display);
    al_destroy_timer(timer);
    al_destroy_event_queue(event_queue);

    return 0;
}
