#include "raylib.h"

#define Cobra_tamanho   100 //MAXIMO DO TAMANHO DA COBRA (quebra quando acaba)
#define CEDULAS     30 //TAMANHO DAS CEDULAS
//TAMANHO DA TELA:
#define ALTURA 800
#define LARGURA 800

//----------------------------------------------------------------------------------
// TIPOS E ESTRUTURA DO JOGO:
//----------------------------------------------------------------------------------
typedef struct Snake {
    Vector2 posicao;
    Vector2 tamanho;
    Vector2 speed;
    Color color;
} Snake;

typedef struct Food {
    Vector2 posicao;
    Vector2 tamanho;
    bool active;
    Color color;
} Food;

//------------------------------------------------------------------------------------
// VARIAVEIS GLOBAIS
//------------------------------------------------------------------------------------

static int framesCounter = 0;
static bool gameOver = false;
static bool pause = false;
static Food fruit = { 0 };
static Snake snake[Cobra_tamanho] = { 0 };
static Vector2 snakePosition[Cobra_tamanho] = { 0 };
static bool allowMove = false;
static Vector2 offset = { 0 };
static int counterTail = 0;
static int pontos=0;
static int recorde=0;

//------------------------------------------------------------------------------------
//Declaração de funções locais
//------------------------------------------------------------------------------------
static void InitGame(void);         // Iniciando o jogo
static void UpdateGame(void);       // atualizando o jogo
static void DrawGame(void);         // Desenhando o jogo
static void UnloadGame(void);       // Unload game
static void UpdateDrawFrame(void);  // Atualizando e desenhando

//------------------------------------------------------------------------------------
// Main do jogo
//------------------------------------------------------------------------------------
int main(void)
{
    // Initialization (Note windowTitle is unused on Android)
    //---------------------------------------------------------
    InitWindow(ALTURA, LARGURA, "Trabalho final: Snake");

    InitGame();
    SetTargetFPS(60);
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())    //Enquanto a janela do game não fechar...
    {
        // atualiza e desenha
        //----------------------------------------------------------------------------------
        UpdateDrawFrame();
        //----------------------------------------------------------------------------------
    }
    // De-Initialization
    //--------------------------------------------------------------------------------------
    UnloadGame();         // Unload loaded data (textures, sounds, models...)

    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}

//iniciacao do game
void InitGame(void)
{
    framesCounter = 0;
    gameOver = false;
    pause = false;

    counterTail = 1;
    allowMove = false;

    offset.x = ALTURA%CEDULAS;
    offset.y = LARGURA%CEDULAS;

    for (int i = 0; i < Cobra_tamanho; i++)
    {
        snake[i].posicao = (Vector2){ offset.x/2, offset.y/2 };
        snake[i].tamanho = (Vector2){ CEDULAS,CEDULAS };
        snake[i].speed = (Vector2){ CEDULAS, 0 };

        if (i == 0) snake[i].color = DARKBLUE;
        else snake[i].color = BLUE;
    }

    for (int i = 0; i < Cobra_tamanho; i++)
    {
        snakePosition[i] = (Vector2){ 0.0f, 0.0f };
    }

    fruit.tamanho = (Vector2){CEDULAS,CEDULAS };
    fruit.color = SKYBLUE;
    fruit.active = false;
}

