#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include "screen.h"
#include "keyboard.h"
#include "timer.h"

// Definições de macros e constantes
#define AREA_INICIO_X 10
#define AREA_FIM_X 70
#define AREA_INICIO_Y 15
#define AREA_FIM_Y 25
#define PONTOS_FASE2 2000
#define PONTOS_FASE3 4000
#define PONTOS_FIM_JOGO 6000
#define MAX_OBSTACULOS 10
#define MAX_OBSTACULOS_MAGENTA 5
#define MAX_OBSTACULOS_AMARELOS 2

// Declaração de variáveis globais
int menuOpcao = 0;
int jogoEmExecucao = 1;
int coracaoX = AREA_INICIO_X + 3;
int coracaoY = AREA_FIM_Y - 2;
int health = 200;
int bossAtaqueCooldown = 0;
int score = 0;
int faseAtual = 1;
int velocidadeObstaculos = 20;
Mix_Music *bgMusicFase1;
Mix_Music *bgMusicFase2;
int jogoPausado = 0;
int playerMoving = 0;
int maiorPontuacaoGlobal = 0;
char nomeMaiorPontuacao[50] = "";
int contadorObstaculos = 0;

// Definição de tipos (structs)
typedef struct No {
    char nome[50];
    int pontuacao;
    struct No *proximo;
} No;

typedef struct {
    int x, y;
    int active;
    int comprimento;
} ObstaculoOsso;

typedef struct {
    int x, y;
    int active;
    int comprimento;
} ObstaculoMagenta;

typedef struct {
    int x, y;
    int active;
    int comprimento;
} ObstaculoAmarelo;

// Arrays de obstáculos
ObstaculoOsso ossos[MAX_OBSTACULOS];
ObstaculoMagenta obstaculosMagenta[MAX_OBSTACULOS_MAGENTA];
ObstaculoAmarelo obstaculosAmarelos[MAX_OBSTACULOS_AMARELOS];

// Declaração das funções (prototipação)
void animarCoracao();
void mostrarMenuPrincipal(No *pontuacoes);
void mostrarCreditos();
void mostrarLore();
void mostrarInstrucoes();
void mostrarMaiorPontuacao();
void mostrarRanking();
void desenharPersonagemASCII();
void desenharAreaComCoracao();
void desenharOssos();
void atualizarOssos();
void moverCoracao(int upPressed, int downPressed, int leftPressed, int rightPressed);
void gerarObstaculos();
void gerarObstaculosMagenta();
void atualizarObstaculosMagenta();
void gerarObstaculosAmarelos();
void atualizarObstaculosAmarelos();
void limparAreaJogo();
void atualizarStatus();
int detectarColisao();
void mudarParaSegundaFase();
void desenharBossFixo();
void mudarParaTerceiraFase();
void desenharBossTerceiraFase();
void iniciarJogo(Mix_Music *bgMusic, No *pontuacoes);
void finalizarJogo(No *pontuacoes, int pontuacaoAtual, int jogoVencido);
void morte(No *pontuacoes, int pontuacaoAtual);
No* criarNo(char *nome, int pontuacao);
void adicionarPontuacao(No **cabeca, char *nome, int pontuacao);
void salvarPontuacao(char *nome, int pontuacao);
void salvarMaiorPontuacao();
void carregarMaiorPontuacao();

// Implementação das funções

// Funções relacionadas ao menu e exibição

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

void mostrarMenuPrincipal(No *pontuacoes) {
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

    screenSetColor(LIGHTMAGENTA, DARKGRAY);
    screenGotoxy(33, 19);
    printf("1. Iniciar");
    screenGotoxy(33, 20);
    printf("2. Créditos");
    screenGotoxy(33, 21);
    printf("3. Lore");
    screenGotoxy(33, 22);
    printf("4. Sair");
    screenGotoxy(33, 23);
    printf("5. Ranking");
    screenGotoxy(33, 24);
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
    screenUpdate();
    readch();
}

