#include "raylib.h"
#include "raymath.h"

#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

#define MAX_BUTTONS 5

typedef struct {
  Rectangle rect;
  const char *label;
} Button;

typedef enum {
  MENU_NONE,
  MENU_PAUSE,
  MENU_SETTINGS,
  MENU_DEBUG
} MenuState;

typedef struct {
  Button buttons[MAX_BUTTONS];
  int buttonCount;
  float verticalSpacing;
} Menu;

typedef struct Config {
  int screenWidth;
  int screenHeight;
  bool is_paused;
  bool drawLines;
  bool closeApp;
  bool debugText;
  bool debugGrid;
  bool debugChunkRenderer;
  bool isChunkOnScreen;
  MenuState currentMenu;
  MenuState lastMenu;
} Config;

#define BASE_GRID_SIZE 50

Menu CreateMenu(const char *labels[], int buttonCount, Config cfg) {
  Menu menu = { .buttonCount = buttonCount, .verticalSpacing = 10.0f };
  float buttonWidth = 200;
  float buttonHeight = 40;

  for (int i = 0; i < buttonCount; i++) {
    menu.buttons[i].rect = (Rectangle){
      (float)(cfg.screenWidth) / 2 - buttonWidth / 2,
      (float)(cfg.screenHeight) / 2 - ((buttonCount * buttonHeight) + ((buttonCount - 1) * menu.verticalSpacing)) / 2 + i * (buttonHeight + menu.verticalSpacing),
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

void draw_grid(Camera2D camera, Config cfg) {
  Vector2 topLeft = camera.target;
  Vector2 bottomRight = Vector2Add(camera.target, (Vector2){
    cfg.screenWidth / camera.zoom, 
    cfg.screenHeight / camera.zoom
  });

  if (cfg.debugGrid) {
    DrawCircleV(Vector2Add(topLeft, (Vector2){ 50, 50 }), 20, RED);
    DrawCircleV(Vector2Add(bottomRight, (Vector2){ -50, -50 }), 20, RED);
  }

  float startX = topLeft.x + BASE_GRID_SIZE;
  float startY = topLeft.y + BASE_GRID_SIZE;

  startX = floor(startX / BASE_GRID_SIZE) * BASE_GRID_SIZE;
  startY = floor(startY / BASE_GRID_SIZE) * BASE_GRID_SIZE;

  if (cfg.debugGrid) {
    DrawCircleV((Vector2){ startX, startY }, 10, BLUE);
  }

  for (float y = startY; y <= bottomRight.y + 1; y += BASE_GRID_SIZE) {
    Vector2 start = (Vector2){topLeft.x + 1, y};
    Vector2 end = (Vector2){bottomRight.x + 1, y};  // Offset by 1
    if (cfg.drawLines) {
      DrawLineV(start, end, LIGHTGRAY);
    }
    else {
      for (float x = startX; x <= bottomRight.x + 1; x += BASE_GRID_SIZE) {
        Vector2 worldPos = {x, y};
        Vector2 screenPos = worldPos;
        DrawCircleV(screenPos, 3.0f, LIGHTGRAY); // Circles HOOOGGGG FPS for whatever reason!!!
      }
    }
  }

  for (float x = startX; x <= bottomRight.x + 1; x += BASE_GRID_SIZE) {
    Vector2 start = (Vector2){x, topLeft.y + 1};  // Offset by 1
    Vector2 end = (Vector2){x, bottomRight.y + 1};  // Offset by 1
    if (cfg.drawLines) {
      DrawLineV(start, end, LIGHTGRAY);
    }
  }
}

void HandleControls(Config *cfg, Camera2D *camera) {
  if (IsKeyPressed(KEY_ONE)) { cfg->drawLines = !cfg->drawLines; }
  if (IsKeyPressed(KEY_TWO)) { cfg->debugText = !cfg->debugText; }
  if (IsKeyPressed(KEY_THREE)) { cfg->debugChunkRenderer = !cfg->debugChunkRenderer; }
  if (IsKeyPressed(KEY_ESCAPE)) { 
    if (cfg->is_paused) {
      cfg->lastMenu = cfg->currentMenu;
      cfg->is_paused = !cfg->is_paused; 
    } else if (!cfg->is_paused && cfg->lastMenu == MENU_NONE) {
      cfg->is_paused = !cfg->is_paused; 
      cfg->currentMenu = MENU_PAUSE;
    } else {
      cfg->is_paused = !cfg->is_paused;
      cfg->currentMenu = cfg->lastMenu;
    }
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

typedef union Block Block;
union Block {
  struct {
    uint8_t a : 1;
    uint8_t b : 1;
    uint8_t c : 1;
    uint8_t d : 1;
    uint8_t e : 1;
    uint8_t f : 1;
    uint8_t g : 1;
    uint8_t h : 1;
  } cells;
  unsigned char block_value;
};

#define CHUNK_SIZE 8
#define BLOCK_SIZE 8

typedef union Chunk Chunk;
union Chunk {
  struct {
    Block a;
    Block b;
    Block c;
    Block d;
    Block e;
    Block f;
    Block g;
    Block h;
  } blocks;
  uint64_t chunk_value;
};

#define MAX_NEIGHBOURS 8

typedef struct ChunkNode ChunkNode; 
struct ChunkNode {
  Chunk c;
  Chunk *Neighbours;
  int x, y;
};

void DrawChunkGridDebug(Camera2D camera, Config cfg) {
  float LOCAL_GRID = 400.0f;
  Vector2 topLeft = camera.target;
  Vector2 bottomRight = Vector2Add(camera.target, (Vector2){
    cfg.screenWidth / camera.zoom, 
    cfg.screenHeight / camera.zoom
  });

  float startX = topLeft.x + LOCAL_GRID;
  float startY = topLeft.y + LOCAL_GRID;

  startX = floor(startX / LOCAL_GRID) * LOCAL_GRID;
  startY = floor(startY / LOCAL_GRID) * LOCAL_GRID;

  for (float y = startY; y <= bottomRight.y + 1; y += LOCAL_GRID) {
    Vector2 start = (Vector2){topLeft.x + 1, y};
    Vector2 end = (Vector2){bottomRight.x + 1, y};  // Offset by 1
    for (float x = startX; x <= bottomRight.x + 1; x += LOCAL_GRID) {
      Vector2 worldPos = {x, y};
      Vector2 screenPos = worldPos;
      DrawCircleV(screenPos, 6.0f, BLUE);
    }
  }
}

int GetCell(uint64_t value, int cell) {
  return (value >> cell) & 1;  // Shift and mask the cell
}

void DrawBlock(uint64_t blockValue, Vector2 basePos) {
  const float CELL_SIZE = 50.0f;  // Each cell is 50x50 units
  
  for (int cell = 0; cell < 8; cell++) {
    int isActive = GetCell(blockValue, cell);
    if (isActive) {
      Vector2 cellPos = {basePos.x + cell * CELL_SIZE, basePos.y};
      DrawRectangleV(cellPos, (Vector2){CELL_SIZE, CELL_SIZE}, GREEN);  // Active cell
    }
  }
}

void DrawChunk(Chunk c, Vector2 basePos) {
  for (int block = 0; block < CHUNK_SIZE; block++) {
    uint64_t blockValue = c.chunk_value >> (block * BLOCK_SIZE);
    DrawBlock(blockValue, basePos);
    basePos.y += BASE_GRID_SIZE;
  }
}

void DrawChunkNodeDebug(Config *cfg, Camera2D camera, ChunkNode c) {
  const float LOCAL_GRID_SIZE = 400.0f;
  Vector2 topLeft = camera.target;
  Vector2 bottomRight = Vector2Add(camera.target, (Vector2){
    cfg->screenWidth / camera.zoom, 
    cfg->screenHeight / camera.zoom
  });

  Vector2 chunkPos = {
    c.x * LOCAL_GRID_SIZE,
    c.y * LOCAL_GRID_SIZE,
  };

 if (chunkPos.x + LOCAL_GRID_SIZE < topLeft.x || chunkPos.x > bottomRight.x ||
     chunkPos.y + LOCAL_GRID_SIZE < topLeft.y || chunkPos.y > bottomRight.y) {
    cfg->isChunkOnScreen = false;
    return;
  }

  cfg->isChunkOnScreen = true;
  DrawChunk(c.c, chunkPos);
  DrawCircleV(chunkPos, 9.0f, RED);
  Vector2 endPos = Vector2Add(chunkPos, (Vector2){ LOCAL_GRID_SIZE, LOCAL_GRID_SIZE});
  DrawCircleV(endPos, 9.0f, RED);
}

void FillChunk(Chunk *c) {
  for (int block = 0; block < CHUNK_SIZE; block++)
    c->chunk_value |= ((uint64_t)((uint8_t)rand() % 256) << (block * BLOCK_SIZE));
}

int main() {
  srand(time(NULL));
  Config cfg = { 800, 450, false, false, false, false, false, false, false, MENU_NONE, MENU_PAUSE};
  InitWindow(cfg.screenWidth, cfg.screenHeight, "Infinite grid and movement test");
  SetExitKey(KEY_NULL);

  Camera2D camera = { 0 };
  camera.zoom = 1.0f;
  // SetTargetFPS(60);

  const char *pauseLabels[] = { "Resume", "Settings", "Quit" };
  Menu pauseMenu = CreateMenu(pauseLabels, 3, cfg);

  const char *settingsLabels[] = { "Toggle Lines", "Debug Options", "Return" };
  Menu settingsMenu = CreateMenu(settingsLabels, 3, cfg);

  const char *debugLabels[] = { "Grid Markers", "Debug Text", "Debug Chunk Renderer", "Return" };
  Menu debugMenu = CreateMenu(debugLabels, 4, cfg);

  Chunk DebugChunk;
  FillChunk(&DebugChunk);
  ChunkNode DebugChunkNode = (ChunkNode){
    .c = DebugChunk, 
    .Neighbours = NULL, 
    .x = 0,
    .y = 0,
  };

  // Game Loop
  while (!WindowShouldClose()) {
    HandleControls(&cfg, &camera);

    BeginDrawing();
      ClearBackground(RAYWHITE);

      if (cfg.closeApp) { break; }
      /*Always Draw*/ {
        BeginMode2D(camera); {
          draw_grid(camera, cfg);
          if (!cfg.is_paused) 
            DebugChunkNode.c.chunk_value = 0;
          if (cfg.debugChunkRenderer) {
            if (!cfg.is_paused)
              FillChunk(&DebugChunkNode.c);
            DrawChunkGridDebug(camera, cfg);
            DrawChunkNodeDebug(&cfg, camera, DebugChunkNode);
         } else {
            cfg.isChunkOnScreen = false;
          }
        } EndMode2D();

        if (cfg.debugText) {
          DrawText(TextFormat("Camera Target X: %.2f", camera.target.x), 10, 10, 20, BLACK);
          DrawText(TextFormat("Camera Target Y: %.2f", camera.target.y), 10, 40, 20, BLACK);
          DrawText(TextFormat("Camera Zoom: %.2f", camera.zoom), 10, 70, 20, BLACK);
          DrawText(TextFormat("Paused: %s", cfg.is_paused ? "true" : "false"), 10, 100, 20, BLACK);
          DrawText(TextFormat("Debug Chunk Renderer: %s", cfg.debugChunkRenderer ? "on" : "off"), 10, 130, 20, BLACK);
          DrawText(TextFormat("Is Chunk On Screen: %s", cfg.isChunkOnScreen ? "true" : "false"), 10, 160, 20, BLACK);
        }

        if (cfg.isChunkOnScreen) {
          DrawRectangle(0, 0, cfg.screenWidth, cfg.screenHeight, Fade(MAROON, 0.5f));
        }
      }

      // Draw exclusively to the pause menu
      if (cfg.is_paused) {
        DrawRectangle(0, 0, cfg.screenWidth, cfg.screenHeight, Fade(BLACK, 0.6f));
        int clicked = -1;

        switch (cfg.currentMenu) {
          case MENU_PAUSE:
            switch (DrawMenu(pauseMenu)) {
              case 0: cfg.is_paused = false; break;
              case 1: cfg.currentMenu = MENU_SETTINGS; break;
              case 2: cfg.closeApp = true; break;
            } break;
          case MENU_SETTINGS:
            switch (DrawMenu(settingsMenu)) {
              case 0: cfg.drawLines = !cfg.drawLines; break;
              case 1: cfg.currentMenu = MENU_DEBUG; break;
              case 2: cfg.currentMenu = MENU_PAUSE; break;
            } break;
          case MENU_DEBUG:
            switch (DrawMenu(debugMenu)) {
              case 0: cfg.debugGrid = !cfg.debugGrid; break;
              case 1: cfg.debugText = !cfg.debugText; break;
              case 2: cfg.debugChunkRenderer = !cfg.debugChunkRenderer; break;
              case 3: cfg.currentMenu = MENU_SETTINGS; break;
            } break;
          default:
            break;
        }
      }
      // Draw exclusively during gameplay
      // I personally don't see a use case for this
      // if (!cfg.is_paused){}
    EndDrawing();
    DrawFPS(cfg.screenWidth - 100, cfg.screenHeight - 20);
  }
  CloseWindow();
  return 0;
}

