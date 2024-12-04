#include <stdio.h>
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
    //Ponteiros ferramentas
    ALLEGRO_DISPLAY * display = NULL;
    ALLEGRO_BITMAP * icon = NULL;
    ALLEGRO_FONT * font = NULL;
    ALLEGRO_TIMER * timer = NULL;
    ALLEGRO_EVENT_QUEUE * event_queue = NULL;

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

    al_set_new_display_flags(ALLEGRO_WINDOWED);           // Bandeira para sinalizar que a exibição será no modo janela.
    display = al_create_display(SCREEN_W, SCREEN_H);      // Cria a janela do programa.
    icon = al_load_bitmap("assets/Icon/icon_window.png"); // Puxa o icon da pasta Icon.
    al_set_window_title(display, "Space Invaders");       // Adiciona o título da janela.
    al_set_display_icon(display, icon);                   // Adiciona um icon na janela.

    if (!display) {
        fprintf(stderr, "Falha ao criar display.\n");
        return -1;
    }

    font = al_create_builtin_font();                       // Adiciona textos através de uma fonte imbutida.
    timer = al_create_timer(1.0 / 60);                     // FPS é crucial para o funcionamento dos eventos.
    event_queue = al_create_event_queue();                 // Cria a lista de eventos.

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

    // Ponteiros Sprites
    ALLEGRO_BITMAP * background = NULL;
    ALLEGRO_BITMAP * nave_jogador = NULL;
    ALLEGRO_BITMAP * inimigo_linha1 = NULL;
    ALLEGRO_BITMAP * inimigo_linha2 = NULL;
    ALLEGRO_BITMAP * inimigo_linha3 = NULL;
    ALLEGRO_BITMAP * inimigo_linha4 = NULL;
    ALLEGRO_BITMAP * projetil_jogador = NULL;
    ALLEGRO_BITMAP * projetil_inimigo = NULL;
    ALLEGRO_BITMAP * fogo = NULL;

    // Definindo sprites
    background = al_load_bitmap("assets/Backgrounds/blue.png");
    nave_jogador = al_load_bitmap("assets/PNG/nave_verde.png");
    inimigo_linha1 = al_load_bitmap("assets/PNG/Enemies/inimigo_azul.png");
    inimigo_linha2 = al_load_bitmap("assets/PNG/Enemies/inimigo_azul2.png");
    inimigo_linha3 = al_load_bitmap("assets/PNG/Enemies/inimigo_cinza.png");
    inimigo_linha4 = al_load_bitmap("assets/PNG/Enemies/inimigo_cinza_2.png");
    projetil_jogador = al_load_bitmap("assets/PNG/Lasers/laserBlue01.png");
    projetil_inimigo = al_load_bitmap("assets/PNG/Lasers/laserRlue01.png");
    fogo = al_load_bitmap("assets/PNG/chama_azul.png");

    // Definindo variáveis e estruturas
    struct Bullets bullets[INDIC_PROJ];               // Vetor da estrutura Bullets.
    int bullet_speed = 10;                            // Velocidade da bala (std = 10)
    int score = 0;                                    // Pontuação
    int player_speed = 10;                            // Velocidade do player (std = 10)
    int bullet_index = 0;                             // Index inicial do vetor bullets
    int i, j;                                         // Variáveis utilizadas no For do vetor bullets
    int cooldown = 0;                                 // Variável de permição para atirar
    int cooldown_frames = 20;                         // Variável de temporização do cooldown
    float player_x = SCREEN_W / 2.0 - PLAYER_W / 2.0; // Variável de localização do jogador
    float player_y = SCREEN_H - PLAYER_H - 10;        // Variável de localização do jogador
    bool running = true;                              // Jogo deve rodar?

    // Definição de tamanho dos sprites
    int largura_navejogador = al_get_bitmap_width(nave_jogador);
    int altura_navejogador = al_get_bitmap_height(nave_jogador);
    float largura_nj = largura_navejogador * 0.75;
    float altura_nj = altura_navejogador * 0.75;

    int largura_projetil = al_get_bitmap_width(projetil_jogador);
    int altura_projetil = al_get_bitmap_height(projetil_jogador);
    float largura_pj = largura_projetil * 0.75;
    float altura_pj = altura_projetil * 0.75;

    int largura_fogo = al_get_bitmap_width(fogo);
    int altura_fogo = al_get_bitmap_height(fogo);
    float largura_fg = largura_fogo * 0.80;
    float altura_fg = altura_fogo * 0.80;

    while (running) {
        ALLEGRO_EVENT event;
        al_wait_for_event(event_queue, &event); //Espera eventos.

        //Fechar janela.
        if (event.keyboard.keycode == ALLEGRO_KEY_ESCAPE || event.type == ALLEGRO_EVENT_DISPLAY_CLOSE){ //Utilizando o Esc ou o botão de fechar.
            break;
        }

        // Controles: movimentar p/direita e p/esquerda e atirar.
        if(!(player_x < 1)){                                    // Limites para esquerda
            if (event.keyboard.keycode == ALLEGRO_KEY_LEFT){    // Se apertar seta esquerda
                player_x -= player_speed;                       // Mover player para esquerda
            }
        }
        if(!(player_x > (SCREEN_W - PLAYER_W))){                // Limites para direita
            if (event.keyboard.keycode == ALLEGRO_KEY_RIGHT){   // Se apertar seta direita
                player_x += player_speed;                       // Mover player para direita
            }
        }
        if(event.type == ALLEGRO_EVENT_KEY_DOWN){                                  // Quando pressionado...
            if (event.keyboard.keycode == ALLEGRO_KEY_SPACE && cooldown == 0){     // Tecla espaço e o cooldown zerado...
                if (bullet_index < INDIC_PROJ) {                                   // Limitador de vetor
                    bullets[bullet_index].x = player_x + 40;                       // Ponto de nascimento do projetil
                    bullets[bullet_index].y = player_y - 90;
                    bullets[bullet_index].speed = - bullet_speed;                  // Velocidade de movimentação do projétil
                    bullets[bullet_index].active = true;                           // Define o projétil como ativo
                    bullet_index++;
                }
                cooldown = cooldown_frames;
            }
        }
        if (event.type == ALLEGRO_EVENT_TIMER){           // A cada frame de tempo
            for (i = 0; i < bullet_index; i++){
                if (bullets[i].active) {                  // Se o projétil ainda estiver ativo...
                    bullets[i].y += bullets[i].speed;     // Movimenta-se.
                    if (bullets[i].y < 0) {               // Se o projétil passar do final do eixo y...
                        bullets[i].active = false;        // Torna inativo.
                    }
                }
            }
        }

        //Operação das naves inimigas, tiro inimigo, vida do jogador, matar inimigo, matar jogador

        //ATUALIZADOR DE SPRITES E TALS - DEIXE SEMPRE POR ÚLTIMO
        if (al_is_event_queue_empty(event_queue)){        // Se a lista estiver vazia - serve para atualizar os sprites e outras coisas.
            al_draw_bitmap(background, 0, 0, 0);
            al_set_target_bitmap(al_get_backbuffer(display));
            al_draw_scaled_bitmap(fogo, 0, 0, largura_fogo, altura_fogo, player_x + 35, player_y - 2, largura_fg, altura_fg, 0);
            al_draw_scaled_bitmap(nave_jogador, 0, 0, largura_navejogador, altura_navejogador, player_x, player_y - 50, largura_nj, altura_nj, 0);

            for (j = 0; j < bullet_index; j++){            // Desenha os tiros ativos
                if (bullets[j].active) {
                    al_draw_scaled_bitmap(projetil_jogador, 0, 0, largura_projetil, altura_projetil, bullets[j].x, bullets[j].y, largura_pj, altura_pj, 0);
                }
            }
        }

        if (cooldown > 0) {
            cooldown--;
        }

            //pontua��o
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