void mostrarLore() {
    screenSetColor(BLACK, DARKGRAY);
    screenClear();

    const char *loreText[] = {
        "╔═════════════════════════════════════════════════════════╗",
        "║                                                         ║",
        "║   Em um mundo onde a luz se apagou,                     ║",
        "║   você é a última centelha de um                        ║",
        "║   coração que já foi cheio de vida.                     ║",
        "║                                                         ║",
        "║   Preso em uma dimensão entre o vazio                   ║",
        "║   e a esperança, você enfrenta os                       ║",
        "║   ecos sombrios dos medos e                             ║",
        "║   arrependimentos de uma alma perdida.                  ║",
        "║                                                         ║",
        "║   Cada passo que você dá, cada                          ║",
        "║   obstáculo que supera, é uma luta                      ║",
        "║   para recuperar fragmentos de                          ║",
        "║   sentimentos e memórias esquecidas.                    ║",
        "║                                                         ║",
        "║   Mas cuidado, pois se o HP do                          ║",
        "║   coração chegar a zero, ele se                         ║",
        "║   despedaçará, e toda esperança será                    ║",
        "║   perdida para sempre.                                  ║",
        "║                                                         ║",
        "║   Você não está apenas jogando, está                    ║",
        "║   lutando para salvar o que resta de                    ║",
        "║   uma alma à beira do esquecimento.                     ║",
        "║                                                         ║",
        "╚═════════════════════════════════════════════════════════╝",
    };

    const char *pressKeyText = "╔═════════════════════════════════════════════════════════╗\n"
                               " ║ Pressione qualquer tecla para continuar                 ║\n"
                               " ╚═════════════════════════════════════════════════════════╝";

    int loreLines = sizeof(loreText) / sizeof(loreText[0]);
    int startY = (AREA_FIM_Y - AREA_INICIO_Y + 1 - loreLines) / 2 + AREA_INICIO_Y;
    int screenWidth = AREA_FIM_X - AREA_INICIO_X + 1;
    int loreWidth = strlen(loreText[0]);
    int startX = (screenWidth - loreWidth) / 2 + AREA_INICIO_X;

    for (int i = 0; i < loreLines; i++) {
        screenGotoxy(startX, startY + i);
        printf("%s", loreText[i]);
    }

    screenGotoxy(startX, startY + loreLines + 1);
    printf("%s", pressKeyText);

    screenUpdate();
    readch();
}

void mostrarInstrucoes() {
    screenSetColor(LIGHTMAGENTA, DARKGRAY);
    screenClear();
    screenGotoxy(10, 10);
    printf("Instruções do Jogo");
    screenGotoxy(10, 12);
    printf("Movimento: Utilize as teclas a seguir para controlar seu personagem:");
    screenGotoxy(10, 14);
    printf("W - Move para cima");
    screenGotoxy(10, 15);
    printf("S - Move para baixo");
    screenGotoxy(10, 16);
    printf("A - Move para a esquerda");
    screenGotoxy(10, 17);
    printf("D - Move para a direita");
    screenGotoxy(10, 19);
    printf("Objetivo:");
    screenGotoxy(10, 21);
    printf("Desviar das estruturas brancas a todo custo, pois entrar em contato com elas,");
    screenGotoxy(10, 22);
    printf("o coração levará dano.");
    screenGotoxy(10, 24);
    printf("Estruturas roxas podem ser atravessadas, mas somente se você permanecer");
    screenGotoxy(10, 25);
    printf("imóvel ao passar por elas.");
    screenGotoxy(10, 27);
    printf("Boa sorte e divirta-se ao enfrentar esses desafios!");
    screenGotoxy(10, 29);
    printf("Pressione qualquer tecla para iniciar o jogo...");
    screenUpdate();
    readch();
}

void mostrarMaiorPontuacao() {
    screenSetColor(YELLOW, DARKGRAY);
    screenGotoxy(33, 25);
    printf("Maior Pontuação: %d ", maiorPontuacaoGlobal);
}

void mostrarRanking() {
    screenSetColor(LIGHTMAGENTA, DARKGRAY);
    screenClear();

    typedef struct {
        char nome[50];
        int pontuacao;
    } Jogador;

    Jogador jogadores[100];
    int totalJogadores = 0;

    FILE *arquivo = fopen("scoreleaders.txt", "r");
    if (arquivo == NULL) {
        printf("Nenhuma pontuação registrada ainda.\n");
        screenUpdate();
        readch();
        return;
    }

    while (fscanf(arquivo, "%s %d", jogadores[totalJogadores].nome, &jogadores[totalJogadores].pontuacao) != EOF) {
        totalJogadores++;
        if (totalJogadores >= 100) break;
    }
    fclose(arquivo);

    for (int i = 0; i < totalJogadores - 1; i++) {
        for (int j = i + 1; j < totalJogadores; j++) {
            if (jogadores[i].pontuacao < jogadores[j].pontuacao) {
                Jogador temp = jogadores[i];
                jogadores[i] = jogadores[j];
                jogadores[j] = temp;
            }
        }
    }

    screenGotoxy(30, 5);
    printf("Ranking de Jogadores");
    screenGotoxy(20, 7);
    printf("Posição   Nome                      Pontuação");
    for (int i = 0; i < totalJogadores && i < 10; i++) {
        screenGotoxy(20, 9 + i);
        printf("%-9d %-25s %d", i + 1, jogadores[i].nome, jogadores[i].pontuacao);
    }

    screenGotoxy(20, 20);
    printf("Pressione qualquer tecla para voltar ao menu.");
    screenUpdate();
    readch();
}

