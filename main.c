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

    #define AREA_INICIO_X 10
    #define AREA_FIM_X 70
    #define AREA_INICIO_Y 15
    #define AREA_FIM_Y 25
    #define PONTOS_FASE2 200
    #define PONTOS_FASE3 400
    #define PONTOS_FIM_JOGO 600

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


    typedef struct No {
        int pontuacao;
        struct No *proximo;
    } No;

    typedef struct {
        int x, y;
        int active;
        int comprimento;
    } ObstaculoMagenta;

    #define MAX_OBSTACULOS_MAGENTA 5
    ObstaculoMagenta obstaculosMagenta[MAX_OBSTACULOS_MAGENTA];

    typedef struct {
        int x, y;
        int active;
        int comprimento;
    } ObstaculoOsso;

    #define MAX_OBSTACULOS 10
    ObstaculoOsso ossos[MAX_OBSTACULOS];
    int contadorObstaculos = 0;

    // Novo tipo de obstáculo amarelo
    typedef struct {
        int x, y;
        int active;
        int comprimento;
    } ObstaculoAmarelo;

    #define MAX_OBSTACULOS_AMARELOS 2
    ObstaculoAmarelo obstaculosAmarelos[MAX_OBSTACULOS_AMARELOS];

    // Declarações de funções
    void animarCoracao();
    void mostrarMenuPrincipal(No *pontuacoes);
    void mostrarCreditos();
    void desenharAreaComCoracao();
    void desenharOssos();
    void atualizarOssos();
    void iniciarJogo(Mix_Music *bgMusic, No *pontuacoes);
    void salvarPontuacao(char *nome, int pontuacao);
    void finalizarJogo(No *pontuacoes, int pontuacaoAtual);
    void morte(No *pontuacoes, int pontuacaoAtual);
    void moverCoracao(int upPressed, int downPressed, int leftPressed, int rightPressed);
    void gerarObstaculos();
    int detectarColisao();
    void atualizarStatus();
    void desenharPersonagemASCII();
    void limparAreaJogo();
    void mudarParaSegundaFase();
    void desenharBossFixo();
    void mudarParaTerceiraFase();
    void atualizarObstaculosMagenta();
    void gerarObstaculosMagenta();
    void gerarObstaculosAmarelos();
    void atualizarObstaculosAmarelos();
    void salvarMaiorPontuacao();
    void desenharBossTerceiraFase();



    No* criarNo(int pontuacao) {
        No *novoNo = (No*)malloc(sizeof(No));
        novoNo->pontuacao = pontuacao;
        novoNo->proximo = NULL;
        return novoNo;
    }

    void adicionarPontuacao(No **cabeca, int pontuacao) {
    No *novoNo = criarNo(pontuacao);
    novoNo->proximo = *cabeca;
    *cabeca = novoNo;

    // Atualiza a maior pontuação global se a pontuação atual for maior
    if (pontuacao > maiorPontuacaoGlobal) {
        maiorPontuacaoGlobal = pontuacao;
        salvarMaiorPontuacao();
    }
}

    int obterMaiorPontuacao(No *cabeca) {
        int maiorPontuacao = 0;
        No *atual = cabeca;
        while (atual != NULL) {
            if (atual->pontuacao > maiorPontuacao) {
                maiorPontuacao = atual->pontuacao;
            }
            atual = atual->proximo;
        }
        return maiorPontuacao;
    }

        void salvarMaiorPontuacao() {
        FILE *file = fopen("score.dat", "w");
        if (file != NULL) {
            fprintf(file, "%d", maiorPontuacaoGlobal);
            fclose(file);
        }
    }

        void mostrarMaiorPontuacao() {
        screenSetColor(YELLOW, DARKGRAY);
        screenGotoxy(33, 25);
        printf("Maior Pontuação: %d", maiorPontuacaoGlobal);
    }

        void carregarMaiorPontuacao() {
        FILE *file = fopen("score.dat", "r");
        if (file != NULL) {
            fscanf(file, "%d", &maiorPontuacaoGlobal);
            fclose(file);
        }
    }

