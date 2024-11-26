#include "raylib.h"
#include "raymath.h"

#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

#define MAX_BUTTONS 5

typedef struct {
    Rectangle rect;
    const char *label;
} Button;

typedef struct {
    Button buttons[MAX_BUTTONS];
    int buttonCount;
    float verticalSpacing;
} Menu;

Menu CreateMenu(const char *labels[], int buttonCount, int screenWidth, int screenHeight) {
    Menu menu = { .buttonCount = buttonCount, .verticalSpacing = 10.0f };
    float buttonWidth = 200;
    float buttonHeight = 40;

    for (int i = 0; i < buttonCount; i++) {
        menu.buttons[i].rect = (Rectangle){
            (float)(screenWidth) / 2 - buttonWidth / 2,
            (float)(screenHeight) / 2 - ((buttonCount * buttonHeight) + ((buttonCount - 1) * menu.verticalSpacing)) / 2 + i * (buttonHeight + menu.verticalSpacing),
            buttonWidth,
            buttonHeight,
        };
        menu.buttons[i].label = labels[i];
    }

    return menu;
}

int DrawMenu(Menu menu) {
    for (int i = 0; i < menu.buttonCount; i++) {
        DrawRectangleRec(menu.buttons[i].rect, Fade(LIGHTGRAY, 0.8f));
        DrawRectangleLinesEx(menu.buttons[i].rect, 2, BLACK);
        DrawText(menu.buttons[i].label,
                 menu.buttons[i].rect.x + (menu.buttons[i].rect.width - MeasureText(menu.buttons[i].label, 20)) / 2,
                 menu.buttons[i].rect.y + (menu.buttons[i].rect.height - 20) / 2,
                 20, BLACK);

        if (CheckCollisionPointRec(GetMousePosition(), menu.buttons[i].rect) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            return i; // Return the index of the button clicked
        }
    }
    return -1; // No button clicked
}

typedef struct Config {
  bool is_paused;
  bool drawLines;
  bool showSettingsMenu;
  bool closeApp;
} Config;

void draw_grid(Camera2D camera, int screenWidth, int screenHeight, Config cfg) {
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
    if (cfg.drawLines) {
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
    if (cfg.drawLines) {
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

void HandleControls(Config *cfg, Camera2D *camera) {
  if (IsKeyPressed(KEY_ONE)) { cfg->drawLines = !cfg->drawLines; }
    if (IsKeyPressed(KEY_ESCAPE)) {
      cfg->showSettingsMenu ? (cfg->showSettingsMenu = false) : (cfg->is_paused = !cfg->is_paused);
    }

  if (!cfg->is_paused) {
    float wheel = GetMouseWheelMove();
    if (wheel != 0) {
      float zoomSpeed = 0.1f;
      camera->zoom = Clamp(camera->zoom + wheel * zoomSpeed, 0.1f, 5.0f);
    }

    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
      Vector2 delta = GetMouseDelta();
      delta = Vector2Scale(delta, -1.0f / camera->zoom);
      camera->target = Vector2Add(camera->target, delta); 
    }
  }
}

int main() {
  srand(time(NULL));
  int screenWidth = 800;
  int screenHeight = 450;
  InitWindow(screenWidth, screenHeight, "Infinite grid and movement test");
  SetExitKey(KEY_NULL);

  Camera2D camera = { 0 };
  camera.zoom = 1.0f;
  SetTargetFPS(60);

  Config cfg = { false, false, false };

  const char *pauseLabels[] = { "Resume", "Settings", "Quit" };
  Menu pauseMenu = CreateMenu(pauseLabels, 3, screenWidth, screenHeight);

  const char *settingsLabels[] = { "Toggle Lines", "Return" };
  Menu settingsMenu = CreateMenu(settingsLabels, 2, screenWidth, screenHeight);

  while (!WindowShouldClose()) {
    HandleControls(&cfg, &camera);

    BeginDrawing();
      ClearBackground(RAYWHITE);

      if (cfg.closeApp) { break; }

      BeginMode2D(camera); {
        draw_grid(camera, screenWidth, screenHeight, cfg);
      } EndMode2D();

      /*Always Draw*/ {
        DrawText(TextFormat("Camera Target X: %.2f", camera.target.x), 10, 10, 20, BLACK);
        DrawText(TextFormat("Camera Target Y: %.2f", camera.target.y), 10, 40, 20, BLACK);
        DrawText(TextFormat("Camera Zoom: %.2f", camera.zoom), 10, 70, 20, BLACK);
        DrawText(TextFormat("Paused: %s", cfg.is_paused ? "true" : "false"), 10, 100, 20, BLACK);
      }

      // Draw exclusively to the pause menu
      if (cfg.is_paused) {
        DrawRectangle(0, 0, screenWidth, screenHeight, Fade(BLACK, 0.6f));
        int clicked = cfg.showSettingsMenu ? DrawMenu(settingsMenu) : DrawMenu(pauseMenu);

        if (!cfg.showSettingsMenu) {
          switch (clicked) {
            case 0: cfg.is_paused = false; break;       // Resume
            case 1: cfg.showSettingsMenu = true; break; // Settings
            case 2: cfg.closeApp = true; break;         // Quit
          }
        } else {
          if (clicked == 0) { cfg.drawLines = !cfg.drawLines; }  // Toggle Lines
          if (clicked == 1) { cfg.showSettingsMenu = false; }    // Return
        }
      }

      // Draw exclusively during gameplay
      if (!cfg.is_paused) {

      }
    EndDrawing();
  }
  CloseWindow();
  return 0;
}

