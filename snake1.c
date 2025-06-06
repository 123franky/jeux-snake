#include "raylib.h"
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

#define LARGEUR 1000
#define HAUTEUR 800
#define TAILLE_CASE 30
#define NB_CASES_X (LARGEUR / TAILLE_CASE)
#define NB_CASES_Y (HAUTEUR / TAILLE_CASE)
#define MAX_SERPENT 100
#define NB_FRUITS 3
#define NB_OBSTACLES 20

typedef struct {
    int x;
    int y;
} Position;

typedef enum { HAUT, BAS, GAUCHE, DROITE } Direction;

Texture2D pelouse;

void ChargerPelouse() {
    pelouse = LoadTexture("pose-gazon-synthetique-toulon-9.jpeg");
}

void DechargerTextures() {
    UnloadTexture(pelouse);
}

void DrawSerpent(Position serpent[], int taille, Color couleurTete, Color couleurCorps, bool joueur2) {
    for (int i = 0; i < taille; i++) {
        Color c = (i == 0) ? couleurTete : couleurCorps;
        DrawCircle(serpent[i].x * TAILLE_CASE + TAILLE_CASE / 2,
                   serpent[i].y * TAILLE_CASE + TAILLE_CASE / 2,
                   TAILLE_CASE / 2 - 2, c);
    }

    // Dessiner les yeux pour la tête
    int hx = serpent[0].x * TAILLE_CASE;
    int hy = serpent[0].y * TAILLE_CASE;
    DrawCircle(hx + TAILLE_CASE / 3, hy + TAILLE_CASE / 3, 3, BLACK);
    DrawCircle(hx + 2 * TAILLE_CASE / 3, hy + TAILLE_CASE / 3, 3, BLACK);
    DrawCircle(hx + TAILLE_CASE / 2, hy + 2 * TAILLE_CASE / 3, 2, RED); // bouche
}

