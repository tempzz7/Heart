#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include "screen.h"
#include "keyboard.h"
#include "timer.h"

#define AREA_INICIO_X 10
#define AREA_FIM_X 70
#define AREA_INICIO_Y 15
#define AREA_FIM_Y 25

int menuOpcao = 0;
int jogoEmExecucao = 1;
int coracaoX = AREA_INICIO_X + 3;
int coracaoY = AREA_FIM_Y - 2;
int health = 200;
int bossAtaqueCooldown = 0;
int score = 0;
int faseAtual = 1;
int velocidadeObstaculos = 20;     
int velocidadeProjeteis = 5;        
int velocidadeBossAtirar = 20;    
Mix_Music *bgMusicFase1;
Mix_Music *bgMusicFase2;



// Estruturas para obstáculos e projéteis
typedef struct {
    int x, y;
    int active;
    int comprimento;
} ObstaculoOsso;

typedef struct {
    int x, y;
    int active;
} Projeteis;

typedef struct {
    int x, y;
    int active;
    int direcao; // 1 para baixo, -1 para cima
} ObstaculoVertical;

#define MAX_OBSTACULOS_VERTICAIS 5
ObstaculoVertical obstaculosVerticais[MAX_OBSTACULOS_VERTICAIS];

#define MAX_OBSTACULOS 10
#define MAX_PROJETEIS 5

ObstaculoOsso ossos[MAX_OBSTACULOS];
Projeteis projeteis[MAX_PROJETEIS];
int contadorObstaculos = 0;

// Declarações de funções
void animarCoracao();
void mostrarMenuPrincipal();
void mostrarCreditos();
void desenharAreaComCoracao();
void desenharOssos();
void atualizarOssos();
void iniciarJogo(Mix_Music *bgMusic);
void finalizarJogo(int pontuacaoAtual);
void moverCoracao(int upPressed, int downPressed, int leftPressed, int rightPressed);
void gerarObstaculos();
int detectarColisao();
void atualizarStatus();
void desenharPersonagemASCII();
void limparAreaJogo();
void bossAtirar();
void atualizarProjeteis();
int detectarColisaoProjeteis();
void atualizarObstaculosVerticais();  // Adicione esta linha
void mudarParaSegundaFase();          // E esta linha

// Função para ler a maior pontuação do arquivo
int lerMaiorPontuacao() {
    int maiorPontuacao = 0;
    FILE *file = fopen("score.dat", "r");
    if (file != NULL) {
        fscanf(file, "%d", &maiorPontuacao);
        fclose(file);
    }
    return maiorPontuacao;
}

// Função para salvar a maior pontuação no arquivo
void salvarMaiorPontuacao(int pontuacaoAtual) {
    int maiorPontuacao = lerMaiorPontuacao();
    if (pontuacaoAtual > maiorPontuacao) {
        FILE *file = fopen("score.dat", "w");
        if (file != NULL) {
            fprintf(file, "%d", pontuacaoAtual);
            fclose(file);
        }
    }
}

// Função para exibir a maior pontuação na tela inicial
void mostrarMaiorPontuacao() {
    int maiorPontuacao = lerMaiorPontuacao();
    screenSetColor(LIGHTMAGENTA, DARKGRAY);
    screenGotoxy(33, 26); // Ajuste para exibir abaixo das opções do menu
    printf("Maior Pontuação: %d", maiorPontuacao);
}

