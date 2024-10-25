#include <string.h>
#include <unistd.h>  // Adicionado para usar sleep
#include "screen.h"
#include "keyboard.h"
#include "timer.h"

// Variáveis de controle
int menuOption = 0;
int gameRunning = 1;
int x = 34, y = 12;  // Posição inicial do texto animado
int heartAnimationFrame = 0;

// Funções de exibição de telas
void showMainMenu();
void showCredits();
void animateHeart();

// Função para mostrar o menu principal
void showMainMenu() {
    screenSetColor(RED, DARKGRAY); // Cor vermelha para o nome "HEART"
    screenClear();
    animateHeart(); // Mostra o coração no topo

    // Centralizando o nome HEART em ASCII grande
    screenGotoxy(20, 12);  // Movido o nome mais para baixo para garantir que o coração não sobreponha
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

    // Posicionando o menu abaixo do nome "HEART" e coração, com a cor roxa
    screenSetColor(LIGHTMAGENTA, DARKGRAY); // Cor roxa para as opções do menu
    screenGotoxy(33, 22);
    printf("1. Iniciar");
    screenGotoxy(33, 23);
    printf("2. Creditos");
    screenGotoxy(33, 24);
    printf("3. Sair");
}

// Função para mostrar a tela de créditos
void showCredits() {
    screenSetColor(WINE_RED, DARKGRAY); // Usando a nova cor vinho para a tela de créditos
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

// Função para desenhar o coração com a cor vermelha
void animateHeart() {
    screenSetColor(RED, DARKGRAY);  // Cor vermelha para o coração
    screenGotoxy(30, 3);  // Movido o coração para garantir que ele fique acima do nome "HEART"
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

// Função principal do jogo
int main() 
{
    int ch = 0;

    screenInit(1); // Inicializa a tela
    keyboardInit();
    timerInit(50);

    showMainMenu();
    screenUpdate();

    while (gameRunning) {
        if (keyhit()) {
            ch = readch();
            if (ch == '1') {
                // Aqui você implementaria a lógica de iniciar o jogo
                screenClear();
                screenGotoxy(35, 12);
                printf("Iniciando o jogo...");
                screenUpdate();
                sleep(2);
                showMainMenu();
            } else if (ch == '2') {
                showCredits();
                screenUpdate();
                readch(); // Aguarda o usuário pressionar uma tecla
                showMainMenu();
            } else if (ch == '3') {
                gameRunning = 0; // Sai do jogo
            }
            screenUpdate();
        }

        // Animação do coração no menu principal
        if (timerTimeOver() == 1) {
            animateHeart();
            screenUpdate();
        }
    }

    keyboardDestroy();
    screenDestroy();
    timerDestroy();

    return 0;
}