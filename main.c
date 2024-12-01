#include <stdio.h>
#include <stdbool.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>

#define SCREEN_W 800
#define SCREEN_H 600
#define PLAYER_W 50
#define PLAYER_H 30
#define BULLET_W 10
#define ENEMY_W 40
#define ENEMY_H 30

int main() {
    //Iniciailizadores do Ambiente.
    if (!al_init()) { //Inicia o Allegro.
        fprintf(stderr, "Falha ao inicializar Allegro.\n");
        return -1;
    }

    if (!al_install_keyboard()) { //Inicia o teclado para receber entrada.
        fprintf(stderr, "Falha ao inicializar o teclado.\n");
        return -1;
    }

    if (!al_init_primitives_addon()) { //Inicia as primitivas.
        fprintf(stderr, "Falha ao inicializar o addon de primitivas.\n");
        return -1;
    }

    if(!al_init_image_addon()) { //Inicia as imagens.
        fprintf(stderr, "Falha ao inicializar o addon da imagem.\n");
        return -1;
    }

    int title = "Space Invaders";
    al_set_new_display_flags(ALLEGRO_WINDOWED); //Bandeira para sinalizar que a exibição será no modo janela.
    ALLEGRO_DISPLAY * display = al_create_display(SCREEN_W, SCREEN_H); //Cria a janela do programa.
    al_set_window_title(display, title); //Adiciona o título da janela.
    if (!display) {
        fprintf(stderr, "Falha ao criar display.\n");
        return -1;
    }

    ALLEGRO_FONT * font = al_create_builtin_font(); //Adiciona textos através de uma fonte imbutida.
    ALLEGRO_TIMER * timer = al_create_timer(1.0 / 60); //FPS e crucial para o funcionamento dos eventos.
    ALLEGRO_EVENT_QUEUE * event_queue = al_create_event_queue(); //Cria a lista de eventos.

    if (!event_queue || !timer || !font) {
        fprintf(stderr, "Falha ao criar recursos principais.\n");
        al_destroy_display(display);
        return -1;
    }

    al_register_event_source(event_queue, al_get_display_event_source(display)); //Recolhe a fonte do evento e adiciona a lista de eventos.
    al_register_event_source(event_queue, al_get_timer_event_source(timer));
    al_register_event_source(event_queue, al_get_keyboard_event_source());


    //definindo fun��es primarias
    int bullet_speed = 10; // velocidade da bala (std = 10)
    int score = 0; //score
    int player_speed = 10; //velocidade do player (std = 10)

    //debugs
    bool enemy_alive = true; // Inimigo deve ser desenhado?
    bool running = true; // jogo deve rodar?
    bool redraw = true;
    float enemy_x = SCREEN_W /2.0 - ENEMY_W / 2.0;
    float enemy_y = 10;
    float player_x = SCREEN_W / 2.0 - PLAYER_W / 2.0;
    float player_y = SCREEN_H - PLAYER_H - 10;
    int bullet_y;
    int bullet_x;
    bool bullet_active; // bala deveria ser desenhada?

    al_start_timer(timer);

    while (running) {
        ALLEGRO_EVENT event;
        al_wait_for_event(event_queue, &event);

        /* inputs */

         /*fechar jogo ao apertar esc*/
        if (event.keyboard.keycode == ALLEGRO_KEY_ESCAPE){ //se apertar esc
            running = false; // jogo para de rodar
        }

        /* movimentando player (letf - right)*/
        if(!(player_x < 0)){ // limites para esquerda
        if (event.keyboard.keycode == ALLEGRO_KEY_LEFT){ //se apertar seta esquerda
            player_x -= player_speed; //mover player para esquerda
        }
        }
        if((!(player_x > SCREEN_W - PLAYER_W))){ // limites para direita
        if (event.keyboard.keycode == ALLEGRO_KEY_RIGHT){ // se apertar seta direita
            player_x += player_speed; // mover player para direita
        }
        }

        /* Tiro player(APERTAR TECLA UP)*/
        if (event.keyboard.keycode == ALLEGRO_KEY_UP){ // se apertar para cima
            bullet_y = player_y;
            bullet_x = player_x + 25;
            bullet_active = true; //ativa a bala
            }

              /*desenhar coisas*/
//         desenhando background
            al_clear_to_color(al_map_rgb(0, 0, 0)); //background preto

//          desenhando bala
        if(bullet_active == true){ // se a bala estiver ativa
            al_draw_filled_circle(bullet_x, bullet_y, BULLET_W, al_map_rgb(255,255,255)); //desenhe ela na posição de bala
            bullet_y -= bullet_speed; // mude a posição y dela conforme a velocidade
            if (bullet_y <10){ //se a bala chegar no top da tela
                bullet_active = false; //desative ela
                bullet_y = player_y; //e reinicie a altura da bala
            }
        }

//          desenhando player
            al_draw_filled_rectangle(player_x, player_y, player_x + PLAYER_W, player_y + PLAYER_H, al_map_rgb(0, 255, 0)); //desenhe o player na posição correta


//          matar nave inimiga

            if((((bullet_x < (enemy_x + ENEMY_W)) && (bullet_x > enemy_x)) && bullet_y <= enemy_y) && bullet_active == true){ // se a bala entrar no perimetro do inimigo e estiver ativa
               // al_draw_filled_rectangle(0,0,10,10,al_map_rgb(255,255,255));
                enemy_alive = false; //mata o inimigo

            }

//          desenhando inimigo
        if(enemy_alive == true){ //se o inimigo estiver vivo

            al_draw_filled_rectangle(enemy_x, enemy_y, enemy_x + ENEMY_W, enemy_y + ENEMY_H, al_map_rgb(0,0,255)); //desenha o inimigo
        }




//          pontua��o
            al_draw_textf(font, al_map_rgb(255, 255, 255), 10, 10, 0, "Pontos: %d", score);
            al_draw_textf(font, al_map_rgb(255, 255, 255), 700, 10, 0, "ESC TO EXIT");
            al_flip_display();

    }
    //Fechadores do Ambiente.
    al_destroy_font(font);
    al_destroy_display(display); //Destrói a janela de exibição.
    al_destroy_timer(timer);
    al_destroy_event_queue(event_queue);

    return 0;
}