void mostrarMenuPrincipal() {
    screenSetColor(RED, DARKGRAY);
    screenClear();
    animarCoracao();

    screenGotoxy(20, 12);
    printf("██░ ██ ▓█████ ▄▄▄       ██▀███  ▄▄▄█████▓    ▐██▌ ");
    screenGotoxy(20, 13);
    printf("▓██░ ██▒▓█   ▀▒████▄    ▓██ ▒ ██▒▓  ██▒ ▓▒    ▐██▌ ");
    screenGotoxy(20, 14);
    printf("▒██▀▀██░▒███  ▒██  ▀█▄  ▓██ ░▄█ ▒▒ ▓██░ ▒░    ▐██▌ ");
    screenGotoxy(20, 15);
    printf("░▓█ ░██ ▒▓█  ▄░██▄▄▄▄██ ▒██▀▀█▄  ░ ▓██▓ ░     ▓██▒ ");
    screenGotoxy(20, 16);
    printf("░▓█▒░██▓░▒████▒▓█   ▓██▒░██▓ ▒██▒  ▒██▒ ░     ▒▄▄ ");
    screenGotoxy(20, 17);
    printf(" ▒ ░░▒░▒░░ ▒░ ░▒▒   ▓▒█░░ ▒▓ ░▒▓░  ▒ ░░       ░▀▀▒ ");
    screenGotoxy(20, 18);
    printf(" ▒ ░▒░ ░ ░ ░  ░ ▒   ▒▒ ░  ░▒ ░ ▒░    ░        ░  ░ ");
    screenGotoxy(20, 19);
    printf(" ░  ░░ ░   ░    ░   ▒     ░░   ░   ░             ░ ");
    screenGotoxy(20, 20);
    printf(" ░  ░  ░   ░  ░     ░  ░   ░                  ░ ");

    screenSetColor(LIGHTMAGENTA, DARKGRAY);
    screenGotoxy(33, 22);
    printf("1. Iniciar");
    screenGotoxy(33, 23);
    printf("2. Créditos");
    screenGotoxy(33, 24);
    printf("3. Sair");
    screenGotoxy(33, 25); // Ajuste da posição para que o texto de maior pontuação não sobreponha as opções.
    mostrarMaiorPontuacao();
}

void mostrarCreditos() {
    screenSetColor(LIGHTMAGENTA, DARKGRAY);
    screenClear();
    screenGotoxy(35, 8);
    printf("Desenvolvido por:");
    screenGotoxy(35, 10);
    printf("Thiago Queiroz");
    screenGotoxy(35, 11);
    printf("Lucas Rodrigues");
    screenGotoxy(35, 12);
    printf("Luiz Nogueira");
    screenGotoxy(35, 20);
    printf("Pressione qualquer tecla para voltar.");
}

void animarCoracao() {
    screenSetColor(RED, DARKGRAY);
    screenGotoxy(30, 3);
    printf("   ⢀⣴⣾⣿⣿⣿⣷⣦⡄⠀⣴⣾⣿⣿⣿⣿⣶⣄⠀⠀ ");
    screenGotoxy(30, 4);
    printf("  ⣰⣿⣿⣿⣿⣿⣿⣿⠋⢠⣾⣿⣿⣿⣿⣿⣿⣿⣿⣧⠀");
    screenGotoxy(30, 5);
    printf("  ⣿⣿⣿⣿⣿⣿⣿⣿⣿⣶⣌⠛⣿⣿⣿⣿⣿⣿⣿⣿⡆");
    screenGotoxy(30, 6);
    printf("  ⣿⣿⣿⣿⣿⣿⣿⣿⣿⡿⢁⣼⣿⣿⣿⣿⣿⣿⣿⣿⠁");
    screenGotoxy(30, 7);
    printf("  ⠸⣿⣿⣿⣿⣿⣿⣿⡟⢀⣾⣿⣿⣿⣿⣿⣿⣿⣿⠏⠀");
    screenGotoxy(30, 8);
    printf("⠀  ⠙⣿⣿⣿⣿⣿⣿⣄⠻⣿⣿⣿⣿⣿⣿⣿⣿⠏⠀⠀");
    screenGotoxy(30, 9);
    printf("⠀⠀  ⠈⠻⣿⣿⣿⣿⣿⣧⡈⢿⣿⣿⣿⣿⡟⠁⠀⠀⠀");
    screenGotoxy(30, 10);
    printf("⠀⠀⠀⠀  ⠈⠻⣿⣿⣿⣿⡇⢸⣿⣿⠟⠉⠀⠀⠀⠀⠀");
    screenGotoxy(30, 11);
    printf("⠀⠀⠀⠀⠀ ⠀ ⠈⠙⢿⡿⠀⡿⠛⠁⠀⠀⠀⠀⠀⠀⠀");
}

