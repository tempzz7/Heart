#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include "screen.h"
#include "keyboard.h"
#include "timer.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

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

typedef struct {
    int x, y;
    int active;
    int comprimento;
} ObstaculoOsso;

typedef struct {
    int x, y;
    int active;
} Projeteis;

#define MAX_OBSTACULOS 10
#define MAX_PROJETEIS 5

ObstaculoOsso ossos[MAX_OBSTACULOS];
Projeteis projeteis[MAX_PROJETEIS];
int contadorObstaculos = 0;

// Declaração antecipada das funções
void animarCoracao();
void mostrarMenuPrincipal();
void mostrarCreditos();
void desenharAreaComCoracao();
void desenharOssos();
void atualizarOssos();
void iniciarJogo(Mix_Music *bgMusic);
void moverCoracao(int upPressed, int downPressed, int leftPressed, int rightPressed);
void gerarObstaculos();
int detectarColisao();
void atualizarStatus();
void desenharPersonagemASCII();
void limparAreaJogo();
void bossAtirar();
void atualizarProjeteis();
int detectarColisaoProjeteis();

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
        cooldown = 20 + rand() % 30;  // Tempo de intervalo entre 20 e 50 para disparos aleatórios

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
        moveCooldown = 5;  // Define a velocidade dos projéteis; quanto maior, mais lento o movimento

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
        if (health <= 0) {
            jogoEmExecucao = 0;
        }
    }
}

void gerarObstaculos() {
    contadorObstaculos++;
    if (contadorObstaculos >= 20) {
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

void limparAreaJogo() {
    for (int y = AREA_INICIO_Y; y <= AREA_FIM_Y; y++) {
        for (int x = AREA_INICIO_X; x <= AREA_FIM_X; x++) {
            screenGotoxy(x, y);
            printf(" ");
        }
    }
}

void iniciarJogo(Mix_Music *bgMusic) {
    health = 200;
    coracaoX = AREA_INICIO_X + 3;
    coracaoY = AREA_FIM_Y - 2;
    score = 0;  // Resetar a pontuação ao iniciar o jogo

    for (int i = 0; i < MAX_OBSTACULOS; i++) {
        ossos[i].active = 0;
    }
    for (int i = 0; i < MAX_PROJETEIS; i++) {
        projeteis[i].active = 0;
    }
    contadorObstaculos = 0;

    screenClear();
    desenharPersonagemASCII();

    while (jogoEmExecucao && health > 0) {
        int upPressed = 0, downPressed = 0, leftPressed = 0, rightPressed = 0;

        if (keyhit()) {
            int tecla = readch();
            if (tecla == 'w' || tecla == 'W') upPressed = 1;
            if (tecla == 's' || tecla == 'S') downPressed = 1;
            if (tecla == 'a' || tecla == 'A') leftPressed = 1;
            if (tecla == 'd' || tecla == 'D') rightPressed = 1;
            if (tecla == 'q' || tecla == 'Q') {
                jogoEmExecucao = 0;
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

        score++;  // Incrementa a pontuação a cada ciclo do jogo

        usleep(30000);
    }

    if (health <= 0) {
        // Para a música ao finalizar o jogo e libera a memória
        Mix_HaltMusic();
        Mix_FreeMusic(bgMusic);

        screenSetColor(RED, BLACK);
        screenClear();
        
        const char *gameOverMessage[] = {
            "*****************************************************",
            "*                                                   *",
            "*               A ultima centelha se apagou.        *",
            "*                                                   *",
            "*      A alma se perdeu no vazio... mas talvez      *",
            "*        ainda haja esperança em um novo começo.    *",
            "*                                                   *",
            "*****************************************************"
        };
        int messageLines = sizeof(gameOverMessage) / sizeof(gameOverMessage[0]);
        int startY = (MAXY - messageLines) / 2;
        int startX = (MAXX - strlen(gameOverMessage[0])) / 2;

        for (int i = 0; i < messageLines; i++) {
            screenGotoxy(startX, startY + i);
            printf("%s", gameOverMessage[i]);
        }

        screenUpdate();
        sleep(20);
    }
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
                // Carrega a música e toca em loop ao iniciar o jogo
                Mix_Music *bgMusic = Mix_LoadMUS("background_music.mp3");
                if (!bgMusic) {
                    fprintf(stderr, "Erro ao carregar música: %s\n", Mix_GetError());
                    return 1;
                }
                Mix_PlayMusic(bgMusic, -1);

                screenClear();
                desenharPersonagemASCII();
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

    // Fecha SDL2_mixer
    Mix_CloseAudio();
    SDL_Quit();

    keyboardDestroy();
    screenDestroy();
    timerDestroy();
    return 0;
}