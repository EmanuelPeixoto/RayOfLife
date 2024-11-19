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
            .r = (unsigned char)(((((float)(i + j) / (width + height)) * 255)-255)*-1),
            .g = (unsigned char)(((((float)(i + j) / (width + height)) * 255)-255)*-1),
            .b = (unsigned char)(((((float)(i + j) / (width + height)) * 255)-255)*-1),
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