void desenharPersonagemASCII() {
    screenSetColor(WHITE, BLACK);
    screenGotoxy(15, 1);
    printf("⠀⢀⣠⣤⣶⣷⣿⣾⣦⣤⣤⣀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀");
    screenGotoxy(15, 2);
    printf("⠀⠀⢴⣾⣿⣿⠟⠛⠛⠻⠿⣿⣿⣿⣿⣶⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣠⣴⣾⣿⣿⣿⣶⣶⣶⣶⣦⡤⠀");
    screenGotoxy(15, 3);
    printf("⠀⣴⣿⣿⡟⣡⣴⣶⣶⣶⣤⣄⠉⢿⣿⣿⣧⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣠⣾⣿⣿⣿⣿⠿⠿⣿⣿⣿⣿⣿⣿⣷⡀");
    screenGotoxy(15, 4);
    printf("⣼⣿⣿⡟⣰⣿⣿⣿⣿⣿⣿⣿⣷⡀⠙⣿⣿⡇⠀⠀⠀⠀⠀⠀⠀⠀⣸⣿⣿⣿⠟⠉⠀⠀⠀⠀⠀⠈⠙⠻⣿⣿⣷⡄");
    screenGotoxy(15, 5);
    printf("⣻⣿⣿⡇⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⠀⠸⣿⡇⠀⠀⠀⠀⣠⠀⠀⠀⣿⣿⣿⠃⠀⠀⣠⣾⣿⣿⣿⣶⣤⡀⣿⣿⣿⡄");
    screenGotoxy(15, 6);
    printf("⠿⣿⣿⣿⣌⠻⣿⣿⣿⣿⣿⣿⣿⠟⠀⢠⣿⠇⠀⠀⠀⣤⣯⠀⠀⠀⣿⣿⡇⠀⠀⣼⣿⣿⣿⣿⣿⣿⣿⡇⣿⣿⣿⡇");
    screenGotoxy(15, 7);
    printf("⠐⢿⣿⣿⣿⣷⣌⡙⠛⠻⠛⠋⢁⣠⣴⣿⠏⠀⢀⣴⣿⣿⣿⣷⡀⠀⢹⣿⣷⠀⠀⢿⣿⣿⣿⣿⣿⣿⣿⢡⣿⣿⣿⠀");
    screenGotoxy(15, 8);
    printf("⠀⠝⢻⣿⣿⣿⣿⣿⣿⣶⣶⣾⣿⣿⣿⠋⢀⣴⣿⣿⣿⣿⣿⣿⣷⠀⠙⣿⣿⣷⣄⡈⠛⠛⠛⠛⢛⣫⣵⣿⣿⣿⡇⠀");
    screenGotoxy(15, 9);
    printf("⠀⠄⠀⠛⣿⢿⣿⣿⣿⣿⣿⠟⠉⠀⠁⠠⣿⣿⣿⣿⣿⣿⣿⣿⣿⡇⠐⠿⢿⣿⣿⣿⣷⣶⣶⣿⣿⣿⣿⣿⣿⣿⠃⠀");
    screenGotoxy(15, 10);
    printf("⠀⢠⣤⣄⣛⣹⣿⣇⣙⡏⠁⠀⠀⠀⠀⠀⠻⢿⠛⠻⢿⢿⣿⣿⣿⡇⠀⠀⠀⠙⠿⣿⡟⠭⣿⣿⣿⣿⣿⣿⡿⠃⠀⠀");
    screenGotoxy(15, 11);
    printf("⠀⠀⠻⣿⣿⣿⣿⣿⣿⣿⣶⣶⣤⣤⣀⣀⣀⡘⠀⠀⠀⠀⠐⢿⡿⠁⠀⠀⠀⠀⠀⣸⠃⠀⣨⣴⣿⣿⣿⣿⠇⠀⠀⠀");
    screenGotoxy(15, 12);
    printf("⠀⠀⠀⠈⢿⣿⣿⣿⣿⣿⣿⣧⡟⠹⣉⡿⠋⠻⡿⠻⢷⡶⠦⣾⣇⣀⣀⣀⣴⣶⣶⣿⣷⣾⣿⣿⡿⢿⣿⠏⠀⠀⠀⠀");
    screenGotoxy(15, 13);
    printf("⠀⠀⠀⠀⠀⠸⣿⣿⣿⣾⣿⣩⣷⣤⣨⣧⡀⢀⠇⠀⠀⡇⠀⠀⢿⠀⢿⣭⣿⣿⣼⣿⣿⣿⡿⠋⠀⢸⡟⠀⠀⠀⠀⠀");
    screenGotoxy(15, 14);
    printf("⠀⠀⠀⠀⠀⠀⠻⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣷⣶⣾⣿⣶⣶⣧⣤⣼⣷⣿⣿⣿⣿⣿⠏⠀⠀⠀⠈⠇⠀⠀⠀⠀⠀");
    screenGotoxy(15, 15);
    printf("⠀⠀⠀⠀⠀⠀⠀⣿⡈⠿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⠁⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀");
    screenGotoxy(15, 16);
    printf("⠀⠀⠀⠀⠀⠀⠀⠈⠣⠀⠘⠻⣿⣿⣿⢿⣿⣿⣿⣿⡿⢿⣿⢿⣿⣿⣿⣿⣿⠿⠃⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀");
    screenGotoxy(15, 17);
    printf("⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠘⢿⣷⣤⣧⣀⠀⡇⠀⠀⣧⠀⠸⠀⠘⢿⣿⠃⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀");
    screenGotoxy(15, 18);
    printf("⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠹⣿⣿⣿⣿⣿⣶⣶⣿⣦⣾⣷⣾⡿⠏⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀");
    screenGotoxy(15, 19);
    printf("⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢻⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⠃⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀");
    screenGotoxy(15, 20);
    printf("⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠉⠻⣿⠟⠉⠁⠀⣸⡟⠑⠃⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀");
    screenGotoxy(15, 21);
    printf("⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢻⠃⠀⠀⠈⠁⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀");
}

