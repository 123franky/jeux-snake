#include "raylib.h"
#include <stdlib.h>
#include <time.h>

#define SQUARE_SIZE 20
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define MAX_SNAKE_LENGTH 1000
#define MAX_OBSTACLES 5

typedef struct SnakeSegment {
    int x, y;
} SnakeSegment;

typedef enum Direction {
    UP, DOWN, LEFT, RIGHT
} Direction;

SnakeSegment snake[MAX_SNAKE_LENGTH];
int snakeLength = 1;
Direction currentDir = RIGHT;

SnakeSegment fruit;
SnakeSegment obstacles[MAX_OBSTACLES];

int score = 0;
bool gameOver = false;

void ResetGame() {
    snakeLength = 1;
    snake[0].x = SCREEN_WIDTH / 2;
    snake[0].y = SCREEN_HEIGHT / 2;
    currentDir = RIGHT;
    score = 0;
    gameOver = false;

    fruit.x = (rand() % (SCREEN_WIDTH / SQUARE_SIZE)) * SQUARE_SIZE;
    fruit.y = (rand() % (SCREEN_HEIGHT / SQUARE_SIZE)) * SQUARE_SIZE;

    // Initialiser obstacles
    for (int i = 0; i < MAX_OBSTACLES; i++) {
        obstacles[i].x = (rand() % (SCREEN_WIDTH / SQUARE_SIZE)) * SQUARE_SIZE;
        obstacles[i].y = (rand() % (SCREEN_HEIGHT / SQUARE_SIZE)) * SQUARE_SIZE;
    }
}

void UpdateGame() {
    // Contrôle joueur
    if (IsKeyPressed(KEY_UP) && currentDir != DOWN) currentDir = UP;
    if (IsKeyPressed(KEY_DOWN) && currentDir != UP) currentDir = DOWN;
    if (IsKeyPressed(KEY_LEFT) && currentDir != RIGHT) currentDir = LEFT;
    if (IsKeyPressed(KEY_RIGHT) && currentDir != LEFT) currentDir = RIGHT;

    // Déplacement du corps
    for (int i = snakeLength - 1; i > 0; i--) {
        snake[i] = snake[i - 1];
    }

    // Déplacement de la tête
    if (currentDir == UP) snake[0].y -= SQUARE_SIZE;
    else if (currentDir == DOWN) snake[0].y += SQUARE_SIZE;
    else if (currentDir == LEFT) snake[0].x -= SQUARE_SIZE;
    else if (currentDir == RIGHT) snake[0].x += SQUARE_SIZE;

    // Passage par les bords
    if (snake[0].x < 0) snake[0].x = SCREEN_WIDTH - SQUARE_SIZE;
    if (snake[0].x >= SCREEN_WIDTH) snake[0].x = 0;
    if (snake[0].y < 0) snake[0].y = SCREEN_HEIGHT - SQUARE_SIZE;
    if (snake[0].y >= SCREEN_HEIGHT) snake[0].y = 0;

    // Collision corps
    for (int i = 1; i < snakeLength; i++) {
        if (snake[0].x == snake[i].x && snake[0].y == snake[i].y) {
            gameOver = true;
        }
    }

    // Collision obstacles
    for (int i = 0; i < MAX_OBSTACLES; i++) {
        if (snake[0].x == obstacles[i].x && snake[0].y == obstacles[i].y) {
            gameOver = true;
        }
    }

    // Fruit mangé
    if (snake[0].x == fruit.x && snake[0].y == fruit.y) {
        snakeLength++;
        score++;
        fruit.x = (rand() % (SCREEN_WIDTH / SQUARE_SIZE)) * SQUARE_SIZE;
        fruit.y = (rand() % (SCREEN_HEIGHT / SQUARE_SIZE)) * SQUARE_SIZE;
    }
}

void DrawGame() {
    BeginDrawing();
    ClearBackground(ORANGE);

    if (!gameOver) {
        // Dessin du serpent
        for (int i = 0; i < snakeLength; i++) {
            DrawRectangle(snake[i].x, snake[i].y, SQUARE_SIZE, SQUARE_SIZE, GREEN);
        }

        // Dessin du fruit
        DrawRectangle(fruit.x, fruit.y, SQUARE_SIZE, SQUARE_SIZE, RED);

        // Dessin des obstacles
        for (int i = 0; i < MAX_OBSTACLES; i++) {
            DrawRectangle(obstacles[i].x, obstacles[i].y, SQUARE_SIZE, SQUARE_SIZE, BLACK);
        }

        // Affichage score
        DrawText(TextFormat("Score: %d", score), 10, 10, 20, WHITE);
    } else {
        DrawText("Vous avez perdu !!!", SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2 - 20, 30, RED);
        DrawText("Appuyez sur A pour recommencer...", SCREEN_WIDTH / 2 - 150, SCREEN_HEIGHT / 2 + 30, 20, LIGHTGRAY);
    }

    EndDrawing();
}

int main() {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Snake avec Raylib");
    SetTargetFPS(10);
    srand(time(NULL));

    ResetGame();

    while (!WindowShouldClose()) {
        if (!gameOver) {
            UpdateGame();
        } else if (IsKeyPressed(KEY_A)) {
            ResetGame();
        }

        DrawGame();
    }

    CloseWindow();
    return 0;
}