// Update game (one frame)
void UpdateGame(void)
{
    if (!gameOver)
    {
        if (IsKeyPressed(KEY_TAB)) pause = !pause;

        if (!pause)
        {
            // Controle do jogo 
            if (IsKeyPressed(KEY_RIGHT)||IsKeyPressed(KEY_D) && (snake[0].speed.x == 0) && allowMove)
            {
                snake[0].speed = (Vector2){ CEDULAS, 0 };
                allowMove = false;
            }
            if (IsKeyPressed(KEY_LEFT)||IsKeyPressed(KEY_A) && (snake[0].speed.x == 0) && allowMove)
            {
                snake[0].speed = (Vector2){ -CEDULAS, 0 };
                allowMove = false;
            }
            if (IsKeyPressed(KEY_UP)||IsKeyPressed(KEY_W) && (snake[0].speed.y == 0) && allowMove)
            {
                snake[0].speed = (Vector2){ 0, -CEDULAS };
                allowMove = false;
            }
            if (IsKeyPressed(KEY_DOWN)||IsKeyPressed(KEY_S) && (snake[0].speed.y == 0) && allowMove)
            {
                snake[0].speed = (Vector2){ 0, CEDULAS };
                allowMove = false;
            }

            // Movimentacao da Snake
            for (int i = 0; i < counterTail; i++) snakePosition[i] = snake[i].posicao;

            if ((framesCounter%5) == 0)
            {
                for (int i = 0; i < counterTail; i++)
                {
                    if (i == 0)
                    {
                        snake[0].posicao.x += snake[0].speed.x;
                        snake[0].posicao.y += snake[0].speed.y;
                        allowMove = true;
                    }
                    else snake[i].posicao = snakePosition[i-1];
                }
            }

            // Wall behaviour
            if (((snake[0].posicao.x) > (ALTURA - offset.x)) ||
                ((snake[0].posicao.y) > (LARGURA - offset.y)) ||
                (snake[0].posicao.x < 0) || (snake[0].posicao.y < 0))
            {
                gameOver = true;
            }

            // Se ocorre as colisao
            for (int i = 1; i < counterTail; i++)
            {
                if ((snake[0].posicao.x == snake[i].posicao.x) && (snake[0].posicao.y == snake[i].posicao.y)) gameOver = true;
            }

            // Calculo da posicao da fruta
            if (!fruit.active)
            {
                fruit.active = true;
                fruit.posicao = (Vector2){ GetRandomValue(0, (ALTURA/CEDULAS) - 1)*CEDULAS + offset.x/2, GetRandomValue(0, (LARGURA/CEDULAS) - 1)*CEDULAS + offset.y/2 };

                for (int i = 0; i < counterTail; i++)
                {
                    while ((fruit.posicao.x == snake[i].posicao.x) && (fruit.posicao.y == snake[i].posicao.y))
                    {
                        fruit.posicao = (Vector2){ GetRandomValue(0, (ALTURA/CEDULAS) - 1)*CEDULAS + offset.x/2, GetRandomValue(0, (LARGURA/CEDULAS) - 1)*CEDULAS + offset.y/2 };
                        i = 0;
                    }
                }
            }

            // Collision
            if ((snake[0].posicao.x < (fruit.posicao.x + fruit.tamanho.x) && (snake[0].posicao.x + snake[0].tamanho.x) > fruit.posicao.x) &&
                (snake[0].posicao.y < (fruit.posicao.y + fruit.tamanho.y) && (snake[0].posicao.y + snake[0].tamanho.y) > fruit.posicao.y))
            {
                snake[counterTail].posicao = snakePosition[counterTail - 1];
                counterTail += 1;
                fruit.active = false;
                pontos+=10;
            }

            framesCounter++;
        }
    }
    else
    {
        if (IsKeyPressed(KEY_ENTER))
        {
            InitGame();
            gameOver = false;
        }
    }
}

// Draw game (one frame)
void DrawGame(void)
{
    BeginDrawing();

        ClearBackground(RAYWHITE);

        if (!gameOver)
        {
            //Desenha as linhas
            for (int i = 0; i < ALTURA/CEDULAS + 1; i++)
            {
                DrawLineV((Vector2){CEDULAS*i + offset.x/2, offset.y/2}, (Vector2){CEDULAS*i + offset.x/2, LARGURA - offset.y/2}, LIGHTGRAY);
            }

            for (int i = 0; i < LARGURA/CEDULAS + 1; i++)
            {
                DrawLineV((Vector2){offset.x/2, CEDULAS*i + offset.y/2}, (Vector2){ALTURA - offset.x/2, CEDULAS*i + offset.y/2}, LIGHTGRAY);
            }

            // desenha snake
            for (int i = 0; i < counterTail; i++) DrawRectangleV(snake[i].posicao, snake[i].tamanho, snake[i].color);

            // desenha a fruta
            DrawRectangleV(fruit.posicao, fruit.tamanho, fruit.color);

            if (pause){
                DrawText("GAME PAUSED", ALTURA/2 - MeasureText("GAME PAUSED", 40)/2, LARGURA/2 - 40, 40, GRAY);
                DrawText(TextFormat("PONTOS: %d", pontos), ALTURA / 2 - MeasureText(TextFormat("PONTOS: %d", pontos), 20) / 2, LARGURA / 2, 20, GRAY);
        }
        }
        else{
            DrawText("PRESS [ENTER] TO PLAY AGAIN", GetScreenWidth()/2 - MeasureText("PRESS [ENTER] TO PLAY AGAIN", 20)/2, GetScreenHeight()/2 - 50, 20, GRAY);
            if(pontos>recorde){ //Se os pontos forem maiores que o recorde, substitui o recorde pela pontuação atual
                recorde=pontos;
                pontos=0; //zera a pontuação para o proximo 
            }
            DrawText(TextFormat("RECORDE: %d", recorde), ALTURA / 2 - MeasureText(TextFormat("RECORDE: %d", recorde), 20) / 2, LARGURA / 2, 20, GRAY);

        }

    EndDrawing();
}

// da base ai
void UnloadGame(void)
{
    // TODO: Unload all dynamic loaded data (textures, sounds, models...)
}

// Update and Draw (one frame)
void UpdateDrawFrame(void)
{
    UpdateGame();
    DrawGame();
}