void bossAtirar() {
    static int cooldown = 0;

     if (cooldown <= 0) {
        cooldown = velocidadeBossAtirar + rand() % 30;

        for (int i = 0; i < MAX_PROJETEIS; i++) {
            if (!projeteis[i].active) {
                // Cada projétil é ativado em uma posição X aleatória no topo da área de jogo
                projeteis[i].active = 1;
                projeteis[i].x = AREA_INICIO_X + (rand() % (AREA_FIM_X - AREA_INICIO_X - 1));
                projeteis[i].y = AREA_INICIO_Y;  // Sempre começa no topo
                break;  // Sai do loop após ativar um projétil, para que apenas um seja disparado por vez
            }
        }
    } else {
        cooldown--;
    }
}

void atualizarProjeteis() {
    static int moveCooldown = 0;

    if (moveCooldown <= 0) {
        moveCooldown = velocidadeProjeteis;
        for (int i = 0; i < MAX_PROJETEIS; i++) {
            if (projeteis[i].active) {
                // Limpa a posição anterior do projétil
                screenGotoxy(projeteis[i].x, projeteis[i].y);
                printf(" ");

                // Move o projétil para baixo
                projeteis[i].y += 1;
                screenGotoxy(projeteis[i].x, projeteis[i].y);
                printf("v");

                // Desativa o projétil se ele atingir o limite inferior da área de jogo
                if (projeteis[i].y >= AREA_FIM_Y) {
                    projeteis[i].active = 0;
                }
            }
        }
    } else {
        moveCooldown--;
    }
}

