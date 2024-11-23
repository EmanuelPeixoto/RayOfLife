#include "raylib.h"

int main() {
  int width = 1920;
  int height = 1080;
  SetConfigFlags(FLAG_WINDOW_RESIZABLE);
  SetConfigFlags(FLAG_FULLSCREEN_MODE);
  SetTargetFPS(60);
  InitWindow(width, height, "Ray-of-Life");
  while (!WindowShouldClose()) {
    BeginDrawing();
      for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
          DrawPixel(i, j, (Color){ 
            .r = 255,
            .g = 0,
            .b = 0,
            .a = 255
          });
        }
      }
      DrawFPS(20, 20);
    EndDrawing();
  }
  CloseWindow();
  return 0;
}
