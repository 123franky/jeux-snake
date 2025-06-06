#include "raylib.h"
#include <stdlib.h>
#include <time.h>
#include <math.h> // Pour fabsf

#define SQUARE_SIZE 20
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define MAX_SNAKE_LENGTH 1000
#define MAX_OBSTACLES 5

typedef struct SnakeSegment {
    Vector2 position;
    float size;
} SnakeSegment;

typedef enum Direction {
    UP, DOWN, LEFT, RIGHT
} Direction;

// Interpolation linéaire
float Lerp(float a, float b, float t) {
    return a + t * (b - a);
}

// Vérifie si deux Vector2 sont égaux
bool Vector2Equals(Vector2 a, Vector2 b) {
    return (fabsf(a.x - b.x) < 0.1f) && (fabsf(a.y - b.y) < 0.1f);
}

SnakeSegment snake[MAX_SNAKE_LENGTH];
int snakeLength = 1;
Direction currentDir = RIGHT;

Vector2 fruit;
Vector2 obstacles[MAX_OBSTACLES];

int score = 0;
bool gameOver = false;
bool pause = false;

void ResetGame() {
    snakeLength = 1;
    snake[0].position.x = SCREEN_WIDTH / 2;
    snake[0].position.y = SCREEN_HEIGHT / 2;
    snake[0].size = 24;
    currentDir = RIGHT;
    score = 0;
    gameOver = false;
    pause = false;

    fruit.x = (rand() % (SCREEN_WIDTH / SQUARE_SIZE)) * SQUARE_SIZE;
    fruit.y = (rand() % (SCREEN_HEIGHT / SQUARE_SIZE)) * SQUARE_SIZE;

    for (int i = 0; i < MAX_OBSTACLES; i++) {
        obstacles[i].x = (rand() % (SCREEN_WIDTH / SQUARE_SIZE)) * SQUARE_SIZE;
        obstacles[i].y = (rand() % (SCREEN_HEIGHT / SQUARE_SIZE)) * SQUARE_SIZE;
    }
}

void UpdateGame() {
    // Pause
    if (IsKeyPressed(KEY_P) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_MIDDLE_RIGHT)) {
        pause = !pause;
    }
    if (pause || gameOver) return;

    // Contrôles clavier
    if (IsKeyPressed(KEY_UP) && currentDir != DOWN) currentDir = UP;
    if (IsKeyPressed(KEY_DOWN) && currentDir != UP) currentDir = DOWN;
    if (IsKeyPressed(KEY_LEFT) && currentDir != RIGHT) currentDir = LEFT;
    if (IsKeyPressed(KEY_RIGHT) && currentDir != LEFT) currentDir = RIGHT;

    // Contrôles manette PS4
    float axisX = GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_X);
    float axisY = GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_Y);

    if (axisY < -0.5f && currentDir != DOWN) currentDir = UP;
    if (axisY > 0.5f && currentDir != UP) currentDir = DOWN;
    if (axisX < -0.5f && currentDir != RIGHT) currentDir = LEFT;
    if (axisX > 0.5f && currentDir != LEFT) currentDir = RIGHT;

    // Corps suit la tête
    for (int i = snakeLength - 1; i > 0; i--) {
        snake[i].position = snake[i - 1].position;
        snake[i].size = Lerp(8, 24, (float)(snakeLength - i) / snakeLength);
    }

    // Tête avance
    if (currentDir == UP) snake[0].position.y -= SQUARE_SIZE;
    else if (currentDir == DOWN) snake[0].position.y += SQUARE_SIZE;
    else if (currentDir == LEFT) snake[0].position.x -= SQUARE_SIZE;
    else if (currentDir == RIGHT) snake[0].position.x += SQUARE_SIZE;

    // Passage bords
    if (snake[0].position.x < 0) snake[0].position.x = SCREEN_WIDTH - SQUARE_SIZE;
    if (snake[0].position.x >= SCREEN_WIDTH) snake[0].position.x = 0;
    if (snake[0].position.y < 0) snake[0].position.y = SCREEN_HEIGHT - SQUARE_SIZE;
    if (snake[0].position.y >= SCREEN_HEIGHT) snake[0].position.y = 0;

    // Collision avec soi-même
    for (int i = 1; i < snakeLength; i++) {
        if (Vector2Equals(snake[0].position, snake[i].position)) {
            gameOver = true;
        }
    }

    // Collision obstacles
    for (int i = 0; i < MAX_OBSTACLES; i++) {
        if (Vector2Equals(snake[0].position, obstacles[i])) {
            gameOver = true;
        }
    }

    // Mange fruit
    if (Vector2Equals(snake[0].position, fruit)) {
        if (snakeLength < MAX_SNAKE_LENGTH) {
            snake[snakeLength] = snake[snakeLength - 1];
            snakeLength++;
        }
        score++;
        fruit.x = (rand() % (SCREEN_WIDTH / SQUARE_SIZE)) * SQUARE_SIZE;
        fruit.y = (rand() % (SCREEN_HEIGHT / SQUARE_SIZE)) * SQUARE_SIZE;
    }
}

void DrawGame() {
    BeginDrawing();
    ClearBackground(DARKGRAY);

    if (!gameOver) {
        for (int i = 0; i < snakeLength; i++) {
            DrawRectangleV(snake[i].position, (Vector2){snake[i].size, snake[i].size}, i == 0 ? YELLOW : GREEN);
        }

        DrawRectangleV(fruit, (Vector2){SQUARE_SIZE, SQUARE_SIZE}, RED);

        for (int i = 0; i < MAX_OBSTACLES; i++) {
            DrawRectangleV(obstacles[i], (Vector2){SQUARE_SIZE, SQUARE_SIZE}, BLACK);
        }

        DrawText(TextFormat("Score: %d", score), 10, 10, 20, WHITE);
        if (pause) {
            DrawText("PAUSE", SCREEN_WIDTH / 2 - 50, SCREEN_HEIGHT / 2, 40, GRAY);
        }
    } else {
        DrawText("Vous avez perdu !!!", SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2 - 20, 30, RED);
        DrawText("Appuyez sur A pour recommencer...", SCREEN_WIDTH / 2 - 150, SCREEN_HEIGHT / 2 + 30, 20, LIGHTGRAY);
    }

    EndDrawing();
}

int main() {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Snake avec Raylib");
    SetTargetFPS(10);
    InitAudioDevice(); // Optionnel si tu ajoutes des sons
    srand(time(NULL));
    ResetGame();

    while (!WindowShouldClose()) {
        if (!gameOver) {
            UpdateGame();
        } else if (IsKeyPressed(KEY_A) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN)) {
            ResetGame();
        }

        DrawGame();
    }

    CloseWindow();
    return 0;
}