int detectarColisaoProjeteis() {
    for (int i = 0; i < MAX_PROJETEIS; i++) {
        if (projeteis[i].active && projeteis[i].x == coracaoX && projeteis[i].y == coracaoY) {
            projeteis[i].active = 0;
            return 1;
        }
    }
    return 0;
}

void desenharAreaComCoracao() {
    screenSetColor(LIGHTMAGENTA, DARKGRAY);
    screenBoxEnable();
    for (int i = AREA_INICIO_X; i <= AREA_FIM_X; i++) {
        screenGotoxy(i, AREA_INICIO_Y);
        printf("═");
        screenGotoxy(i, AREA_FIM_Y);
        printf("═");
    }
    for (int i = AREA_INICIO_Y; i <= AREA_FIM_Y; i++) {
        screenGotoxy(AREA_INICIO_X, i);
        printf("║");
        screenGotoxy(AREA_FIM_X, i);
        printf("║");
    }
    screenGotoxy(AREA_INICIO_X, AREA_INICIO_Y); printf("╔");
    screenGotoxy(AREA_FIM_X, AREA_INICIO_Y); printf("╗");
    screenGotoxy(AREA_INICIO_X, AREA_FIM_Y); printf("╚");
    screenGotoxy(AREA_FIM_X, AREA_FIM_Y); printf("╝");

    screenSetColor(RED, DARKGRAY);
    screenGotoxy(coracaoX, coracaoY);
    printf("❤");
    screenBoxDisable();

    desenharOssos();
    atualizarProjeteis();
    atualizarObstaculosVerticais();
    atualizarStatus();
}

void atualizarStatus() {
    screenSetColor(RED, BLACK);
    screenGotoxy(AREA_INICIO_X, AREA_FIM_Y + 2);
    printf("HP: %d", health);
    screenGotoxy(AREA_FIM_X - 10, AREA_FIM_Y + 2);  // Posiciona a pontuação no canto direito
    printf("Score: %d", score);
}

void desenharOssos() {
    screenSetColor(WHITE, DARKGRAY);
    for (int i = 0; i < MAX_OBSTACULOS; i++) {
        if (ossos[i].active) {
            for (int j = 0; j < ossos[i].comprimento; j++) {
                screenGotoxy(ossos[i].x, ossos[i].y + j);
                printf("|");
            }
        }
    }
}

void atualizarOssos() {
    for (int i = 0; i < MAX_OBSTACULOS; i++) {
        if (ossos[i].active) {
            ossos[i].x -= 1;
            if (ossos[i].x < AREA_INICIO_X + 1) {
                ossos[i].active = 0;
            }
        }
    }
}

void moverCoracao(int upPressed, int downPressed, int leftPressed, int rightPressed) {
    if (upPressed && coracaoY > AREA_INICIO_Y + 1) {
        coracaoY--;
    }
    if (downPressed && coracaoY < AREA_FIM_Y - 2) {
        coracaoY++;
    }
    if (leftPressed && coracaoX > AREA_INICIO_X + 1) {
        coracaoX--;
    }
    if (rightPressed && coracaoX < AREA_FIM_X - 1) {
        coracaoX++;
    }
    if (detectarColisao() || detectarColisaoProjeteis()) {
        health -= 20;
    }
}

void gerarObstaculos() {
    contadorObstaculos++;
    if (contadorObstaculos >= velocidadeObstaculos) {
        contadorObstaculos = 0;
        for (int i = 0; i < MAX_OBSTACULOS; i++) {
            if (!ossos[i].active) {
                ossos[i].active = 1;
                ossos[i].x = AREA_FIM_X - 1;
                ossos[i].y = (rand() % (AREA_FIM_Y - AREA_INICIO_Y - 2)) + AREA_INICIO_Y + 1;
                ossos[i].comprimento = rand() % 3 + 2;
                break;
            }
        }
    }
}

