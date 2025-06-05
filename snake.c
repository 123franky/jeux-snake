#include "raylib.h"
#include <stdlib.h>
#include <time.h>

#define LARGEUR 1000
#define HAUTEUR 800
#define TAILLE_CASE 25
#define NB_CASES_X (LARGEUR / TAILLE_CASE)
#define NB_CASES_Y (HAUTEUR / TAILLE_CASE)
#define MAX_SERPENT 200
#define NB_FRUITS 3
#define NB_OBSTACLES 10

typedef struct {
    int x;
    int y;
} Position;

typedef enum { HAUT, BAS, GAUCHE, DROITE } Direction;

void DessinerDamier() {
    for (int y = 0; y < NB_CASES_Y; y++) {
        for (int x = 0; x < NB_CASES_X; x++) {
            DrawRectangle(x * TAILLE_CASE, y * TAILLE_CASE, TAILLE_CASE, TAILLE_CASE, WHITE);
        }
    }
}

bool EstEgale(Position a, Position b) {
    return a.x == b.x && a.y == b.y;
}

bool EstSurObstacle(Position p, Position obstacles[], int nb) {
    for (int i = 0; i < nb; i++) {
        if (EstEgale(p, obstacles[i])) return true;
    }
    return false;
}

void InitialiserObstacles(Position obstacles[], int nb) {
    for (int i = 0; i < nb; i++) {
        obstacles[i].x = rand() % NB_CASES_X;
        obstacles[i].y = rand() % NB_CASES_Y;
    }
}

void InitialiserFruits(Position fruits[], int nb, Position obstacles[]) {
    for (int i = 0; i < nb; i++) {
        do {
            fruits[i].x = rand() % NB_CASES_X;
            fruits[i].y = rand() % NB_CASES_Y;
        } while (EstSurObstacle(fruits[i], obstacles, NB_OBSTACLES));
    }
}

int main() {
    InitWindow(LARGEUR, HAUTEUR, "Snake Avanc\xE9 - Raylib");
    InitAudioDevice();
    Music musique = LoadMusicStream("musique.mp3");
    PlayMusicStream(musique);

    SetTargetFPS(6);
    srand(time(NULL));

    int niveau = 1;
    int vitesse[] = {6, 10, 15};
    int score = 0;
    bool recommencer = false;

    do {
        SetTargetFPS(vitesse[niveau - 1]);

        Direction direction = DROITE;
        Direction ancienneDirection = DROITE;
        Position serpent[MAX_SERPENT] = { {10, 10}, {9, 10}, {8, 10} };
        int tailleSerpent = 3;

        Position fruits[NB_FRUITS];
        Position obstacles[NB_OBSTACLES];
        InitialiserObstacles(obstacles, NB_OBSTACLES);
        InitialiserFruits(fruits, NB_FRUITS, obstacles);
        score = 0;
        recommencer = false;

        while (!WindowShouldClose()) {
            UpdateMusicStream(musique);

            if (IsKeyPressed(KEY_UP) && ancienneDirection != BAS) direction = HAUT;
            else if (IsKeyPressed(KEY_DOWN) && ancienneDirection != HAUT) direction = BAS;
            else if (IsKeyPressed(KEY_LEFT) && ancienneDirection != DROITE) direction = GAUCHE;
            else if (IsKeyPressed(KEY_RIGHT) && ancienneDirection != GAUCHE) direction = DROITE;

            for (int i = tailleSerpent; i > 0; i--) {
                serpent[i] = serpent[i - 1];
            }

            switch (direction) {
                case HAUT: serpent[0].y--; break;
                case BAS: serpent[0].y++; break;
                case GAUCHE: serpent[0].x--; break;
                case DROITE: serpent[0].x++; break;
            }

            ancienneDirection = direction;

            if (serpent[0].x < 0) serpent[0].x = NB_CASES_X - 1;
            if (serpent[0].x >= NB_CASES_X) serpent[0].x = 0;
            if (serpent[0].y < 0) serpent[0].y = NB_CASES_Y - 1;
            if (serpent[0].y >= NB_CASES_Y) serpent[0].y = 0;

            for (int i = 1; i < tailleSerpent; i++) {
                if (EstEgale(serpent[0], serpent[i])) {
                    recommencer = true;
                    break;
                }
            }

            if (EstSurObstacle(serpent[0], obstacles, NB_OBSTACLES)) {
                recommencer = true;
            }

            for (int i = 0; i < NB_FRUITS; i++) {
                if (EstEgale(serpent[0], fruits[i])) {
                    tailleSerpent++;
                    fruits[i].x = rand() % NB_CASES_X;
                    fruits[i].y = rand() % NB_CASES_Y;
                    score += 10;
                }
            }

            BeginDrawing();
            DessinerDamier();

            for (int i = 0; i < NB_OBSTACLES; i++) {
                DrawRectangle(obstacles[i].x * TAILLE_CASE, obstacles[i].y * TAILLE_CASE, TAILLE_CASE, TAILLE_CASE, BLUE);
            }

            for (int i = 0; i < NB_FRUITS; i++) {
                DrawCircle(fruits[i].x * TAILLE_CASE + TAILLE_CASE/2, fruits[i].y * TAILLE_CASE + TAILLE_CASE/2, TAILLE_CASE/3, RED);
            }

            for (int i = 0; i < tailleSerpent; i++) {
                float scale = 1.0f - (i * 0.03f);
                Color color = (i == 0) ? DARKGREEN : GREEN;
                DrawEllipse(serpent[i].x * TAILLE_CASE + TAILLE_CASE/2, serpent[i].y * TAILLE_CASE + TAILLE_CASE/2, TAILLE_CASE/2 * scale, TAILLE_CASE/2 * scale, color);
            }

            DrawText(TextFormat("Score: %d", score), 10, 10, 20, BLACK);
            EndDrawing();

            if (recommencer) break;
        }

        while (!WindowShouldClose()) {
            BeginDrawing();
            ClearBackground(RAYWHITE);
            DrawText("Perdu ! Appuie sur R pour recommencer ou ESC pour quitter.", 150, HAUTEUR / 2, 20, RED);
            EndDrawing();

            if (IsKeyPressed(KEY_R)) {
                recommencer = true;
                break;
            }
            if (IsKeyPressed(KEY_ESCAPE)) {
                recommencer = false;
                break;
            }
        }

    } while (recommencer && !WindowShouldClose());

    UnloadMusicStream(musique);
    CloseAudioDevice();
    CloseWindow();
    return 0;
}