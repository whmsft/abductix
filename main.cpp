#include <string> // to_string
// #include <vector>
#include "include/raylib.h"
#include "include/nlohmann/json.hpp"
using namespace std;

#if defined(PLATFORM_WEB) 
  #include <emscripten/emscripten.h>
#endif

void update(), draw();
void gameLoop() {update();BeginDrawing();draw();EndDrawing();}

char* levelMapFile = LoadFileText("levels.json");
nlohmann::json levelMap = nlohmann::json::parse(levelMapFile);
int level = 1;
int screenWidth, screenHeight, offsetX, offsetY, playerX, playerY;
int levelWidth, levelHeight;

int main(void) {
  SetConfigFlags(FLAG_WINDOW_RESIZABLE);
  #if defined(PLATFORM_WEB)
    InitWindow(320, 640, "abductix");
    emscripten_set_main_loop(gameLoop, 30, 1);
  #else
    InitWindow(320, 640, "abductix");
    //InitWindow(GetMonitorWidth(GetCurrentMonitor()), GetMonitorHeight(GetCurrentMonitor()), "abductix");
    //ToggleFullscreen();
    SetTargetFPS(30);
    while (!WindowShouldClose()) gameLoop();
  #endif
  CloseWindow();
  return 0;
}

void update() {
  screenWidth = GetScreenWidth();
  screenHeight = GetScreenHeight();
  levelWidth = levelMap[to_string(level)]["width"].get<int>();
  levelHeight = levelMap[to_string(level)]["height"].get<int>();
  playerX = levelMap[to_string(level)]["playerX"];
  offsetX = screenWidth/(2*levelWidth+2);
  offsetY = (screenHeight/2)-((levelHeight*(screenWidth/(levelWidth+1)))/2);
}

void draw() {
  ClearBackground(BLACK);
  // DrawText(TextFormat("PlayerX: %i",playerX), 10, 10, 20, WHITE);
  for (size_t rowIndex = 0; rowIndex < levelMap[to_string(level)]["tilemap"].size(); ++rowIndex) {
    const auto& row = levelMap[to_string(level)]["tilemap"][rowIndex];
    for (size_t tileIndex = 0; tileIndex < row.size(); ++tileIndex) {
      const auto& tile = row[tileIndex];
      if (tile.get<int>() != 0) {
        DrawRectangle(offsetX+tileIndex*(screenWidth/(levelWidth+1)), offsetY+rowIndex*(screenWidth/(levelWidth+1)), (screenWidth/(levelWidth+1))-1, (screenWidth/(levelWidth+1))-1, WHITE);
      }
    }
  }
}