int detectarColisao() {
    for (int i = 0; i < MAX_OBSTACULOS; i++) {
        if (ossos[i].active) {
            for (int j = 0; j < ossos[i].comprimento; j++) {
                if (ossos[i].x == coracaoX && ossos[i].y + j == coracaoY) {
                    ossos[i].active = 0;
                    return 1;
                }
            }
        }
    }
    return 0;
}

void gerarObstaculosVerticais() {
    static int cooldownVertical = 0;
    cooldownVertical++;
    if (faseAtual == 2 && cooldownVertical >= 30) {
        cooldownVertical = 0;
        for (int i = 0; i < MAX_OBSTACULOS_VERTICAIS; i++) {
            if (!obstaculosVerticais[i].active) {
                obstaculosVerticais[i].active = 1;
                obstaculosVerticais[i].x = (rand() % (AREA_FIM_X - AREA_INICIO_X - 2)) + AREA_INICIO_X + 1;
                obstaculosVerticais[i].y = AREA_INICIO_Y + 1;
                obstaculosVerticais[i].direcao = 1; // Começa descendo
                break;
            }
        }
    }
}

void atualizarObstaculosVerticais() {
    for (int i = 0; i < MAX_OBSTACULOS_VERTICAIS; i++) {
        if (obstaculosVerticais[i].active) {
            // Limpa a posição anterior
            screenGotoxy(obstaculosVerticais[i].x, obstaculosVerticais[i].y);
            printf(" ");

            // Move o obstáculo
            obstaculosVerticais[i].y += obstaculosVerticais[i].direcao;

            // Verifica limites e inverte direção
            if (obstaculosVerticais[i].y >= AREA_FIM_Y -1) {
                obstaculosVerticais[i].direcao = -1;
            } else if (obstaculosVerticais[i].y <= AREA_INICIO_Y + 1) {
                obstaculosVerticais[i].direcao = 1;
            }

            // Desenha o obstáculo
            screenGotoxy(obstaculosVerticais[i].x, obstaculosVerticais[i].y);
            printf("*");

            // Verifica colisão com o coração
            if (obstaculosVerticais[i].x == coracaoX && obstaculosVerticais[i].y == coracaoY) {
                obstaculosVerticais[i].active = 0;
                health -= 30;
            }
        }
    }
}

void limparAreaJogo() {
    for (int y = AREA_INICIO_Y + 1; y <= AREA_FIM_Y - 1; y++) {
        for (int x = AREA_INICIO_X + 1; x <= AREA_FIM_X - 1; x++) {
            screenGotoxy(x, y);
            printf(" ");
        }
    }
}

void iniciarJogo(Mix_Music *bgMusic) {
    health = 200;
    coracaoX = AREA_INICIO_X + 3;
    coracaoY = AREA_FIM_Y - 2;
    score = 0; // Redefinir a pontuação ao iniciar o jogo
    for (int i = 0; i < MAX_OBSTACULOS; i++) {
        ossos[i].active = 0;
    }
    for (int i = 0; i < MAX_PROJETEIS; i++) {
        projeteis[i].active = 0;
    }
    for (int i = 0; i < MAX_OBSTACULOS_VERTICAIS; i++) {
    obstaculosVerticais[i].active = 0;
}
    contadorObstaculos = 0;
    int jogando = 1;
    screenClear();
    desenharPersonagemASCII();
    while (jogando && health > 0) {
         if (score >= 2000 && faseAtual == 1) {
            faseAtual = 2;
            mudarParaSegundaFase();
        }
        int upPressed = 0, downPressed = 0, leftPressed = 0, rightPressed = 0;
        if (keyhit()) {
            int tecla = readch();
            if (tecla == 'w' || tecla == 'W') upPressed = 1;
            if (tecla == 's' || tecla == 'S') downPressed = 1;
            if (tecla == 'a' || tecla == 'A') leftPressed = 1;
            if (tecla == 'd' || tecla == 'D') rightPressed = 1;
            if (tecla == 'q' || tecla == 'Q') {
                jogando = 0;
                break;
            }
        }
        moverCoracao(upPressed, downPressed, leftPressed, rightPressed);
        atualizarOssos();
        gerarObstaculos();
        bossAtirar();
        limparAreaJogo();
        desenharAreaComCoracao();
        screenUpdate();
        score++; // Aumenta a pontuação a cada ciclo do jogo
        usleep(30000);
    }
    Mix_HaltMusic();
    Mix_FreeMusic(bgMusic);
    finalizarJogo(score); // Salvar a pontuação após o jogo
}