int main() {
    InitWindow(LARGEUR, HAUTEUR, "Snake Multijoueur avec Raylib");

    // Choix du nombre de joueurs et du niveau
    int nbJoueurs = 1;
    int niveau = 1;

    while (!IsKeyPressed(KEY_ONE) && !IsKeyPressed(KEY_TWO)) {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawText("Appuie sur 1 pour 1 joueur, 2 pour 2 joueurs", 200, 300, 20, BLACK);
        EndDrawing();
    }
    if (IsKeyPressed(KEY_TWO)) nbJoueurs = 2;

    while (!IsKeyPressed(KEY_ONE) && !IsKeyPressed(KEY_TWO) && !IsKeyPressed(KEY_THREE)) {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawText("Choisis un niveau: 1 (lent), 2 (moyen), 3 (rapide)", 200, 350, 20, DARKGRAY);
        EndDrawing();
    }
    if (IsKeyPressed(KEY_TWO)) niveau = 2;
    else if (IsKeyPressed(KEY_THREE)) niveau = 3;

    SetTargetFPS(5 + 5 * niveau);

    InitAudioDevice();
    Music music = LoadMusicStream("musique.mp3");
    PlayMusicStream(music);

    srand(time(NULL));

    ChargerPelouse();

    Direction direction1 = DROITE;
    Direction ancienneDirection1 = DROITE;
    int taille1 = 3;
    Position serpent1[MAX_SERPENT] = {{10, 10}, {9, 10}, {8, 10}};

    Direction direction2 = GAUCHE;
    Direction ancienneDirection2 = GAUCHE;
    int taille2 = 3;
    Position serpent2[MAX_SERPENT] = {{20, 20}, {21, 20}, {22, 20}};

    Position fruits[NB_FRUITS];
    for (int i = 0; i < NB_FRUITS; i++) {
        fruits[i].x = rand() % NB_CASES_X;
        fruits[i].y = rand() % NB_CASES_Y;
    }

    Position obstacles[NB_OBSTACLES];
    for (int i = 0; i < NB_OBSTACLES; i++) {
        obstacles[i].x = rand() % NB_CASES_X;
        obstacles[i].y = rand() % NB_CASES_Y;
    }

    int score1 = 0, score2 = 0;
    bool gameOver = false;
    bool pause = false;

    while (!WindowShouldClose()) {
        UpdateMusicStream(music);

        if (IsKeyPressed(KEY_P)) pause = !pause;

        if (!pause && !gameOver) {
            if (IsKeyPressed(KEY_UP) && ancienneDirection1 != BAS) direction1 = HAUT;
            else if (IsKeyPressed(KEY_DOWN) && ancienneDirection1 != HAUT) direction1 = BAS;
            else if (IsKeyPressed(KEY_LEFT) && ancienneDirection1 != DROITE) direction1 = GAUCHE;
            else if (IsKeyPressed(KEY_RIGHT) && ancienneDirection1 != GAUCHE) direction1 = DROITE;

            if (nbJoueurs == 2) {
                if (IsKeyPressed(KEY_W) && ancienneDirection2 != BAS) direction2 = HAUT;
                else if (IsKeyPressed(KEY_S) && ancienneDirection2 != HAUT) direction2 = BAS;
                else if (IsKeyPressed(KEY_A) && ancienneDirection2 != DROITE) direction2 = GAUCHE;
                else if (IsKeyPressed(KEY_D) && ancienneDirection2 != GAUCHE) direction2 = DROITE;
            }

            for (int i = taille1; i > 0; i--) serpent1[i] = serpent1[i - 1];
            for (int i = taille2; i > 0; i--) serpent2[i] = serpent2[i - 1];

            switch (direction1) {
                case HAUT: serpent1[0].y--; break;
                case BAS: serpent1[0].y++; break;
                case GAUCHE: serpent1[0].x--; break;
                case DROITE: serpent1[0].x++; break;
            }
            if (nbJoueurs == 2) {
                switch (direction2) {
                    case HAUT: serpent2[0].y--; break;
                    case BAS: serpent2[0].y++; break;
                    case GAUCHE: serpent2[0].x--; break;
                    case DROITE: serpent2[0].x++; break;
                }
            }

            ancienneDirection1 = direction1;
            ancienneDirection2 = direction2;

            serpent1[0].x = (serpent1[0].x + NB_CASES_X) % NB_CASES_X;
            serpent1[0].y = (serpent1[0].y + NB_CASES_Y) % NB_CASES_Y;
            serpent2[0].x = (serpent2[0].x + NB_CASES_X) % NB_CASES_X;
            serpent2[0].y = (serpent2[0].y + NB_CASES_Y) % NB_CASES_Y;

            for (int i = 0; i < NB_OBSTACLES; i++) {
                if ((serpent1[0].x == obstacles[i].x && serpent1[0].y == obstacles[i].y) ||
                    (nbJoueurs == 2 && serpent2[0].x == obstacles[i].x && serpent2[0].y == obstacles[i].y)) {
                    gameOver = true;
                }
            }

            for (int f = 0; f < NB_FRUITS; f++) {
                if (serpent1[0].x == fruits[f].x && serpent1[0].y == fruits[f].y) {
                    taille1++;
                    score1++;
                    fruits[f].x = rand() % NB_CASES_X;
                    fruits[f].y = rand() % NB_CASES_Y;
                }
                if (nbJoueurs == 2 && serpent2[0].x == fruits[f].x && serpent2[0].y == fruits[f].y) {
                    taille2++;
                    score2++;
                    fruits[f].x = rand() % NB_CASES_X;
                    fruits[f].y = rand() % NB_CASES_Y;
                }
            }
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawTexture(pelouse, 0, 0, WHITE);

        for (int i = 0; i < NB_OBSTACLES; i++) {
            DrawRectangle(obstacles[i].x * TAILLE_CASE, obstacles[i].y * TAILLE_CASE, TAILLE_CASE, TAILLE_CASE, BLUE);
        }

        for (int i = 0; i < NB_FRUITS; i++) {
            DrawCircle(fruits[i].x * TAILLE_CASE + TAILLE_CASE / 2,
                       fruits[i].y * TAILLE_CASE + TAILLE_CASE / 2,
                       TAILLE_CASE / 2 - 2, RED);
        }

        DrawSerpent(serpent1, taille1, DARKGREEN, GREEN, false);
        if (nbJoueurs == 2) DrawSerpent(serpent2, taille2, DARKBLUE, SKYBLUE, true);

        DrawText(TextFormat("J1: %d", score1), 10, 10, 20, BLACK);
        if (nbJoueurs == 2) DrawText(TextFormat("J2: %d", score2), 10, 40, 20, BLACK);

        if (pause) {
            DrawText("PAUSE", LARGEUR / 2 - 50, HAUTEUR / 2, 30, GRAY);
        }

        if (gameOver) {
            DrawText("GAME OVER - Appuie sur [R] pour recommencer", LARGEUR / 2 - 200, HAUTEUR / 2 + 40, 20, RED);
            if (IsKeyPressed(KEY_R)) {
                taille1 = 3; taille2 = 3;
                score1 = 0; score2 = 0;
                serpent1[0] = (Position){10, 10}; serpent2[0] = (Position){20, 20};
                direction1 = DROITE; direction2 = GAUCHE;
                gameOver = false;
            }
        }

        EndDrawing();
    }

    StopMusicStream(music);
    UnloadMusicStream(music);
    CloseAudioDevice();
    DechargerTextures();
    CloseWindow();
    return 0;
}
