#include "raylib.h"
#include "raymath.h"

#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

typedef struct Game {
  bool is_paused;
  bool drawLines;
} Game;

void draw_grid(Camera2D camera, int screenWidth, int screenHeight, Game game) {
    const int baseGridSize = 50;

    Vector2 topLeft = camera.target;
    Vector2 bottomRight = Vector2Add(camera.target, (Vector2){
        screenWidth / camera.zoom, 
        screenHeight / camera.zoom
    });

    DrawCircleV(Vector2Add(topLeft, (Vector2){ 50, 50 }), 20, RED);
    DrawCircleV(Vector2Add(bottomRight, (Vector2){ -50, -50 }), 20, RED);

    float startX = topLeft.x + baseGridSize;
    float startY = topLeft.y + baseGridSize;

    startX = floor(startX / baseGridSize) * baseGridSize;
    startY = floor(startY / baseGridSize) * baseGridSize;

    DrawCircleV((Vector2){ startX, startY }, 10, BLUE);

    for (float y = startY; y <= bottomRight.y + 1; y += baseGridSize) {
        Vector2 start = (Vector2){topLeft.x + 1, y};
        Vector2 end = (Vector2){bottomRight.x + 1, y};  // Offset by 1
        if (game.drawLines) {
            DrawLineV(start, end, LIGHTGRAY);
        }
        else {
            for (float x = startX; x <= bottomRight.x + 1; x += baseGridSize) {
                Vector2 worldPos = {x, y};
                Vector2 screenPos = worldPos;
                DrawCircleV(screenPos, 3.0f, LIGHTGRAY);
            }
        }
    }

    for (float x = startX; x <= bottomRight.x + 1; x += baseGridSize) {
        Vector2 start = (Vector2){x, topLeft.y + 1};  // Offset by 1
        Vector2 end = (Vector2){x, bottomRight.y + 1};  // Offset by 1
        if (game.drawLines) {
            DrawLineV(start, end, LIGHTGRAY);
        }
        else {
            for (float y = startY; y <= bottomRight.y + 1; y += baseGridSize) {
                Vector2 worldPos = {x, y};
                Vector2 screenPos = worldPos;
                DrawCircleV(screenPos, 3.0f, LIGHTGRAY);
            }
        }
    }
}

int main() {
    srand(time(NULL));
    int screenWidth = 800;
    int screenHeight = 450;
    InitWindow(screenWidth, screenHeight, "Infinite grid and movement test");

    Camera2D camera = { 0 };
    camera.zoom = 1.0f;
    SetTargetFPS(60);

    Game game = { false, false };

    while (!WindowShouldClose()) {
        if (IsKeyPressed(KEY_ONE)) { game.drawLines = !game.drawLines; }

        float zoomSpeed = 0.1f;
        camera.zoom = Clamp(camera.zoom + GetMouseWheelMove() * zoomSpeed, 0.1f, 5.0f);

        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            Vector2 delta = GetMouseDelta();
            delta = Vector2Scale(delta, -1.0f / camera.zoom);
            camera.target = Vector2Add(camera.target, delta); 
        }

        BeginDrawing();
          ClearBackground(RAYWHITE);
          BeginMode2D(camera);
            draw_grid(camera, screenWidth, screenHeight, game);
          EndMode2D();
          DrawText(TextFormat("Camera Target X: %.2f", camera.target.x), 10, 10, 20, BLACK);
          DrawText(TextFormat("Camera Target Y: %.2f", camera.target.y), 10, 40, 20, BLACK);
          DrawText(TextFormat("Camera Zoom: %.2f", camera.zoom), 10, 70, 20, BLACK);
        EndDrawing();
    }
    CloseWindow();
    return 0;
}

