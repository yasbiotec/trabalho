#include "raylib.h"

#define LARGURA_TELA 800
#define ALTURA_TELA 800
#define inicial_cobra 20 //cobra inicialmente
#define MAX_COMPRIMENTO 100 //maximo de comprimento da cobra

typedef struct {
    int x;
    int y;
} Vetor2D;

typedef struct {
    Vetor2D posicoes[MAX_COMPRIMENTO];
    int comprimento;
    int direcao;
} Cobra;

Cobra cobra;

void IniciarJogo();
void AtualizarJogo();
void DesenharJogo();

int main() {
    InitWindow(LARGURA_TELA, ALTURA_TELA, "Snake");
    SetTargetFPS(10);
    IniciarJogo();

    while (!WindowShouldClose()) {
        AtualizarJogo();
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DesenharJogo();
        EndDrawing();
    }

    CloseWindow();
    return 0;
}

void IniciarJogo() {
    cobra.comprimento = 1;
    cobra.direcao = 4;  // Direita
    cobra.posicoes[0].x = LARGURA_TELA / (2 * inicial_cobra);
    cobra.posicoes[0].y = ALTURA_TELA / (2 * inicial_cobra);
}

void AtualizarJogo() {
    if (IsKeyPressed(KEY_RIGHT) && cobra.direcao != 3) cobra.direcao = 4;
    if (IsKeyPressed(KEY_LEFT) && cobra.direcao != 4) cobra.direcao = 3;
    if (IsKeyPressed(KEY_UP) && cobra.direcao != 2) cobra.direcao = 1;
    if (IsKeyPressed(KEY_DOWN) && cobra.direcao != 1) cobra.direcao = 2;

    // Atualizar posição da cobra
    for (int i = cobra.comprimento - 1; i > 0; i--) {
        cobra.posicoes[i] = cobra.posicoes[i - 1];
    }

    if (cobra.direcao == 4) cobra.posicoes[0].x += 1;
    if (cobra.direcao == 3) cobra.posicoes[0].x -= 1;
    if (cobra.direcao == 1) cobra.posicoes[0].y -= 1;
    if (cobra.direcao == 2) cobra.posicoes[0].y += 1;


    if (cobra.posicoes[0].x >= LARGURA_TELA / inicial_cobra) cobra.posicoes[0].x = 0;
    if (cobra.posicoes[0].x < 0) cobra.posicoes[0].x = LARGURA_TELA / inicial_cobra - 1;
    if (cobra.posicoes[0].y >= ALTURA_TELA / inicial_cobra) cobra.posicoes[0].y = 0;
    if (cobra.posicoes[0].y < 0) cobra.posicoes[0].y = ALTURA_TELA / inicial_cobra - 1;
}

void DesenharJogo() {
    // Desenhar a cobra
    for (int i = 0; i < cobra.comprimento; i++) {
        DrawRectangle(cobra.posicoes[i].x * inicial_cobra, cobra.posicoes[i].y * inicial_cobra, inicial_cobra, inicial_cobra, DARKGREEN);
    }
}

/*/////IGNORA QUE ISSO AQ FOI MINHA PRATICA QUE EU TO BASENADO
int podeMover(int x, int y, int dx, int dy, int largura, int altura) {
    int posicao_x = x + dx * TAMANHO;
    int posicao_y = y + dy * TAMANHO;

    if (posicao_x >=0 && posicao_x +TAMANHO<= largura &&posicao_y >=0 && posicao_y +TAMANHO<= altura ){
        return 1;
    }
    else{
        return 0;
    }
}
int main(){
    int altura=ALTURA;
    int largura=LARGURA;
    char texto[10] = "Pressione uma seta"; //Texto inicial
    srand(time(NULL));
    int posicao_x=rand()% (LARGURA + TAMANHO), posicao_y=rand()% (ALTURA - TAMANHO); //posicao do quadrado,rand o coloca com uma posicao aleatoria nos limites da largura e altura.

    InitWindow(LARGURA, ALTURA, "Snake"); //Inicializa janela, com certo tamanho e titulo
    SetTargetFPS(10);// Ajusta a janela para 60 frames por segundo

     // Loop principal do jogo
    while (!WindowShouldClose()) {
        int dx = 0, dy = 0; //posicoes para ver se podem ou não se mover
        if (IsKeyPressed(KEY_RIGHT)) dx = 1;
        if (IsKeyPressed(KEY_LEFT)) dx = -1;
        if (IsKeyPressed(KEY_UP)) dy = -1;
        if (IsKeyPressed(KEY_DOWN)) dy = 1;

        if (podeMover(posicao_x, posicao_y, dx, dy, largura, altura)) {
        posicao_x += dx * TAMANHO;
        posicao_y += dy * TAMANHO;
        }

	// Atualiza o que eh mostrado na tela a partir do estado do jogo
	BeginDrawing(); //Inicia o ambiente de desenho na tela
	ClearBackground(RAYWHITE); //Limpa a tela e define cor de fundo
	DrawText(texto, 50, 150, 50, BLUE); //Desenha um texto, com posicao, tamanho e cor
	DrawRectangle(posicao_x, posicao_y, TAMANHO, TAMANHO, GREEN); //desenha o quadrado
	EndDrawing(); //Finaliza o ambiente de desenho na tela
    }

    CloseWindow(); // Fecha a janela
    return 0;
}