void mostrarLore() {
    // Configura as cores de fundo e texto
    screenSetColor(BLACK, DARKGRAY);
    screenClear();

    // Texto do lore com bordas em ASCII
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
    
    // Calcula posições centralizadas
    int startY = (AREA_FIM_Y - AREA_INICIO_Y + 1 - loreLines) / 2 + AREA_INICIO_Y;
    int screenWidth = AREA_FIM_X - AREA_INICIO_X + 1;
    int loreWidth = strlen(loreText[0]);
    int startX = (screenWidth - loreWidth) / 2 + AREA_INICIO_X;

    // Renderiza o texto linha por linha
    for (int i = 0; i < loreLines; i++) {
        screenGotoxy(startX, startY + i);
        printf("%s", loreText[i]);
    }

    // Exibe o aviso de "Pressione qualquer tecla para continuar" abaixo do texto principal
    screenGotoxy(startX, startY + loreLines + 1); // Posiciona logo abaixo do lore
    printf("%s", pressKeyText);

    // Atualiza a tela e espera a entrada do usuário
    screenUpdate();
    readch(); // Aguarda o usuário pressionar uma tecla para continuar
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
    screenGotoxy(20, 19);
    printf(" ░  ░░ ░   ░    ░   ▒     ░░   ░   ░             ░ ");

    screenSetColor(LIGHTMAGENTA, DARKGRAY);
    screenGotoxy(33, 20);
    printf("1. Iniciar");
    screenGotoxy(33, 21);
    printf("2. Créditos");
    screenGotoxy(33, 22);
    printf("3. Lore");
    screenGotoxy(33, 23);
    printf("4. Sair");
    screenGotoxy(33, 24);
    mostrarMaiorPontuacao(pontuacoes);
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

// Função para exibir as instruções do jogo
    void mostrarInstrucoes() {
        printf("\n\nInstruções do Jogo\n\n");
        printf("Movimento: Utilize as teclas a seguir para controlar seu personagem:\n\n");
        printf("W - Move para cima\n");
        printf("S - Move para baixo\n");
        printf("A - Move para a esquerda\n");
        printf("D - Move para a direita\n\n");
        printf("Objetivo:\n\n");
        printf("Desviar das estruturas brancas a todo custo, pois entrar em contato com elas, o coração levará dano.\n");
        printf("Estruturas roxas podem ser atravessadas, mas somente se você permanecer imóvel ao passar por elas.\n");
        printf("Boa sorte e divirta-se ao enfrentar esses desafios!\n\n");
        printf("Pressione qualquer tecla para iniciar o jogo...\n");
        getchar(); // Aguarda o jogador pressionar uma tecla
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

    void atualizarStatus() {
        screenSetColor(RED, BLACK);
        screenGotoxy(AREA_INICIO_X, AREA_FIM_Y + 2);
        printf("HP: %d", health);
        screenGotoxy(AREA_FIM_X - 10, AREA_FIM_Y + 2);
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

    int detectarColisao() {
        // Verificar colisões com Obstáculos de Osso
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

        // Verificar colisões com Obstáculos Roxos
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

        // Verificar colisões com Obstáculos Amarelos
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

    void limparAreaJogo() {
        for (int y = AREA_INICIO_Y + 1; y <= AREA_FIM_Y - 1; y++) {
            for (int x = AREA_INICIO_X + 1; x <= AREA_FIM_X - 1; x++) {
                screenGotoxy(x, y);
                printf(" ");
            }
        }
    }

    void gerarObstaculosAmarelos() {
        static int contadorAmarelos = 0;
        contadorAmarelos++;
        int frequencia = 150; // Aumenta o espaçamento entre os obstáculos amarelos

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

    void iniciarJogo(Mix_Music *bgMusic, No *pontuacoes) {
        health = 200;
        coracaoX = AREA_INICIO_X + 3;
        coracaoY = AREA_FIM_Y - 2;
        score = 0;
        faseAtual = 1;
        // Inicializar obstáculos
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
                finalizarJogo(pontuacoes, score);
                break;
            }
            // Controle de movimento do jogador
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
            // Limpa a área do jogo primeiro
            limparAreaJogo();
            // Atualiza o estado do jogo
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
            // Atualiza a pontuação e a tela
            screenUpdate();
            score++;
            usleep(25000);
        }
        // Parar a música e liberar recursos após o término do jogo
        Mix_HaltMusic();
        Mix_FreeMusic(bgMusic);
        finalizarJogo(pontuacoes, score);
    }


void morte(No *pontuacoes, int pontuacaoAtual) {
    adicionarPontuacao(&pontuacoes, pontuacaoAtual);
    salvarMaiorPontuacao(pontuacoes);
    mostrarMaiorPontuacao(pontuacoes);
    screenUpdate();

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
    sleep(3); // Pausa de 3 segundos para exibir a mensagem de "Game Over"
    
    // Limpa a tela antes de solicitar o nome do jogador
    screenClear();
    screenUpdate();

        // Centraliza e exibe a mensagem de solicitação do nome
    const char *nomePrompt = "Digite seu nome para salvar a pontuação: ";
    int nomePromptLength = strlen(nomePrompt);
    int nomeStartX = (screenWidth - nomePromptLength) / 2 + AREA_INICIO_X;
    int nomeStartY = startY;

    screenGotoxy(nomeStartX, nomeStartY);
    printf("%s", nomePrompt);
    screenUpdate();

        // Captura e exibe o nome caractere por caractere
        char nome[50];
        int i = 0;
        int ch;
        while (i < sizeof(nome) - 1) {
            ch = getchar(); // Captura um caractere do terminal

            if (ch == '\n') { // Enter para finalizar
                break;
            } else if (ch == 127 || ch == '\b') { // Backspace
                if (i > 0) {
                    i--;
                    nome[i] = '\0';
                    screenGotoxy(nomeStartX + nomePromptLength + i, nomeStartY);
                    printf(" "); // Apaga o último caractere da tela
                    screenGotoxy(nomeStartX + nomePromptLength + i, nomeStartY);
                    screenUpdate();
                }
            } else if (ch >= 32 && ch <= 126) { // Caracteres imprimíveis
                nome[i++] = ch;
                nome[i] = '\0';
                printf("%c", ch); // Exibe o caractere digitado
                screenUpdate();
            }
        }
        nome[i] = '\0'; // Termina a string

        // Salva a pontuação e exibe uma confirmação única
        salvarPontuacao(nome, pontuacaoAtual);
        printf("\nPontuação salva com sucesso!\n");
        screenUpdate();
        // Exibe o menu principal apenas uma vez
        mostrarMenuPrincipal(pontuacoes);
        screenUpdate();

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

    void desenharBossTerceiraFase() {
        int startX = (AREA_FIM_X + AREA_INICIO_X) / 2 - 16;  // Centraliza a largura de 48 caracteres
        int startY = AREA_INICIO_Y - 15;  // Posição inicial da altura do boss

        screenSetColor(DARKGRAY, BLACK);

        // ASCII Art do Boss (48 caracteres de largura por 14 de altura)
        screenGotoxy(startX, startY);
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
        printf("⠀⠻⣆⠀⠀⢿⡀⠀⠀⡇⢀⣼⣿⣿⣿⣷⣾⣿⣿⣿⣿⣿⣷⣾⣿⣿⣿⣿⣿⣿⣿⣿⠿⠟⠉⠀⣉⣷⣿⣷⠟⠁⠀⠀⠀⠀⠀⠀⠀");
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


    void mudarParaTerceiraFase() {
        Mix_PauseMusic();
        screenSetColor(YELLOW, DARKGRAY);
        screenClear();

        velocidadeObstaculos = 10;
        Mix_ResumeMusic();

        // Limita o número de obstáculos roxos ativos
        for (int i = 0; i < MAX_OBSTACULOS_MAGENTA; i++) {
            obstaculosMagenta[i].active = 0;
        }

        desenharBossTerceiraFase();

        screenSetColor(YELLOW, DARKGRAY);
        screenGotoxy(AREA_INICIO_X, AREA_INICIO_Y - 2);
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

    void salvarPontuacao(char *nome, int pontuacao) {
        FILE *arquivo = fopen("scoreleaders.txt", "a"); // Abre o arquivo em modo de adição
        if (arquivo == NULL) {
            printf("Erro ao abrir o arquivo de pontuações.\n");
            return;
        }

        fprintf(arquivo, "Nome: %s | Pontuação: %d\n", nome, pontuacao); // Escreve o nome e a pontuação
        fclose(arquivo);
    }


void finalizarJogo(No *pontuacoes, int pontuacaoAtual) {
    // Adiciona pontuação e salva o maior valor
    adicionarPontuacao(&pontuacoes, pontuacaoAtual);
    salvarMaiorPontuacao(pontuacoes);

    // Define cor e limpa a tela
    screenSetColor(RED, BLACK);
    screenClear();

    if (pontuacaoAtual >= PONTOS_FIM_JOGO) { // Caso de vitória
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
        sleep(5); // Pausa de 5 segundos para exibir a mensagem de vitória

        // Limpa a tela antes de solicitar o nome do jogador
        screenClear();
        screenUpdate();

        // Centraliza e exibe a mensagem de solicitação do nome
        const char *nomePrompt = "Digite seu nome para salvar a pontuação: ";
        int nomePromptLength = strlen(nomePrompt);
        int nomeStartX = (screenWidth - nomePromptLength) / 2 + AREA_INICIO_X;
        int nomeStartY = startY;

        screenGotoxy(nomeStartX, nomeStartY);
        printf("%s", nomePrompt);
        screenUpdate();

        // Captura e exibe o nome caractere por caractere
        char nome[50];
        int i = 0;
        int ch;
        while (i < sizeof(nome) - 1) {
            ch = getchar(); // Captura um caractere do terminal

            if (ch == '\n') { // Enter para finalizar
                break;
            } else if (ch == 127 || ch == '\b') { // Backspace
                if (i > 0) {
                    i--;
                    nome[i] = '\0';
                    screenGotoxy(nomeStartX + nomePromptLength + i, nomeStartY);
                    printf(" "); // Apaga o último caractere da tela
                    screenGotoxy(nomeStartX + nomePromptLength + i, nomeStartY);
                    screenUpdate();
                }
            } else if (ch >= 32 && ch <= 126) { // Caracteres imprimíveis
                nome[i++] = ch;
                nome[i] = '\0';
                printf("%c", ch); // Exibe o caractere digitado
                screenUpdate();
            }
        }
        nome[i] = '\0'; // Termina a string

        // Salva a pontuação e exibe uma confirmação única
        salvarPontuacao(nome, pontuacaoAtual);
        printf("\nPontuação salva com sucesso!\n");
        screenUpdate();
        // Exibe o menu principal apenas uma vez
        mostrarMenuPrincipal(pontuacoes);
        screenUpdate();

    } else { // Caso de derrota, chama a função 'morte' apenas uma vez
        morte(pontuacoes, pontuacaoAtual);
    }
}


int main() {
    No *pontuacoes = NULL;
    carregarMaiorPontuacao(&pontuacoes);
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
                screenUpdate();
                readch();
                mostrarMenuPrincipal(pontuacoes);
            } else if (ch == '3') {
                mostrarLore();
                screenUpdate();
                readch();
                mostrarMenuPrincipal(pontuacoes);
            } else if (ch == '4') {
                jogoEmExecucao = 0;
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