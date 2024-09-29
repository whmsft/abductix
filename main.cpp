#include <string>
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

Image imageCornerTopLeft, imageCornerTopRight, imageCornerBottomLeft, imageCornerBottomRight = LoadImage("assets/corner.png");
Image imageWallUp, imageWallDown, imageWallLeft, imageWallRight = LoadImage("assets/wall.png");

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
  ImageResizeNN(&imageCornerTopLeft, screenWidth/(levelWidth+1), screenWidth/(levelWidth+1));
  ImageResizeNN(&imageCornerTopRight, screenWidth/(levelWidth+1), screenWidth/(levelWidth+1));
  ImageResizeNN(&imageCornerBottomLeft, screenWidth/(levelWidth+1), screenWidth/(levelWidth+1));
  ImageResizeNN(&imageCornerBottomRight, screenWidth/(levelWidth+1), screenWidth/(levelWidth+1));
  ImageResizeNN(&imageWallUp, screenWidth/(levelWidth+1), screenWidth/(levelWidth+1));
  ImageResizeNN(&imageWallDown, screenWidth/(levelWidth+1), screenWidth/(levelWidth+1));
  ImageResizeNN(&imageWallLeft, screenWidth/(levelWidth+1), screenWidth/(levelWidth+1));
  ImageResizeNN(&imageWallRight, screenWidth/(levelWidth+1), screenWidth/(levelWidth+1));
  ImageRotate(&imageCornerTopRight,90);
  //ImageRotateCW(&imageCornerTopRight);
  ImageRotate(&imageCornerBottomLeft,180);
  ImageRotate(&imageCornerBottomRight,270);
  ImageRotate(&imageWallDown,180);
  ImageRotate(&imageWallLeft,270);
  ImageRotate(&imageWallRight,90);
}

void draw() {
  Texture2D textureCornerTopLeft = LoadTextureFromImage(imageCornerTopLeft);
  Texture2D textureCornerTopRight = LoadTextureFromImage(imageCornerTopRight);
  Texture2D textureCornerBottomLeft = LoadTextureFromImage(imageCornerBottomLeft);
  Texture2D textureCornerBottomRight = LoadTextureFromImage(imageCornerBottomRight);
  Texture2D textureWallUp = LoadTextureFromImage(imageWallUp);
  Texture2D textureWallDown = LoadTextureFromImage(imageWallDown);
  Texture2D textureWallLeft = LoadTextureFromImage(imageWallLeft);
  Texture2D textureWallRight = LoadTextureFromImage(imageWallRight);
  ClearBackground(ColorFromHSV(302,.54,.52));
  for (size_t rowIndex = 0; rowIndex < levelMap[to_string(level)]["tilemap"].size(); ++rowIndex) {
    const auto& row = levelMap[to_string(level)]["tilemap"][rowIndex];
    for (size_t tileIndex = 0; tileIndex < row.size(); ++tileIndex) {
      const auto& tile = row[tileIndex];
      if (tile.get<int>() == 1) DrawTexture(textureCornerTopLeft, offsetX+tileIndex*(screenWidth/(levelWidth+1)), offsetY+rowIndex*(screenWidth/(levelWidth+1)), WHITE);
      if (tile.get<int>() == 2) DrawTexture(textureCornerTopRight, offsetX+tileIndex*(screenWidth/(levelWidth+1)), offsetY+rowIndex*(screenWidth/(levelWidth+1)), WHITE);
      if (tile.get<int>() == 3) DrawTexture(textureCornerBottomLeft, offsetX+tileIndex*(screenWidth/(levelWidth+1)), offsetY+rowIndex*(screenWidth/(levelWidth+1)), WHITE);
      if (tile.get<int>() == 4) DrawTexture(textureCornerBottomRight, offsetX+tileIndex*(screenWidth/(levelWidth+1)), offsetY+rowIndex*(screenWidth/(levelWidth+1)), WHITE);
      if (tile.get<int>() == 5) DrawTexture(textureWallUp, offsetX+tileIndex*(screenWidth/(levelWidth+1)), offsetY+rowIndex*(screenWidth/(levelWidth+1)), WHITE);
      if (tile.get<int>() == 6) DrawTexture(textureWallDown, offsetX+tileIndex*(screenWidth/(levelWidth+1)), offsetY+rowIndex*(screenWidth/(levelWidth+1)), WHITE);
      if (tile.get<int>() == 7) DrawTexture(textureWallLeft, offsetX+tileIndex*(screenWidth/(levelWidth+1)), offsetY+rowIndex*(screenWidth/(levelWidth+1)), WHITE);
      if (tile.get<int>() == 8) DrawTexture(textureWallRight, offsetX+tileIndex*(screenWidth/(levelWidth+1)), offsetY+rowIndex*(screenWidth/(levelWidth+1)), WHITE);
    }
  }
}
//272,80,48
//302,54,52