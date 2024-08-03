#include "raylib.h"

//defines:
#define Cobra_tamanho   256 //MAXIMO DO TAMANHO DA COBRA
#define Tamanho_CEDULAS     30 //TAMANHO DAS CEDULAS

//----------------------------------------------------------------------------------
// TIPOS E ESTRUTURA DO JOGO:
//----------------------------------------------------------------------------------
typedef struct Snake {
    Vector2 position;
    Vector2 size;
    Vector2 speed;
    Color color;
} Snake;

typedef struct Food {
    Vector2 position;
    Vector2 size;
    bool active;
    Color color;
} Food;

//------------------------------------------------------------------------------------
// Global Variables Declaration
//------------------------------------------------------------------------------------
static const int screenWidth = 800;
static const int screenHeight = 800;

static int framesCounter = 0;
static bool gameOver = false;
static bool pause = false;

static Food fruit = { 0 };
static Snake snake[Cobra_tamanho] = { 0 };
static Vector2 snakePosition[Cobra_tamanho] = { 0 };
static bool allowMove = false;
static Vector2 offset = { 0 };
static int counterTail = 0;

//------------------------------------------------------------------------------------
// Module Functions Declaration (local)
//------------------------------------------------------------------------------------
static void InitGame(void);         // Initialize game
static void UpdateGame(void);       // Update game (one frame)
static void DrawGame(void);         // Draw game (one frame)
static void UnloadGame(void);       // Unload game
static void UpdateDrawFrame(void);  // Update and Draw (one frame)

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void)
{
    // Initialization (Note windowTitle is unused on Android)
    //---------------------------------------------------------
    InitWindow(screenWidth, screenHeight, "classic game: snake");

    InitGame();

#if defined(PLATFORM_WEB)
    emscripten_set_main_loop(UpdateDrawFrame, 60, 1);
#else
    SetTargetFPS(60);
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update and Draw
        //----------------------------------------------------------------------------------
        UpdateDrawFrame();
        //----------------------------------------------------------------------------------
    }
#endif
    // De-Initialization
    //--------------------------------------------------------------------------------------
    UnloadGame();         // Unload loaded data (textures, sounds, models...)

    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}

//------------------------------------------------------------------------------------
// Module Functions Definitions (local)
//------------------------------------------------------------------------------------

