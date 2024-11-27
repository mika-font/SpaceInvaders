#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <stdio.h>
#include <stdbool.h>

#define SCREEN_W 800
#define SCREEN_H 600
#define PLAYER_W 50
#define PLAYER_H 30
#define BULLET_W 5
#define BULLET_H 10
#define ALIEN_W 40
#define ALIEN_H 30
#define NUM_ALIENS 5
#define NUM_BULLETS 10

typedef struct {
    float x, y;
    bool active;


} Bullet;

typedef struct {
    float x, y;
    bool active;
} Alien;

void init_aliens(Alien aliens[], int num_aliens) {
    for (int i = 0; i < num_aliens; i++) {
        aliens[i].x = 100 + i * 100;
        aliens[i].y = 50;
        aliens[i].active = true;
    }
}

void draw_aliens(Alien aliens[], int num_aliens) {
    for (int i = 0; i < num_aliens; i++) {
        if (aliens[i].active)
            al_draw_filled_rectangle(aliens[i].x, aliens[i].y, aliens[i].x + ALIEN_W, aliens[i].y + ALIEN_H, al_map_rgb(255, 0, 0));
    }
}

void move_aliens(Alien aliens[], int num_aliens, float speed) {
    for (int i = 0; i < num_aliens; i++) {
        aliens[i].y += speed;
        if (aliens[i].y > SCREEN_H) {
            aliens[i].y = 50;
        }
    }
}

void shoot_bullet(Bullet bullets[], int size, float x, float y) {
    for (int i = 0; i < size; i++) {
        if (!bullets[i].active) {
            bullets[i].x = x;
            bullets[i].y = y;
            bullets[i].active = true;
            break;
        }
    }
}

void update_bullets(Bullet bullets[], int size) {
    for (int i = 0; i < size; i++) {
        if (bullets[i].active) {
            bullets[i].y -= 5;
            if (bullets[i].y < 0)
                bullets[i].active = false;
        }
    }
}

void draw_bullets(Bullet bullets[], int size) {
    for (int i = 0; i < size; i++) {
        if (bullets[i].active)
            al_draw_filled_rectangle(bullets[i].x, bullets[i].y, bullets[i].x + BULLET_W, bullets[i].y + BULLET_H, al_map_rgb(255, 255, 255));
    }
}

bool check_collision(float x1, float y1, float w1, float h1, float x2, float y2, float w2, float h2) {
    return (x1 < x2 + w2 && x1 + w1 > x2 && y1 < y2 + h2 && y1 + h1 > y2);
}

// Função para verificar se o jogador venceu
bool check_victory(Alien aliens[], int num_aliens) {
    for (int i = 0; i < num_aliens; i++) {
        if (aliens[i].active) {
            return false; // Ainda há alienígenas ativos
        }
    }
    return true; // Todos os alienígenas foram destruídos
}

// Função para exibir mensagem de vitória
void display_victory_message(ALLEGRO_FONT *font) {
    al_clear_to_color(al_map_rgb(0, 0, 0));
    al_draw_text(font, al_map_rgb(255, 255, 255), SCREEN_W / 2, SCREEN_H / 2 - 20, ALLEGRO_ALIGN_CENTRE, "Parabéns! Você venceu!");
    al_flip_display();
    al_rest(3.0); // Mostra a mensagem por 3 segundos
}

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

    if (!al_init_font_addon() || !al_init_ttf_addon()) {
        fprintf(stderr, "Falha ao inicializar os addons de fontes.\n");
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

    bool running = true;
    bool redraw = true;
    float player_x = SCREEN_W / 2.0 - PLAYER_W / 2.0;
    float player_y = SCREEN_H - PLAYER_H - 10;
    int score = 0;

    Bullet bullets[NUM_BULLETS] = {0};
    Alien aliens[NUM_ALIENS];
    init_aliens(aliens, NUM_ALIENS);

    al_start_timer(timer);

    while (running) {
        ALLEGRO_EVENT event;
        al_wait_for_event(event_queue, &event);

        if (event.type == ALLEGRO_EVENT_TIMER) {
            redraw = true;

            update_bullets(bullets, NUM_BULLETS);
            move_aliens(aliens, NUM_ALIENS, 0.5);

            for (int i = 0; i < NUM_BULLETS; i++) {
                if (bullets[i].active) {
                    for (int j = 0; j < NUM_ALIENS; j++) {
                        if (aliens[j].active && check_collision(bullets[i].x, bullets[i].y, BULLET_W, BULLET_H, aliens[j].x, aliens[j].y, ALIEN_W, ALIEN_H)) {
                            bullets[i].active = false;
                            aliens[j].active = false;
                            score += 10;
                        }
                    }
                }
            }

            if (check_victory(aliens, NUM_ALIENS)) {
                display_victory_message(font);
                running = false;
            }

        } else if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            running = false;
        } else if (event.type == ALLEGRO_EVENT_KEY_DOWN) {
            if (event.keyboard.keycode == ALLEGRO_KEY_ESCAPE)
                running = false;
            if (event.keyboard.keycode == ALLEGRO_KEY_SPACE)
                shoot_bullet(bullets, NUM_BULLETS, player_x + PLAYER_W / 2.0 - BULLET_W / 2.0, player_y);
        } else if (event.type == ALLEGRO_EVENT_KEY_CHAR) {
            if (event.keyboard.keycode == ALLEGRO_KEY_LEFT && player_x > 0)
                player_x -= 5;
            if (event.keyboard.keycode == ALLEGRO_KEY_RIGHT && player_x < SCREEN_W - PLAYER_W)
                player_x += 5;
        }

        if (redraw && al_is_event_queue_empty(event_queue)) {
            redraw = false;

            al_clear_to_color(al_map_rgb(0, 0, 0));
            al_draw_filled_rectangle(player_x, player_y, player_x + PLAYER_W, player_y + PLAYER_H, al_map_rgb(0, 255, 0));
            draw_bullets(bullets, NUM_BULLETS);
            draw_aliens(aliens, NUM_ALIENS);
            al_draw_textf(font, al_map_rgb(255, 255, 255), 10, 10, 0, "Pontuação: %d", score);
            al_flip_display();
        }
    }

    al_destroy_font(font);
    al_destroy_display(display);
    al_destroy_timer(timer);
    al_destroy_event_queue(event_queue);

    return 0;
}
#include <stdio.h>
#include <stdlib.h>

int main()
{
    printf("Vamos criar o jogo!");
    return 0;
}
