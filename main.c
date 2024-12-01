#include <stdio.h>
#include <stdbool.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>

#define SCREEN_W 800
#define SCREEN_H 600
#define PLAYER_W 100
#define PLAYER_H 30
#define BULLET_W 10
#define ENEMY_W 40
#define ENEMY_H 30
#define INDIC_PROJ 200

struct Bullets{
    float x, y;  // Posição do tiro
    float speed; // Velocidade do tiro
    bool active; // Se o tiro está ativo
};

int main(int argc, char **argv) {
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

    al_set_new_display_flags(ALLEGRO_WINDOWED); //Bandeira para sinalizar que a exibição será no modo janela.
    ALLEGRO_DISPLAY * display = al_create_display(SCREEN_W, SCREEN_H); //Cria a janela do programa.
    ALLEGRO_BITMAP * icon = al_load_bitmap("assets/Icon/icon_window.png"); //Puxa o icon da pasta Icon.
    al_set_window_title(display, "Space Invaders"); //Adiciona o título da janela.
    al_set_display_icon(display, icon); //Adiciona um icon na janela.

    if (!display) {
        fprintf(stderr, "Falha ao criar display.\n");
        return -1;
    }

    ALLEGRO_FONT * font = al_create_builtin_font(); //Adiciona textos através de uma fonte imbutida.
    ALLEGRO_TIMER * timer = al_create_timer(1.0 / 60); //FPS é crucial para o funcionamento dos eventos.
    ALLEGRO_EVENT_QUEUE * event_queue = al_create_event_queue(); //Cria a lista de eventos.

    if (!event_queue || !timer || !font) {
        fprintf(stderr, "Falha ao criar recursos principais.\n");
        al_destroy_display(display);
        return -1;
    }
    //Registra fontes de eventos em filas de eventos específicas e inicia o timer.
    al_register_event_source(event_queue, al_get_display_event_source(display));
    al_register_event_source(event_queue, al_get_timer_event_source(timer));
    al_register_event_source(event_queue, al_get_keyboard_event_source());
    al_start_timer(timer);

    //Definindo sprites
    ALLEGRO_BITMAP * background = al_load_bitmap("assets/Backgrounds/blue.png");
    ALLEGRO_BITMAP * nave_jogador = al_load_bitmap("assets/PNG/nave_verde.png");
    ALLEGRO_BITMAP * inimigo_linha1 = al_load_bitmap("assets/PNG/Enemies/inimigo_azul.png");
    ALLEGRO_BITMAP * inimigo_linha2 = al_load_bitmap("assets/PNG/Enemies/inimigo_azul2.png");
    ALLEGRO_BITMAP * inimigo_linha3 = al_load_bitmap("assets/PNG/Enemies/inimigo_cinza.png");
    ALLEGRO_BITMAP * inimigo_linha4 = al_load_bitmap("assets/PNG/Enemies/inimigo_cinza_2.png");
    ALLEGRO_BITMAP * projetil_jogador = al_load_bitmap("assets/PNG/Lasers/laserBlue01.png");
    ALLEGRO_BITMAP * projetil_inimigo = al_load_bitmap("assets/PNG/Lasers/laserRlue01.png");

    //Definindo variáveis
    struct Bullets bullets[INDIC_PROJ];
    int bullet_speed = 10; // Velocidade da bala (std = 10)
    int score = 0; //Pontuação
    int player_speed = 10; // Velocidade do player (std = 10)
    int bullet_index = 0;
    int i, j;
    int cooldown_frames = 60; // 200ms (12 frames a 60 FPS)
    int cooldown_timer = 0;

    //debugs
    bool enemy_alive = true; // Inimigo deve ser desenhado?
    bool running = true; // jogo deve rodar?
    bool redraw = true;
    float enemy_x = SCREEN_W /2.0 - ENEMY_W / 2.0;
    float enemy_y = 10;
    float player_x = SCREEN_W / 2.0 - PLAYER_W / 2.0;
    float player_y = SCREEN_H - PLAYER_H - 10;

    while (running) {
        ALLEGRO_EVENT event;
        al_wait_for_event(event_queue, &event); //Espera eventos.

        //Fechar janela.
        if (event.keyboard.keycode == ALLEGRO_KEY_ESCAPE){ //Utilizando o Esc.
            running = false;
        }
        if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE){ //Utilizando o botão da janela.
            break;
        }

        //Desenhando plano de fundo e jogador
        al_draw_bitmap(background, 0, 0, 0);
        al_set_target_bitmap(al_get_backbuffer(display));
        al_draw_bitmap(nave_jogador, player_x, player_y - 40, 0);

        // Movimento do player (letf - right)//
        if(!(player_x < 0)){ // limites para esquerda
            if (event.keyboard.keycode == ALLEGRO_KEY_LEFT){ //se apertar seta esquerda
                player_x -= player_speed; //mover player para esquerda
            }
        }
        if(!(player_x > (SCREEN_W - PLAYER_W))){ // limites para direita
            if (event.keyboard.keycode == ALLEGRO_KEY_RIGHT){ // se apertar seta direita
                player_x += player_speed; // mover player para direita
            }
        }

        /* Tiro player(APERTAR TECLA SPACE)*/
        if (event.keyboard.keycode == ALLEGRO_KEY_SPACE){ // se apertar espaco
            if (bullet_index < INDIC_PROJ) {
                bullets[bullet_index].x = player_x + 50;
                bullets[bullet_index].y = player_y - 100;
                bullets[bullet_index].speed = - bullet_speed; // Velocidade para cima
                bullets[bullet_index].active = true;
                bullet_index++;
            }
        }

        if (event.type == ALLEGRO_EVENT_TIMER) {
            for (i = 0; i < bullet_index; i++) {
                if (bullets[i].active) {
                    bullets[i].y += bullets[i].speed;
                    // Desativa o tiro se sair da tela
                    if (bullets[i].y < 0) {
                        bullets[i].active = false;
                    }
                }
            }
        }

        if (al_is_event_queue_empty(event_queue)) {
            // Desenha os tiros ativos
            for (j = 0; j < bullet_index; j++) {
                if (bullets[j].active) {
                    al_draw_bitmap(projetil_jogador, bullets[j].x, bullets[j].y, 0);
                }
            }
        }

        /*if(bullet_active == true){ // se a bala estiver ativa
            al_draw_bitmap(projetil_jogador, bullet_x, bullet_y - 40, 0); //desenhe ela na posição de bala
            bullet_y -= bullet_speed; // mude a posição y dela conforme a velocidade
            if (bullet_y <10){ //se a bala chegar no top da tela
                al_destroy_bitmap(projetil_jogador);
                bullet_active = false; //desativa
                bullet_y = player_y; //e reinicie a altura da bala
            }
        }*/

//          matar nave inimiga

            //if((((bullet_x < (enemy_x + ENEMY_W)) && (bullet_x > enemy_x)) && bullet_y <= enemy_y) && bullet_active == true){ // se a bala entrar no perimetro do inimigo e estiver ativa
               // al_draw_filled_rectangle(0,0,10,10,al_map_rgb(255,255,255));
                //enemy_alive = false; //mata o inimigo

            //}

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
    al_destroy_bitmap(background);
    al_destroy_bitmap(nave_jogador);
    al_destroy_bitmap(inimigo_linha1);
    al_destroy_bitmap(inimigo_linha2);
    al_destroy_bitmap(inimigo_linha3);
    al_destroy_bitmap(inimigo_linha4);
    al_destroy_bitmap(projetil_jogador);
    al_destroy_bitmap(projetil_inimigo);
    al_destroy_bitmap(icon);
    al_destroy_font(font);
    al_destroy_display(display);
    al_destroy_timer(timer);
    al_destroy_event_queue(event_queue);

    return 0;
}