// Initialize game variables
void InitGame(void)
{
    framesCounter = 0;
    gameOver = false;
    pause = false;

    counterTail = 1;
    allowMove = false;

    offset.x = screenWidth%Tamanho_CEDULAS;
    offset.y = screenHeight%Tamanho_CEDULAS;

    for (int i = 0; i < Cobra_tamanho; i++)
    {
        snake[i].position = (Vector2){ offset.x/2, offset.y/2 };
        snake[i].size = (Vector2){ Tamanho_CEDULAS, Tamanho_CEDULAS };
        snake[i].speed = (Vector2){ Tamanho_CEDULAS, 0 };

        if (i == 0) snake[i].color = DARKBLUE;
        else snake[i].color = BLUE;
    }

    for (int i = 0; i < Cobra_tamanho; i++)
    {
        snakePosition[i] = (Vector2){ 0.0f, 0.0f };
    }

    fruit.size = (Vector2){ Tamanho_CEDULAS, Tamanho_CEDULAS };
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
            // Player control
            if (IsKeyPressed(KEY_RIGHT)||IsKeyPressed(KEY_D) && (snake[0].speed.x == 0) && allowMove)
            {
                snake[0].speed = (Vector2){ Tamanho_CEDULAS, 0 };
                allowMove = false;
            }
            if (IsKeyPressed(KEY_LEFT)||IsKeyPressed(KEY_A) && (snake[0].speed.x == 0) && allowMove)
            {
                snake[0].speed = (Vector2){ -Tamanho_CEDULAS, 0 };
                allowMove = false;
            }
            if (IsKeyPressed(KEY_UP)||IsKeyPressed(KEY_W) && (snake[0].speed.y == 0) && allowMove)
            {
                snake[0].speed = (Vector2){ 0, -Tamanho_CEDULAS };
                allowMove = false;
            }
            if (IsKeyPressed(KEY_DOWN)||IsKeyPressed(KEY_S) && (snake[0].speed.y == 0) && allowMove)
            {
                snake[0].speed = (Vector2){ 0, Tamanho_CEDULAS };
                allowMove = false;
            }

            // Snake movement
            for (int i = 0; i < counterTail; i++) snakePosition[i] = snake[i].position;

            if ((framesCounter%5) == 0)
            {
                for (int i = 0; i < counterTail; i++)
                {
                    if (i == 0)
                    {
                        snake[0].position.x += snake[0].speed.x;
                        snake[0].position.y += snake[0].speed.y;
                        allowMove = true;
                    }
                    else snake[i].position = snakePosition[i-1];
                }
            }

            // Wall behaviour
            if (((snake[0].position.x) > (screenWidth - offset.x)) ||
                ((snake[0].position.y) > (screenHeight - offset.y)) ||
                (snake[0].position.x < 0) || (snake[0].position.y < 0))
            {
                gameOver = true;
            }

            // Collision with yourself
            for (int i = 1; i < counterTail; i++)
            {
                if ((snake[0].position.x == snake[i].position.x) && (snake[0].position.y == snake[i].position.y)) gameOver = true;
            }

            // Fruit position calculation
            if (!fruit.active)
            {
                fruit.active = true;
                fruit.position = (Vector2){ GetRandomValue(0, (screenWidth/Tamanho_CEDULAS) - 1)*Tamanho_CEDULAS + offset.x/2, GetRandomValue(0, (screenHeight/Tamanho_CEDULAS) - 1)*Tamanho_CEDULAS + offset.y/2 };

                for (int i = 0; i < counterTail; i++)
                {
                    while ((fruit.position.x == snake[i].position.x) && (fruit.position.y == snake[i].position.y))
                    {
                        fruit.position = (Vector2){ GetRandomValue(0, (screenWidth/Tamanho_CEDULAS) - 1)*Tamanho_CEDULAS + offset.x/2, GetRandomValue(0, (screenHeight/Tamanho_CEDULAS) - 1)*Tamanho_CEDULAS + offset.y/2 };
                        i = 0;
                    }
                }
            }

            // Collision
            if ((snake[0].position.x < (fruit.position.x + fruit.size.x) && (snake[0].position.x + snake[0].size.x) > fruit.position.x) &&
                (snake[0].position.y < (fruit.position.y + fruit.size.y) && (snake[0].position.y + snake[0].size.y) > fruit.position.y))
            {
                snake[counterTail].position = snakePosition[counterTail - 1];
                counterTail += 1;
                fruit.active = false;
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
            // Draw grid lines
            for (int i = 0; i < screenWidth/Tamanho_CEDULAS + 1; i++)
            {
                DrawLineV((Vector2){Tamanho_CEDULAS*i + offset.x/2, offset.y/2}, (Vector2){Tamanho_CEDULAS*i + offset.x/2, screenHeight - offset.y/2}, LIGHTGRAY);
            }

            for (int i = 0; i < screenHeight/Tamanho_CEDULAS + 1; i++)
            {
                DrawLineV((Vector2){offset.x/2, Tamanho_CEDULAS*i + offset.y/2}, (Vector2){screenWidth - offset.x/2, Tamanho_CEDULAS*i + offset.y/2}, LIGHTGRAY);
            }

            // Draw snake
            for (int i = 0; i < counterTail; i++) DrawRectangleV(snake[i].position, snake[i].size, snake[i].color);

            // Draw fruit to pick
            DrawRectangleV(fruit.position, fruit.size, fruit.color);

            if (pause) DrawText("GAME PAUSED", screenWidth/2 - MeasureText("GAME PAUSED", 40)/2, screenHeight/2 - 40, 40, GRAY);
        }
        else DrawText("PRESS [ENTER] TO PLAY AGAIN", GetScreenWidth()/2 - MeasureText("PRESS [ENTER] TO PLAY AGAIN", 20)/2, GetScreenHeight()/2 - 50, 20, GRAY);

    EndDrawing();
}

// Unload game variables
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