void desenharPersonagemASCII() {
    screenSetColor(WHITE, DARKGRAY);
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
    printf("⠀⠀⠀⠀⠀⠸⣿⣿⣿⣿⣿⣿⣿⣤⣨⣧⡀⢀⠇⠀⠀⡇⠀⠀⢿⠀⢿⣭⣿⣿⣼⣿⣿⣿⡿⠋⠀⢸⡟⠀⠀⠀⠀⠀");
    screenGotoxy(15, 14);
    printf("⠀⠀⠀⠀⠀⠀⠻⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣷⣶⣾⣿⣶⣶⣧⣤⣼⣷⣿⣿⣿⣿⣿⠏⠀⠀⠀⠈⠇⠀⠀⠀⠀⠀");
    screenGotoxy(15, 15);
    printf("⠀⠀⠀⠀⠀⠀⠀⣿⡈⠿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⠁⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀");
    screenGotoxy(15, 16);
    printf("⠀⠀⠀⠀⠀⠀⠀⠈⠣⠀⠘⠻⣿⣿⣿⢿⣿⣿⣿⣿⡿⢿⣿⢿⣿⣿⣿⣿⠿⠃⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀");
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

// Funções relacionadas ao jogo

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
    screenSetColor(RED, BLACK);
    screenGotoxy(coracaoX, coracaoY);
    printf("💜");
    screenBoxDisable();
    desenharOssos();
    atualizarStatus();
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

    // Desenha obstáculos roxos
    for (int i = 0; i < MAX_OBSTACULOS_MAGENTA; i++) {
        if (obstaculosMagenta[i].active) {
            screenSetColor(DARK_PURPLE, BLACK);
            for (int j = 0; j < obstaculosMagenta[i].comprimento; j++) {
                screenGotoxy(obstaculosMagenta[i].x, obstaculosMagenta[i].y + j);
                printf("|");
            }
        }
    }

    // Desenha obstáculos amarelos
    for (int i = 0; i < MAX_OBSTACULOS_AMARELOS; i++) {
        if (obstaculosAmarelos[i].active) {
            screenSetColor(YELLOW, BLACK);
            for (int j = 0; j < obstaculosAmarelos[i].comprimento; j++) {
                screenGotoxy(obstaculosAmarelos[i].x, obstaculosAmarelos[i].y + j);
                printf("💖");
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
    playerMoving = 0;

    if (upPressed && coracaoY > AREA_INICIO_Y + 1) {
        coracaoY--;
        playerMoving = 1;
    }
    if (downPressed && coracaoY < AREA_FIM_Y - 2) {
        coracaoY++;
        playerMoving = 1;
    }
    if (leftPressed && coracaoX > AREA_INICIO_X + 1) {
        coracaoX--;
        playerMoving = 1;
    }
    if (rightPressed && coracaoX < AREA_FIM_X - 1) {
        coracaoX++;
        playerMoving = 1;
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

void gerarObstaculosMagenta() {
    static int contadorFase3 = 0;
    contadorFase3++;

    int frequencia = (faseAtual == 3) ? 25 : 30;

    if (contadorFase3 >= frequencia) {
        contadorFase3 = 0;
        for (int i = 0; i < MAX_OBSTACULOS_MAGENTA; i++) {
            if (!obstaculosMagenta[i].active) {
                obstaculosMagenta[i].active = 1;
                obstaculosMagenta[i].x = AREA_FIM_X - 1;
                obstaculosMagenta[i].y = (rand() % (AREA_FIM_Y - AREA_INICIO_Y - 4)) + AREA_INICIO_Y + 2;
                obstaculosMagenta[i].comprimento = rand() % 3 + 2;
                break;
            }
        }
    }
}

void atualizarObstaculosMagenta() {
    for (int i = 0; i < MAX_OBSTACULOS_MAGENTA; i++) {
        if (obstaculosMagenta[i].active) {
            for (int j = 0; j < obstaculosMagenta[i].comprimento; j++) {
                screenGotoxy(obstaculosMagenta[i].x, obstaculosMagenta[i].y + j);
                printf(" ");
            }

            obstaculosMagenta[i].x -= 1;

            if (obstaculosMagenta[i].x < AREA_INICIO_X + 1) {
                obstaculosMagenta[i].active = 0;
            } else {
                screenSetColor(DARK_PURPLE, BLACK);
                for (int j = 0; j < obstaculosMagenta[i].comprimento; j++) {
                    screenGotoxy(obstaculosMagenta[i].x, obstaculosMagenta[i].y + j);
                    printf("|");
                }
            }
        }
    }
}

void gerarObstaculosAmarelos() {
    static int contadorAmarelos = 0;
    contadorAmarelos++;
    int frequencia = 150;

    if (faseAtual == 3 && contadorAmarelos >= frequencia) {
        contadorAmarelos = 0;
        for (int i = 0; i < MAX_OBSTACULOS_AMARELOS; i++) {
            if (!obstaculosAmarelos[i].active) {
                obstaculosAmarelos[i].active = 1;
                obstaculosAmarelos[i].x = AREA_FIM_X - 1;
                obstaculosAmarelos[i].y = (rand() % (AREA_FIM_Y - AREA_INICIO_Y - 2)) + AREA_INICIO_Y + 1;
                obstaculosAmarelos[i].comprimento = 1;
                break;
            }
        }
    }
}

void atualizarObstaculosAmarelos() {
    for (int i = 0; i < MAX_OBSTACULOS_AMARELOS; i++) {
        if (obstaculosAmarelos[i].active) {
            for (int j = 0; j < obstaculosAmarelos[i].comprimento; j++) {
                screenGotoxy(obstaculosAmarelos[i].x, obstaculosAmarelos[i].y + j);
                printf(" ");
            }

            obstaculosAmarelos[i].x -= 1;

            if (obstaculosAmarelos[i].x < AREA_INICIO_X + 1) {
                obstaculosAmarelos[i].active = 0;
            } else {
                screenSetColor(YELLOW, BLACK);
                for (int j = 0; j < obstaculosAmarelos[i].comprimento; j++) {
                    screenGotoxy(obstaculosAmarelos[i].x, obstaculosAmarelos[i].y + j);
                    printf("💖");
                }
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

void atualizarStatus() {
    screenSetColor(RED, BLACK);
    screenGotoxy(AREA_INICIO_X, AREA_FIM_Y + 2);
    printf("HP: %d", health);
    screenGotoxy(AREA_FIM_X - 10, AREA_FIM_Y + 2);
    printf("Score: %d", score);
}

int detectarColisao() {
    // Colisão com obstáculos de osso
    for (int i = 0; i < MAX_OBSTACULOS; i++) {
        if (ossos[i].active) {
            for (int j = 0; j < ossos[i].comprimento; j++) {
                if (ossos[i].x == coracaoX && ossos[i].y + j == coracaoY) {
                    ossos[i].active = 0;
                    health -= 20;
                    return 1;
                }
            }
        }
    }

    // Colisão com obstáculos magenta
    for (int i = 0; i < MAX_OBSTACULOS_MAGENTA; i++) {
        if (obstaculosMagenta[i].active) {
            for (int j = 0; j < obstaculosMagenta[i].comprimento; j++) {
                if (obstaculosMagenta[i].x == coracaoX && obstaculosMagenta[i].y + j == coracaoY) {
                    if (playerMoving) {
                        obstaculosMagenta[i].active = 0;
                        health -= 20;
                        return 1;
                    }
                }
            }
        }
    }

    // Colisão com obstáculos amarelos
    for (int i = 0; i < MAX_OBSTACULOS_AMARELOS; i++) {
        if (obstaculosAmarelos[i].active) {
            for (int j = 0; j < obstaculosAmarelos[i].comprimento; j++) {
                if (obstaculosAmarelos[i].x == coracaoX && obstaculosAmarelos[i].y + j == coracaoY) {
                    obstaculosAmarelos[i].active = 0;
                    health += 20;
                    if (health > 200) health = 200;
                    return 1;
                }
            }
        }
    }

    return 0;
}

void mudarParaSegundaFase() {
    jogoPausado = 1;
    Mix_PauseMusic();

    screenClear();

    desenharBossFixo();

    jogoPausado = 0;
    Mix_ResumeMusic();

    velocidadeObstaculos = 13;

    screenSetColor(YELLOW, DARKGRAY);
    screenGotoxy(AREA_INICIO_X, AREA_INICIO_Y - 2);
}

void desenharBossFixo() {
    screenSetColor(DARKGRAY, BLACK);

    int startX = (AREA_FIM_X + AREA_INICIO_X) / 2 - 16;
    int startY = AREA_INICIO_Y - 15;

    screenGotoxy(startX, startY);
    printf("⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⠏⠙⠋⠉⠛⠛⠛⢿⣿⣿⣿⣿⣿⣿⣿⣿");
    screenGotoxy(startX, startY + 1);
    printf("⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⠏⣀⣀⠀⠀⠀⣀⣀⣀⡨⢿⣿⣿⣿⣿⣿⣿");
    screenGotoxy(startX, startY + 2);
    printf("⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡟⢠⣾⣿⣿⡷⠒⣾⣿⣿⣿⣿⡄⢿⣿⣿⣿⣿⣿");
    screenGotoxy(startX, startY + 3);
    printf("⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡇⣾⣿⣿⡿⣃⣼⣿⠿⣿⣿⣿⡇⣼⣿⣿⣿⣿⣿");
    screenGotoxy(startX, startY + 4);
    printf("⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡇⠉⠉⠁⠀⠛⠉⢙⠁⠀⠀⠀⠠⣿⣿⣿⣿⣿⣿");
    screenGotoxy(startX, startY + 5);
    printf("⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣧⣄⡀⠤⣂⣀⢀⡘⠺⠦⣄⣠⡤⣿⣿⣿⣿⣿⣿");
    screenGotoxy(startX, startY + 6);
    printf("⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡅⠈⡲⠄⠀⡖⠋⠙⠒⣾⠎⠇⢿⣿⣿⣿⣿⣿");
    screenGotoxy(startX, startY + 7);
    printf("⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣧⠀⣷⡖⣮⠄⠀⠒⣲⣿⢸⣇⣿⣿⣿⣿⣿⣿");
    screenGotoxy(startX, startY + 8);
    printf("⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡆⢻⣿⠛⠉⠉⢲⣯⠇⣸⣿⣿⣿⣿⣿⣿⣿");
    screenGotoxy(startX, startY + 9);
    printf("⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣷⠒⠛⠓⡦⢶⠟⠛⢒⣿⣿⣿⣿⣿⣿⣿⣿");
    screenGotoxy(startX, startY + 10);
    printf("⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣆⠀⠀⠀⠀⠑⣰⣿⣿⣿⣿⣿⣿⣿⣿⣿");
    screenGotoxy(startX, startY + 11);
    printf("⣿⣿⣿⣿⠟⠋⠉⢹⠿⣿⣿⣿⠛⢿⣿⡷⠀⠒⠀⢾⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿");
    screenGotoxy(startX, startY + 12);
    printf("⣿⣿⣿⣧⡀⠀⠀⢾⡀⠉⠉⠁⠀⠀⢀⣀⠀⠀⠀⠈⠁⠀⢻⡿⠟⠛⠟⠛⠉⠛");
    screenGotoxy(startX, startY + 13);
    printf("⣿⡿⠛⠛⠉⠂⠀⠀⠀⢀⡂⠓⠄⠀⠈⠉⠗⣉⠈⠁⠂⠀⠀⠈⠁⠀⠀⠀⠀⠀");
    screenGotoxy(startX, startY + 14);
    printf("⣿⡀⠀⠈⠆⠀⠀⠀⠀⠀⠉⠘⠐⠿⠿⠁⠀⠘⠃⠤⠤⠀⠀⠀⠀⠀⠀⠀⠀⠀");

    screenUpdate();
}

void mudarParaTerceiraFase() {
    Mix_PauseMusic();
    screenSetColor(YELLOW, DARKGRAY);
    screenClear();

    velocidadeObstaculos = 10;
    Mix_ResumeMusic();

    for (int i = 0; i < MAX_OBSTACULOS_MAGENTA; i++) {
        obstaculosMagenta[i].active = 0;
    }

    desenharBossTerceiraFase();

    screenSetColor(YELLOW, DARKGRAY);
    screenGotoxy(AREA_INICIO_X, AREA_INICIO_Y - 2);
}

void desenharBossTerceiraFase() {
    int startX = (AREA_FIM_X + AREA_INICIO_X) / 2 - 16;
    int startY = AREA_INICIO_Y - 15;

    screenSetColor(DARKGRAY, BLACK);

    // ASCII Art do Boss
    screenGotoxy(startX, startY + 1);
    printf("⠀⠀⠀⠀⠀⠀⠀⠀⠀⣾⡇⠀⠀⠀⠀⣼⣿⣿⣿⣷⡄⠀⠀⠀⠀⠀⢀⣾⣿⣿⣿⣿⣿⣆⠀⠀⢿⡇⢠⠴⠋⠉⠑⣦⡀⠀⠀⠀⠀⠀");
    screenGotoxy(startX, startY + 2);
    printf("⠀⣴⠟⠓⠲⣄⡀⠀⠀⣿⠁⠀⠀⠀⣾⣿⣿⣿⣿⣿⣿⡄⠀⠀⠀⠀⣼⣿⣿⣿⣿⣿⣿⣿⡄⠀⢸⡿⠁⠀⠀⡠⠂⠈⣷⠀⠀⠀⠀⠀");
    screenGotoxy(startX, startY + 3);
    printf("⣰⡏⢤⡀⠀⠈⠛⣦⣄⣿⠀⠀⢀⣾⣿⣿⣿⣿⣿⣿⣿⡇⠀⠀⠀⢰⣿⣿⣿⣿⣿⣿⣿⣿⣧⠀⠀⣇⠀⢠⠎⠀⠀⢀⣿⠀⠀⠀⠀⠀");
    screenGotoxy(startX, startY + 4);
    printf("⣿⠁⠀⠙⢦⣄⠀⠀⠹⡟⠀⠀⣾⣿⣿⣿⣿⣿⣿⣿⣿⣷⠀⠀⠀⠈⣿⣿⣿⣿⣿⣿⣿⣿⣿⣆⠀⢹⣰⢿⡆⠀⢀⣾⠃⠀⠀⠀⠀⠀");
    screenGotoxy(startX, startY + 5);
    printf("⢻⡆⠀⠀⠀⣿⠇⠀⠀⡇⠀⢠⣿⣟⢻⡟⠛⢻⠟⢻⡿⣿⠄⠀⠀⠀⣿⣿⣻⣋⣉⣋⣠⣧⣴⣿⣦⣸⠃⢸⣿⢀⡾⠃⠀⠀⠀⠀⠀⠀");
    screenGotoxy(startX, startY + 6);
    printf("⠀⠻⣆⠀⠀⢿⡀⠀⠀⡇⢀⣼⣿⣿⣿⣷⣾⣿⣿⣿⣿⣿⣷⣾⣿⣿⣿⣿⣿⣿⣿⠿⠟⠉⠀⣉⣷⣿⣷⠟⠁⠀⠀⠀⠀⠀⠀⠀");
    screenGotoxy(startX, startY + 7);
    printf("⠀⠀⠈⠳⣄⠈⢻⣶⣴⣧⣬⣁⣉⠙⠻⠿⠿⠿⢿⣿⣿⣿⣿⣿⣯⣀⠀⣹⣿⣿⡋⠁⠀⡠⠔⠊⠉⠉⠉⠙⠳⣦⠀⠀⠀⠀⠀⠀⠀");
    screenGotoxy(startX, startY + 8);
    printf("⠀⠀⠀⠀⠈⡿⠛⠉⣀⡀⠀⠀⠀⠉⠉⠒⠀⠀⢸⣿⣿⣿⣿⣿⠋⠁⢀⣼⣿⣿⣇⡀⠀⠀⠀⠀⠀⣴⣶⣦⡀⠀⠘⣧⠀⠀⠀⠀⠀⠀");
    screenGotoxy(startX, startY + 9);
    printf("⠀⠀⠀⠀⣸⠇⢠⣾⣿⣿⡇⠀⠀⣀⣠⣤⣤⠴⢾⣿⣿⣿⣿⣿⣷⣿⣿⣿⣿⡿⠛⠉⠻⢷⣶⣦⣤⣿⣿⣿⣷⠀⠀⢸⠀⠀⠀⠀⠀⠀");
    screenGotoxy(startX, startY + 10);
    printf("⠀⠀⠀⠀⣿⡄⢾⣿⣿⣿⣷⣶⣿⣿⠟⠉⠀⠀⠀⠈⠻⣿⣿⣿⣿⣿⣿⡿⠟⠀⠀⢀⣀⣤⣽⣿⣿⣿⣿⣿⠿⠀⠀⣾⠀⠀⠀⠀⠀⠀");
    screenGotoxy(startX, startY + 11);
    printf("⠀⠀⠀⠀⢹⣇⠈⠛⢿⣿⣿⣿⣿⣶⣶⣶⣦⣤⣤⣄⣀⡘⢿⣿⣿⣠⡤⢒⣢⣴⣾⣿⣿⣿⡿⢿⣯⣿⣿⡇⠀⠀⢰⡟⠀⠀⠀⠀⠀⠀");
    screenGotoxy(startX, startY + 12);
    printf("⠀⠀⠀⠀⠀⢿⡆⠀⠘⣿⣿⣟⠉⢛⠿⣿⣿⣿⣿⣿⣿⣿⣿⣾⣿⣴⣾⣿⠛⠛⣿⠋⠉⡇⢘⣿⡟⢸⠇⠀⠠⣿⡇⠀⠀⠀⠀⠀⠀");
    screenGotoxy(startX, startY + 13);
    printf("⠀⠀⠀⠀⠀⠈⢿⣦⠀⠹⣿⠻⣦⣸⡀⡇⠀⢰⠀⠀⡆⠀⠈⢿⣿⠉⠀⢸⡇⠀⢠⣇⠀⢸⣷⣸⡿⠁⡸⠀⠀⠀⢿⡇⠀⠀⠀⠀⠀⠀");
    screenGotoxy(startX, startY + 14);
    printf("⠀⠀⠀⠀⠀⠀⢸⣿⠀⠀⢻⠀⠙⣿⣿⣿⡀⣼⡄⢰⣷⣤⣦⣼⠿⢤⡴⠛⣷⡤⠻⡟⣦⣾⣿⣿⠃⠀⠀⣰⠇⠀⢸⣿⡀⠀⠀⠀⠀⠀");
    screenGotoxy(startX, startY + 15);
    printf("⠀⠀⠀⠀⠀⠀⢸⣿⠀⢠⡀⠇⠀⠘⢿⣿⣿⣿⣝⣶⣿⡄⠀⢻⠀⢠⡇⠀⣏⣀⣴⣷⣿⣿⣿⠃⠀⡄⢠⣿⣶⢶⣾⣿⠀");
    screenGotoxy(startX, startY + 16);
    printf("⠀⠀⠀⠀⠀⠀⣼⡿⠀⢸⣿⡄⠀⠀⠈⢿⣿⣿⣿⣿⣿⣿⣷⣾⣷⣾⣿⣿⣿⣿⣿⣿⣿⡿⠃⠀⣼⣿⡾⠃⠀⣰⣿⣿");

    screenUpdate();
}

void iniciarJogo(Mix_Music *bgMusic, No *pontuacoes) {
    health = 200;
    coracaoX = AREA_INICIO_X + 3;
    coracaoY = AREA_FIM_Y - 2;
    score = 0;
    faseAtual = 1;

    for (int i = 0; i < MAX_OBSTACULOS; i++) {
        ossos[i].active = 0;
    }
    for (int i = 0; i < MAX_OBSTACULOS_MAGENTA; i++) {
        obstaculosMagenta[i].active = 0;
    }
    for (int i = 0; i < MAX_OBSTACULOS_AMARELOS; i++) {
        obstaculosAmarelos[i].active = 0;
    }
    contadorObstaculos = 0;
    int jogando = 1;
    int jogoVencido = 0;

    screenClear();
    desenharPersonagemASCII();

    while (jogando && health > 0) {
        // Verificar transições de fase
        if (score >= PONTOS_FASE2 && faseAtual == 1) {
            faseAtual = 2;
            mudarParaSegundaFase();
        }
        if (score >= PONTOS_FASE3 && faseAtual == 2) {
            faseAtual = 3;
            mudarParaTerceiraFase();
        }
        if (score >= PONTOS_FIM_JOGO && faseAtual == 3) {
            jogoVencido = 1;
            jogando = 0;
            break;
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

        limparAreaJogo();

        moverCoracao(upPressed, downPressed, leftPressed, rightPressed);
        if (detectarColisao()) {
            if (health <= 0) {
                jogando = 0;
                break;
            }
        }

        atualizarOssos();
        atualizarObstaculosMagenta();
        atualizarObstaculosAmarelos();
        gerarObstaculos();
        gerarObstaculosMagenta();
        gerarObstaculosAmarelos();
        desenharAreaComCoracao();

        screenUpdate();
        score++;
        usleep(25000);
    }

    Mix_HaltMusic();
    Mix_FreeMusic(bgMusic);
    finalizarJogo(pontuacoes, score, jogoVencido);
}

void finalizarJogo(No *pontuacoes, int pontuacaoAtual, int jogoVencido) {
    screenSetColor(RED, BLACK);
    screenClear();

    if (jogoVencido) {
        const char *vitoriaMessage[] = {
            "*****************************************",
            "*                                       *",
            "*  O coração voltou a sentir plenamente!*",
            "*                                       *",
            "* 'As emoções foram recuperadas, e a    *",
            "*  alma perdida encontrou seu caminho.' *",
            "*                                       *",
            "*     Obrigado por jogar!               *",
            "*                                       *",
            "*****************************************"
        };
        int messageLines = sizeof(vitoriaMessage) / sizeof(vitoriaMessage[0]);
        int startY = (AREA_FIM_Y - AREA_INICIO_Y + 1 - messageLines) / 2 + AREA_INICIO_Y;
        int screenWidth = AREA_FIM_X - AREA_INICIO_X + 1;
        int messageWidth = strlen(vitoriaMessage[0]);
        int startX = (screenWidth - messageWidth) / 2 + AREA_INICIO_X;

        for (int i = 0; i < messageLines; i++) {
            screenGotoxy(startX, startY + i);
            printf("%s", vitoriaMessage[i]);
        }
        screenUpdate();
        sleep(5);

        screenClear();
        screenUpdate();

        const char *nomePrompt = "Digite seu nome para salvar a pontuação: ";
        int nomePromptLength = strlen(nomePrompt);
        int nomeStartX = (screenWidth - nomePromptLength) / 2 + AREA_INICIO_X;
        int nomeStartY = startY;

        screenGotoxy(nomeStartX, nomeStartY);
        printf("%s", nomePrompt);
        screenUpdate();

        char nome[50] = "";
        int i = 0;
        int ch;
        while (1) {
            if (keyhit()) {
                ch = readch();

                if (ch == '\n' || ch == '\r') {
                    break;
                } else if (ch == '\b' || ch == 127 || ch == 8) {
                    if (i > 0) {
                        i--;
                        nome[i] = '\0';
                        printf("\b \b");
                        fflush(stdout);
                    }
                } else if (ch >= 32 && ch <= 126 && i < sizeof(nome) - 1) {
                    nome[i++] = ch;
                    nome[i] = '\0';
                    printf("%c", ch);
                    fflush(stdout);
                }
                screenUpdate();
            }
        }
        nome[i] = '\0';

        adicionarPontuacao(&pontuacoes, nome, pontuacaoAtual);
        salvarPontuacao(nome, pontuacaoAtual);
        printf("\nPontuação salva com sucesso!\n");
        screenUpdate();
        sleep(2);

        mostrarMenuPrincipal(pontuacoes);
        screenUpdate();

    } else {
        morte(pontuacoes, pontuacaoAtual);
    }
}

void morte(No *pontuacoes, int pontuacaoAtual) {
    screenSetColor(BLACK, DARKGRAY);
    screenClear();

    const char *gameOverMessage[] = {
        "╔═════════════════════════════════════════════════════════╗",
        "║                                                         ║",
        "║          A última centelha se apagou no vazio...        ║",
        "║                                                         ║",
        "║   'Mesmo nas trevas, a luz pode renascer.'              ║",
        "║                                                         ║",
        "║        Tente novamente, não desista.                    ║",
        "║                                                         ║",
        "╚═════════════════════════════════════════════════════════╝"
    };

    int messageLines = sizeof(gameOverMessage) / sizeof(gameOverMessage[0]);
    int startY = (AREA_FIM_Y - AREA_INICIO_Y + 1 - messageLines) / 2 + AREA_INICIO_Y;
    int screenWidth = AREA_FIM_X - AREA_INICIO_X + 1;
    int messageWidth = strlen(gameOverMessage[0]);
    int startX = (screenWidth - messageWidth) / 2 + AREA_INICIO_X;

    for (int i = 0; i < messageLines; i++) {
        screenGotoxy(startX, startY + i);
        printf("%s", gameOverMessage[i]);
    }

    screenUpdate();
    sleep(3);

    screenClear();
    screenUpdate();

    const char *nomePrompt = "Digite seu nome para salvar a pontuação: ";
    int nomePromptLength = strlen(nomePrompt);
    int nomeStartX = (screenWidth - nomePromptLength) / 2 + AREA_INICIO_X;
    int nomeStartY = startY;

    screenGotoxy(nomeStartX, nomeStartY);
    printf("%s", nomePrompt);
    screenUpdate();

    char nome[50] = "";
    int i = 0;
    int ch;
    while (1) {
        if (keyhit()) {
            ch = readch();

            if (ch == '\n' || ch == '\r') {
                break;
            } else if (ch == '\b' || ch == 127 || ch == 8) {
                if (i > 0) {
                    i--;
                    nome[i] = '\0';
                    printf("\b \b");
                    fflush(stdout);
                }
            } else if (ch >= 32 && ch <= 126 && i < sizeof(nome) - 1) {
                nome[i++] = ch;
                nome[i] = '\0';
                printf("%c", ch);
                fflush(stdout);
            }
            screenUpdate();
        }
    }
    nome[i] = '\0';

    adicionarPontuacao(&pontuacoes, nome, pontuacaoAtual);
    salvarPontuacao(nome, pontuacaoAtual);
    printf("\nPontuação salva com sucesso!\n");
    screenUpdate();
    sleep(2);

    mostrarMenuPrincipal(pontuacoes);
    screenUpdate();
}

// Funções relacionadas às pontuações

No* criarNo(char *nome, int pontuacao) {
    No *novoNo = (No*)malloc(sizeof(No));
    strncpy(novoNo->nome, nome, 50);
    novoNo->pontuacao = pontuacao;
    novoNo->proximo = NULL;
    return novoNo;
}

void adicionarPontuacao(No **cabeca, char *nome, int pontuacao) {
    No *novoNo = criarNo(nome, pontuacao);
    novoNo->proximo = *cabeca;
    *cabeca = novoNo;

    if (pontuacao > maiorPontuacaoGlobal) {
        maiorPontuacaoGlobal = pontuacao;
        strncpy(nomeMaiorPontuacao, nome, 50);
        salvarMaiorPontuacao();
    }
}

void salvarPontuacao(char *nome, int pontuacao) {
    FILE *arquivo = fopen("scoreleaders.txt", "a");
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo de pontuações.\n");
        return;
    }

    fprintf(arquivo, "%s %d\n", nome, pontuacao);
    fclose(arquivo);
}

void salvarMaiorPontuacao() {
    FILE *file = fopen("score.dat", "w");
    if (file != NULL) {
        fprintf(file, "%d\n%s", maiorPontuacaoGlobal, nomeMaiorPontuacao);
        fclose(file);
    }
}

void carregarMaiorPontuacao() {
    FILE *file = fopen("score.dat", "r");
    if (file != NULL) {
        fscanf(file, "%d\n", &maiorPontuacaoGlobal);
        fgets(nomeMaiorPontuacao, 50, file);
        nomeMaiorPontuacao[strcspn(nomeMaiorPontuacao, "\n")] = '\0';
        fclose(file);
    }
}

// Função principal

int main() {
    No *pontuacoes = NULL;
    carregarMaiorPontuacao();
    int ch = 0;
    screenInit(1);
    keyboardInit();
    timerInit(50);
    srand(time(NULL));

    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        fprintf(stderr, "Erro ao inicializar SDL: %s\n", SDL_GetError());
        return 1;
    }
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096) < 0) {
        fprintf(stderr, "Erro ao inicializar SDL_mixer: %s\n", Mix_GetError());
        return 1;
    }

    mostrarMenuPrincipal(pontuacoes);
    screenUpdate();

    while (jogoEmExecucao) {
        if (keyhit()) {
            ch = readch();
            if (ch == '1') {
                mostrarInstrucoes();
                Mix_Music *bgMusic = Mix_LoadMUS("background_music.mp3");
                if (!bgMusic) {
                    fprintf(stderr, "Erro ao carregar música: %s\n", Mix_GetError());
                    return 1;
                }
                Mix_VolumeMusic(32);
                Mix_PlayMusic(bgMusic, -1);
                screenClear();
                iniciarJogo(bgMusic, pontuacoes);
                mostrarMenuPrincipal(pontuacoes);
            } else if (ch == '2') {
                mostrarCreditos();
                mostrarMenuPrincipal(pontuacoes);
            } else if (ch == '3') {
                mostrarLore();
                mostrarMenuPrincipal(pontuacoes);
            } else if (ch == '4') {
                jogoEmExecucao = 0;
            } else if (ch == '5') {
                mostrarRanking();
                mostrarMenuPrincipal(pontuacoes);
            }
            screenUpdate();
        }
    }

    Mix_CloseAudio();
    SDL_Quit();
    keyboardDestroy();
    screenDestroy();
    timerDestroy();
    return 0;
}
