#include "raylib.h"
#include "raymath.h"
#include "rlgl.h"
int main() {
  int screenWidth = 800;
  int screenHeight = 450;
  InitWindow(screenWidth, screenHeight, "Minha janela");
  Camera2D camera = { 0 };
  camera.zoom = 1.0f;
  int zoomMode = 0;
  SetTargetFPS(60);
  while (!WindowShouldClose()) { 
    if (IsKeyPressed(KEY_ONE)) zoomMode = 0;
    else if (IsKeyPressed(KEY_TWO)) zoomMode = 1;
    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
      Vector2 delta = GetMouseDelta();
      delta = Vector2Scale(delta, -1.0f / camera.zoom);
      camera.target = Vector2Add(camera.target, delta); }
    if (zoomMode == 0) {
      float wheel = GetMouseWheelMove();
      if (wheel != 0) {
        Vector2 mouseWorldPos = GetScreenToWorld2D(GetMousePosition(), camera);
        camera.offset = GetMousePosition();
        camera.target = mouseWorldPos;
        float scaleFactor = 1.0f + (0.25f * fabsf(wheel));
        if (wheel < 0) scaleFactor = 1.0f/scaleFactor;
        camera.zoom = Clamp(camera.zoom * scaleFactor, 0.125f, 64.0f); }
    } else {
      if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
        Vector2 mouseWorldPos = GetScreenToWorld2D(GetMousePosition(), camera);
        camera.offset = GetMousePosition();
        camera.target = mouseWorldPos; }
      if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
        float deltaX = GetMouseDelta().x;
        float scaleFactor = 1.0f + (0.01f * fabsf(deltaX));
        if (deltaX < 0) scaleFactor = 1.0f / scaleFactor;
        camera.zoom =  Clamp(camera.zoom * scaleFactor, 0.125f, 64.0f); } }
    BeginDrawing();
      ClearBackground(RAYWHITE);
      BeginMode2D(camera);
        rlPushMatrix();
          rlTranslatef(0, 25 * 50, 0);
          rlRotatef(90, 1, 0, 0);
          DrawGrid(100, 50);
        rlPopMatrix();
        DrawCircle(GetScreenWidth() / 2, GetScreenHeight() / 2, 50, MAROON);
      EndMode2D();
      DrawCircleV(GetMousePosition(), 4, DARKGRAY);
      DrawTextEx(GetFontDefault(), TextFormat("[%i, %i]", GetMouseX(), GetMouseY()), Vector2Add(GetMousePosition(), (Vector2){ -44, -24 }), 20, 2, BLACK);
      DrawText("[1][2] Select mouse zoom mode (Wheel or Move)", 20, 20, 20, DARKGRAY);
      if (zoomMode == 0) DrawText("Mouse left button drag to move, mouse wheel to zoom", 20, 50, 20, DARKGRAY);
      else DrawText("Mouse left button drag to move, mouse press and move to zoom", 20, 50, 20, DARKGRAY);
    EndDrawing(); }
  CloseWindow();
  return 0; }
