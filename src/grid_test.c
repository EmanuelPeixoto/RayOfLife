#include "raylib.h"
#include "raymath.h"

#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

typedef struct Game {
    bool is_paused;
} Game;

void draw_grid(Camera2D camera, int screenWidth, int screenHeight, bool drawLines) {
    // Define the base grid size (cell size in world units)
    const int baseGridSize = 50;

    // Convert screen coordinates to world coordinates
    Vector2 topLeft = camera.target;
    Vector2 bottomRight = Vector2Add(camera.target, (Vector2){screenWidth, screenHeight});

    DrawCircleV(Vector2Add(topLeft, (Vector2){ 50, 50 }), 20, RED);
    DrawCircleV(Vector2Add(bottomRight, (Vector2){ -50, -50 }), 20, RED);

    // Adjust start positions based on camera's target and offset
    float startX = topLeft.x + baseGridSize;  // Move 1 unit inside the camera's target
    float startY = topLeft.y + baseGridSize;  // Move 1 unit inside the camera's target

    // Snap the start position to the grid
    startX = floor(startX / baseGridSize) * baseGridSize;
    startY = floor(startY / baseGridSize) * baseGridSize;

    DrawCircleV((Vector2){ startX, startY }, 10, BLUE);

    // Draw horizontal lines
    for (float y = startY; y <= bottomRight.y + 1; y += baseGridSize) {
        Vector2 start = (Vector2){topLeft.x + 1, y};
        Vector2 end = (Vector2){bottomRight.x + 1, y};  // Offset by 1
        if (drawLines) {
            DrawLineV(start, end, LIGHTGRAY);
        }
        else {
            // Draw dots if in dot mode (KEY_TWO)
            for (float x = startX; x <= bottomRight.x + 1; x += baseGridSize) {
                Vector2 worldPos = {x, y};
                Vector2 screenPos = worldPos;
                DrawCircleV(screenPos, 3.0f, LIGHTGRAY);
            }
        }
    }

    // Draw vertical lines
    for (float x = startX; x <= bottomRight.x + 1; x += baseGridSize) {
        Vector2 start = (Vector2){x, topLeft.y + 1};  // Offset by 1
        Vector2 end = (Vector2){x, bottomRight.y + 1};  // Offset by 1
        if (drawLines) {
            DrawLineV(start, end, LIGHTGRAY);
        }
        else {
            // Draw dots if in dot mode (KEY_TWO)
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
    InitWindow(screenWidth, screenHeight, "Conway's Game of Life");

    Camera2D camera = { 0 };
    camera.zoom = 1.0f;
    SetTargetFPS(60);

    Game game = { false };
    bool drawLines = true; // Start with lines mode

    while (!WindowShouldClose()) {
        if (IsKeyPressed(KEY_SPACE)) {
            game.is_paused = !game.is_paused;
        }

        // Switch grid mode on key press
        if (IsKeyPressed(KEY_ONE)) {
            drawLines = true; // Lines mode
        }
        if (IsKeyPressed(KEY_TWO)) {
            drawLines = false; // Dots mode
        }

        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            Vector2 delta = GetMouseDelta();
            delta = Vector2Scale(delta, -1.0f / camera.zoom);
            camera.target = Vector2Add(camera.target, delta); 
        }

        // Begin drawing
        BeginDrawing();
          ClearBackground(RAYWHITE);

          // Begin 2D mode for the camera
          BeginMode2D(camera);
            // Draw the grid in the selected mode (lines or dots)
            draw_grid(camera, screenWidth, screenHeight, drawLines);

          EndMode2D();


          // Display camera target and offset directly
          DrawText(TextFormat("Camera Target X: %.2f", camera.target.x), 10, 40, 20, DARKGRAY);
          DrawText(TextFormat("Camera Target Y: %.2f", camera.target.y), 10, 70, 20, DARKGRAY);
          DrawText(TextFormat("Camera Offset X: %.2f", camera.offset.x), 10, 100, 20, DARKGRAY);
          DrawText(TextFormat("Camera Offset Y: %.2f", camera.offset.y), 10, 130, 20, DARKGRAY);
          DrawText(game.is_paused ? "PAUSED" : "RUNNING", 20, 20, 20, BLACK);

        EndDrawing();
    }

    CloseWindow(); // Close window and OpenGL context
    return 0;
}

