#include <stdio.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>

#define SCREEN_W 800
#define SCREEN_H 600
#define PLAYER_W 100
#define PLAYER_H 30
#define INDIC_PROJ 200
#define ENEMY_COUNT 20

struct Bullets{
    float x, y;  // Posição do tiro
    float speed; // Velocidade do tiro
    bool active; // Se o tiro está ativo
};

struct Enemy {
    float x, y;  // Posição do inimigo
    bool active; // Se o inimigo está ativo
};

struct Bullets_Enemy {
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

    if(!al_init_image_addon()) { //Inicia as imagens.
        fprintf(stderr, "Falha ao inicializar o addon da imagem.\n");
        return -1;
    }

    if (!al_install_audio()) {
    fprintf(stderr, "Falha ao inicializar o áudio.\n");
    return -1;
}

if (!al_init_acodec_addon()) {
    fprintf(stderr, "Falha ao inicializar o addon de codec de áudio.\n");
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

    al_install_audio();
    al_init_acodec_addon();

    /////////////////////////////////////////////////////////

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
    ALLEGRO_BITMAP * projetil_jogador = NULL;
    ALLEGRO_BITMAP * projetil_inimigo = NULL;
    ALLEGRO_BITMAP * fogo = NULL;
    ALLEGRO_BITMAP * inimigo_sprite = NULL;

    // Definindo sprites
    background = al_load_bitmap("assets/Backgrounds/fundo_real.jpg");
    nave_jogador = al_load_bitmap("assets/PNG/nave_verde.png");
    projetil_jogador = al_load_bitmap("assets/PNG/laserBlue01.png");
    projetil_inimigo = al_load_bitmap("assets/PNG/laserRed01.png");
    fogo = al_load_bitmap("assets/PNG/chama_azul.png");
    inimigo_sprite = al_load_bitmap("assets/PNG/inimigo_azul.png");

    // Definindo Sons
    ALLEGRO_SAMPLE * tiro_som = NULL;
    ALLEGRO_SAMPLE * musica_fundo = NULL;
    ALLEGRO_SAMPLE_INSTANCE * songInstance = NULL;

    al_reserve_samples(10);

    tiro_som = al_load_sample("assets/Sounds/tiro_som.wav");
    musica_fundo = al_load_sample("assets/Sounds/musica_fundo.wav");
    songInstance = al_create_sample_instance(musica_fundo);
    al_set_sample_instance_playmode(songInstance, ALLEGRO_PLAYMODE_LOOP);

    al_attach_sample_instance_to_mixer(songInstance, al_get_default_mixer());


    ///////////////////////////////////////////////////////////////////////////////

    if (!background || !nave_jogador || !projetil_jogador || !projetil_inimigo || !fogo || !inimigo_sprite) {
        fprintf(stderr, "Falha ao carregar sprites.\n");
        al_destroy_display(display);
        return -1;
    }

    // Definindo variáveis e estruturas
    struct Enemy enemies[ENEMY_COUNT];                // Vetor da estrutura Enemy
    bool gerar_enemy = true;                          // Gerador de Inimigos
    float enemy_speed = 1.0;                          // Velocidade de deslocamento horizontal
    int enemy_direction = -1;                         // Direção inicial à esquerda
    int linha, coluna, enemy_index, m;                // Variáveis para geração de inimigos e para For dos inimigos
    int espacamento_x = 100;                          // Espaçamento horizontal entre inimigos
    int espacamento_y = 50;                           // Espaçamento vertical entre linhas de inimigos

    struct Bullets_Enemy enemy_bullets[INDIC_PROJ];   // Vetor da estrutura Bullets Enemy
    struct Bullets bullets[INDIC_PROJ];               // Vetor da estrutura Bullets.
    int bullet_speed = 10;                            // Velocidade da bala (std = 10)
    int bullet_enemy_speed = 3;                       // Velocidade da bala inimiga
    int score = 0;                                    // Pontuação
    int player_speed = 10;                            // Velocidade do player (std = 10)
    int bullet_index = 0;                             // Index inicial do vetor bullets
    int bullet_index_enemy = 0;                       // Index inicial do vetor bullet enemy
    int i, j;                                         // Variáveis utilizadas no For do vetor bullets
    int cooldown = 0;                                 // Variável de permição para atirar
    int cooldown_frames = 20;                         // Variável de temporização do cooldown
    float player_x = SCREEN_W / 2.0 - PLAYER_W / 2.0; // Variável de localização do jogador
    float player_y = SCREEN_H - PLAYER_H - 10;        // Variável de localização do jogador

    bool running = true;                              // Jogo deve rodar?

    // Definição de tamanho dos sprites
    int largura_background = al_get_bitmap_width(background);
    int altura_background = al_get_bitmap_height(background);
    float largura_back = largura_background * 1.1;
    float altura_back = altura_background * 1.3;

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

    int largura_inimigo = al_get_bitmap_width(inimigo_sprite);
    int altura_inimigo = al_get_bitmap_height(inimigo_sprite);
    float largura_in = largura_inimigo * 0.50;
    float altura_in = altura_inimigo * 0.50;

    while (running) {


        ALLEGRO_EVENT event;
        al_wait_for_event(event_queue, &event); //Espera eventos.

        int tocar_musica = 1;
        if (tocar_musica == 1){
        al_play_sample_instance(songInstance);
        tocar_musica = 0;
        }
        //Fechar janela utilizando o Esc ou o botão de fechar.
        if (event.keyboard.keycode == ALLEGRO_KEY_ESCAPE || event.type == ALLEGRO_EVENT_DISPLAY_CLOSE){
            al_stop_sample_instance(songInstance);
            break;
        }

        //Posicionar inimigos
        if (gerar_enemy == true) {
            for (linha = 0; linha < 4; linha++) {
                for (coluna = 0; coluna < 5; coluna++) {
                    enemy_index = linha * 5 + coluna;     // Calcula o índice no vetor enemies
                    enemies[enemy_index].x = espacamento_x + (coluna * 120); // Posição X
                    enemies[enemy_index].y = espacamento_y + (linha * 80);   // Posição Y
                    enemies[enemy_index].active = true;       // Define o inimigo como ativo
                }
            }
            gerar_enemy = false;
        }

        // Controles: movimentar p/direita, p/esquerda e atirar.
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
            if (event.keyboard.keycode == ALLEGRO_KEY_SPACE && cooldown == 0){      // Tecla espaço e o cooldown zerado...
                al_play_sample(tiro_som, 1, 0, 1, ALLEGRO_PLAYMODE_ONCE, 0);           // som do tiro
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

        // Geração de tiro aletório dos inimigos
        if (event.type == ALLEGRO_EVENT_TIMER){
            for (m = 0; m < ENEMY_COUNT; m++) {
                if (enemies[m].active) {                        // Se o inimigo estiver ativo
                    if (rand() % 1200 < 1) {                    // Probabilidade de disparo por frame
                        if (bullet_index_enemy < INDIC_PROJ) {
                            enemy_bullets[bullet_index_enemy].x = enemies[m].x + largura_in / 2;
                            enemy_bullets[bullet_index_enemy].y = enemies[m].y + altura_in;
                            enemy_bullets[bullet_index_enemy].speed = -bullet_enemy_speed;
                            enemy_bullets[bullet_index_enemy].active = true;
                            bullet_index_enemy++;
                            break;
                        } //Fim if
                    } //Fim if
                } // Fim if
            } //Fim for
        } //Fim if

        if (event.type == ALLEGRO_EVENT_TIMER){           // A cada frame de tempo realiza as seguintas verificações:
            for (m = 0; m < bullet_index_enemy; m++){
                if (enemy_bullets[m].active) {
                    enemy_bullets[m].y -= enemy_bullets[m].speed;
                    if (enemy_bullets[m].y > SCREEN_H){
                        enemy_bullets[m].active = false;
                    }
                } //Fim if
            } //Fim for
            for (i = 0; i < bullet_index; i++){
                if (bullets[i].active) {                  // Se o projétil estiver ativo, irá avançar
                    bullets[i].y += bullets[i].speed;
                    if (bullets[i].y < 0) {               // Se o projétil ultrapassar a altura da tela, irá desativar
                        bullets[i].active = false;
                    }
                    for (m = 0; m < ENEMY_COUNT; m++) {
                        if (enemies[m].active) {          // Se o inimigo e o projétil estiverem ativos, irá desativar ambos
                            if (bullets[i].x < enemies[m].x + largura_in && bullets[i].x + largura_pj > enemies[m].x &&
                                bullets[i].y < enemies[m].y + altura_in && bullets[i].y + largura_pj > enemies[m].y) {
                                bullets[i].active = false;
                                enemies[m].active = false;
                                score += 50;
                            } //Fim if
                        } //Fim if
                    } //Fim for
                } //Fim if
            } //Fim for
        } //Fim if

        // A cada frame, movimenta-se os inimigos
        if (event.type == ALLEGRO_EVENT_TIMER){
            for (m = 0; m < ENEMY_COUNT; m++) {
                enemies[m].x += enemy_speed * enemy_direction;
                enemies[m].y += 0.01;
            }
        } // Fim if

        // Verifica os limites da tela para alterar a direção de movimentação dos inimigos
        if (enemies[0].x <= 0 || enemies[ENEMY_COUNT - 1].x + largura_in >= SCREEN_W) {
            enemy_direction *= -1;
        }

        if (al_is_event_queue_empty(event_queue)){        // Se a lista estiver vazia, irá atualizar os sprites
            al_draw_scaled_bitmap(background, 0, 0, largura_background, altura_background,0, 0, largura_back, altura_back, 0);
            al_set_target_bitmap(al_get_backbuffer(display));
            al_draw_scaled_bitmap(fogo, 0, 0, largura_fogo, altura_fogo, player_x + 35, player_y - 2, largura_fg, altura_fg, 0);
            al_draw_scaled_bitmap(nave_jogador, 0, 0, largura_navejogador, altura_navejogador, player_x, player_y - 50, largura_nj, altura_nj, 0);

            for (j = 0; j < bullet_index; j++){           // Desenha os projéteis ativos do jogador
                if (bullets[j].active) {
                    al_draw_scaled_bitmap(projetil_jogador, 0, 0, largura_projetil, altura_projetil, bullets[j].x, bullets[j].y, largura_pj, altura_pj, 0);
                }
            } // Fim for
            for (m = 0; m < ENEMY_COUNT; m++) {           // Desenha os inimigos ativos
                if (enemies[m].active) {
                    al_draw_scaled_bitmap(inimigo_sprite, 0, 0, largura_inimigo, altura_inimigo, enemies[m].x, enemies[m].y, largura_in, altura_in, 0);
                }
            } //Fim for
            for (m = 0; m < bullet_index_enemy; m++){           // Desenha os projéteis ativos dos inimigos
                if (enemy_bullets[m].active) {
                    al_draw_scaled_bitmap(projetil_inimigo, 0, 0, largura_projetil, altura_projetil, enemy_bullets[m].x, enemy_bullets[m].y, largura_pj, altura_pj, 0);
                }
            } // Fim for
        } //Fim if

        // Cooldown para atirar
        if (cooldown > 0) {
            cooldown--;
        }

        al_draw_textf(font, al_map_rgb(255, 255, 255), 10, 10, 0, "Pontos: %d", score); // Imprime os pontos na tela e o aviso
        al_draw_textf(font, al_map_rgb(255, 255, 255), 700, 10, 0, "ESC TO EXIT");
        al_flip_display();


    } //Fim While

    //Fechadores do Ambiente.
    al_destroy_bitmap(background);
    al_destroy_bitmap(nave_jogador);
    al_destroy_bitmap(inimigo_sprite);
    al_destroy_bitmap(projetil_jogador);
    al_destroy_bitmap(projetil_inimigo);
    al_destroy_bitmap(icon);
    al_destroy_font(font);
    al_destroy_display(display);
    al_destroy_timer(timer);
    al_destroy_event_queue(event_queue);
    al_destroy_sample(tiro_som);
    al_destroy_sample(musica_fundo);
    al_destroy_sample_instance(songInstance);


    return 0;
}