// Função para finalizar o jogo
void finalizarJogo(int pontuacaoAtual) {
    salvarMaiorPontuacao(pontuacaoAtual);
    screenSetColor(RED, BLACK);
    screenClear();
    const char *gameOverMessage[] = {
        "*****************************************",
        "*                                       *",
        "*         A última centelha             *",
        "*                                       *",
        "*     se apagou no vazio...             *",
        "*                                       *",
        "*     Talvez haja um novo começo.       *",
        "*                                       *",
        "*****************************************"
    };
    int messageLines = sizeof(gameOverMessage) / sizeof(gameOverMessage[0]);
    int startY = (AREA_FIM_Y - messageLines) / 2;
    int startX = (AREA_FIM_X - strlen(gameOverMessage[0])) / 2;
    for (int i = 0; i < messageLines; i++) {
        screenGotoxy(startX, startY + i);
        printf("%s", gameOverMessage[i]);
    }
    screenUpdate();
    sleep(2);
}

void mudarParaSegundaFase() {
    // Aumenta a dificuldade dos obstáculos e projéteis
    velocidadeObstaculos = 10;   // Obstáculos aparecem mais rápido
    velocidadeProjeteis = 3;     // Projéteis se movem mais rápido
    velocidadeBossAtirar = 15;   // Boss atira com mais frequência

    // Opcional: Mudar a aparência do jogo
    screenClear();
    screenSetColor(YELLOW, DARKGRAY);
    screenGotoxy(AREA_INICIO_X, AREA_INICIO_Y - 2);
    printf("SEGUNDA FASE - Prepare-se para o Desafio!");
}

int main() {
    int ch = 0;
    screenInit(1);
    keyboardInit();
    timerInit(50);

    // Inicializa SDL2 para áudio
    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        fprintf(stderr, "Erro ao inicializar SDL: %s\n", SDL_GetError());
        return 1;
    }
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        fprintf(stderr, "Erro ao inicializar SDL_mixer: %s\n", Mix_GetError());
        return 1;
    }

    mostrarMenuPrincipal();
    screenUpdate();

    while (jogoEmExecucao) {
        if (keyhit()) {
            ch = readch();
            if (ch == '1') {
                Mix_Music *bgMusic = Mix_LoadMUS("background_music.mp3");
                if (!bgMusic) {
                    fprintf(stderr, "Erro ao carregar música: %s\n", Mix_GetError());
                    return 1;
                }
                Mix_PlayMusic(bgMusic, -1);
                screenClear();
                iniciarJogo(bgMusic);
                mostrarMenuPrincipal();
            } else if (ch == '2') {
                mostrarCreditos();
                screenUpdate();
                readch();
                mostrarMenuPrincipal();
            } else if (ch == '3') {
                jogoEmExecucao = 0;
            }
            screenUpdate();
        }
    }

    // Finalizar SDL2 e liberar recursos
    Mix_CloseAudio();
    SDL_Quit();
    keyboardDestroy();
    screenDestroy();
    timerDestroy();

    return 0;
}
