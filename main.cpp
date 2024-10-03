#include <string>
#include "include/raylib.h"
#include "include/nlohmann/json.hpp"
using namespace std;

#if defined(PLATFORM_WEB)
  #include <emscripten/emscripten.h>
#endif

void update(), draw();
void gameLoop() { update(); BeginDrawing(); draw(); EndDrawing(); }

char* levelMapFile = LoadFileText("levels.json");
nlohmann::json levelMap = nlohmann::json::parse(levelMapFile);
int level = 1;
int screenWidth, screenHeight, offsetX, offsetY, tileSize;
int levelWidth, levelHeight;
int frame = 0;
int playerMapX, playerMapY, playerX, playerY;
int purpleBoxX, purpleBoxY, purpleBoxMapX, purpleBoxMapY;
int purplePlaceholderX, purplePlaceholderY, purplePlaceholderMapX, purplePlaceholderMapY;
bool isPurpleBoxAttached = false;

Image imageCornerTopLeft, imageCornerTopRight, imageCornerBottomLeft, imageCornerBottomRight, imageWallUp, imageWallDown, imageWallLeft, imageWallRight, imagePlayer, imagePurpleBox, imagePurplePlaceholder;
Texture2D textureCornerTopLeft, textureCornerTopRight, textureCornerBottomLeft, textureCornerBottomRight, textureWallUp, textureWallDown, textureWallLeft, textureWallRight, texturePlayer, texturePurpleBox, texturePurplePlaceholder;

void LoadTextures() {
    imageCornerTopLeft = LoadImage("assets/corner.png");
    imageCornerTopRight = LoadImage("assets/corner.png");
    imageCornerBottomLeft = LoadImage("assets/corner.png");
    imageCornerBottomRight = LoadImage("assets/corner.png");
    imageWallUp = LoadImage("assets/wall.png");
    imageWallDown = LoadImage("assets/wall.png");
    imageWallLeft = LoadImage("assets/wall.png");
    imageWallRight = LoadImage("assets/wall.png");
    imagePlayer = LoadImage("assets/player.png");
    imagePurpleBox = LoadImage("assets/purple.box.png");
    imagePurplePlaceholder = LoadImage("assets/purple.placeholder.png");

    textureCornerTopLeft = LoadTextureFromImage(imageCornerTopLeft);
    textureCornerTopRight = LoadTextureFromImage(imageCornerTopRight);
    textureCornerBottomLeft = LoadTextureFromImage(imageCornerBottomLeft);
    textureCornerBottomRight = LoadTextureFromImage(imageCornerBottomRight);
    textureWallUp = LoadTextureFromImage(imageWallUp);
    textureWallDown = LoadTextureFromImage(imageWallDown);
    textureWallLeft = LoadTextureFromImage(imageWallLeft);
    textureWallRight = LoadTextureFromImage(imageWallRight);
    texturePlayer = LoadTextureFromImage(imagePlayer);
    texturePurpleBox = LoadTextureFromImage(imagePurpleBox);
    texturePurplePlaceholder = LoadTextureFromImage(imagePurplePlaceholder);
}

void ResizeAndRotateTextures(int newTileSize) {
    ImageResizeNN(&imageCornerTopLeft, newTileSize, newTileSize);
    ImageResizeNN(&imageCornerTopRight, newTileSize, newTileSize);
    ImageResizeNN(&imageCornerBottomLeft, newTileSize, newTileSize);
    ImageResizeNN(&imageCornerBottomRight, newTileSize, newTileSize);
    ImageResizeNN(&imageWallUp, newTileSize, newTileSize);
    ImageResizeNN(&imageWallDown, newTileSize, newTileSize);
    ImageResizeNN(&imageWallLeft, newTileSize, newTileSize);
    ImageResizeNN(&imageWallRight, newTileSize, newTileSize);
    ImageResizeNN(&imagePlayer, newTileSize, newTileSize);
    ImageResizeNN(&imagePurpleBox, newTileSize, newTileSize);
    ImageResizeNN(&imagePurplePlaceholder, newTileSize, newTileSize);

    ImageRotateCW(&imageCornerTopRight);
    ImageRotateCCW(&imageCornerBottomLeft);
    ImageRotateCW(&imageCornerBottomRight);
    ImageRotateCW(&imageCornerBottomRight);

    ImageRotateCW(&imageWallDown);
    ImageRotateCW(&imageWallDown);
    ImageRotateCCW(&imageWallLeft);
    ImageRotateCW(&imageWallRight);

    UnloadTexture(textureCornerTopLeft);
    UnloadTexture(textureCornerTopRight);
    UnloadTexture(textureCornerBottomLeft);
    UnloadTexture(textureCornerBottomRight);
    UnloadTexture(textureWallUp);
    UnloadTexture(textureWallDown);
    UnloadTexture(textureWallLeft);
    UnloadTexture(textureWallRight);
    UnloadTexture(texturePlayer);
    UnloadTexture(texturePurpleBox);
    UnloadTexture(texturePurplePlaceholder);

    textureCornerTopLeft = LoadTextureFromImage(imageCornerTopLeft);
    textureCornerTopRight = LoadTextureFromImage(imageCornerTopRight);
    textureCornerBottomLeft = LoadTextureFromImage(imageCornerBottomLeft);
    textureCornerBottomRight = LoadTextureFromImage(imageCornerBottomRight);
    textureWallUp = LoadTextureFromImage(imageWallUp);
    textureWallDown = LoadTextureFromImage(imageWallDown);
    textureWallLeft = LoadTextureFromImage(imageWallLeft);
    textureWallRight = LoadTextureFromImage(imageWallRight);
    texturePlayer = LoadTextureFromImage(imagePlayer);
    texturePurpleBox = LoadTextureFromImage(imagePurpleBox);
    texturePurplePlaceholder = LoadTextureFromImage(imagePurplePlaceholder);
}

int newDropMapY(int positionX, int positionY) {
    for (int index=positionY; index<levelMap[to_string(level)]["height"].get<int>(); index++) {
        if (levelMap[to_string(level)]["tilemap"][index][positionX].get<int>()!=0) {return index-1; break;}
    }
    return -1;
}

void InitLevel() {
    levelWidth = levelMap[to_string(level)]["width"].get<int>();
    levelHeight = levelMap[to_string(level)]["height"].get<int>();
    
    screenWidth = GetScreenWidth();
    screenHeight = GetScreenHeight();

    tileSize = screenWidth / (levelWidth + 2);
    offsetX = (screenWidth - (levelWidth * tileSize)) / 2;
    offsetY = (screenHeight - (levelHeight * tileSize)) / 2;

    ResizeAndRotateTextures(tileSize);

    playerMapX = levelMap[to_string(level)]["playerX"].get<int>();
    playerMapY = levelMap[to_string(level)]["playerY"].get<int>();
    purpleBoxMapX = levelMap[to_string(level)]["purple"]["boxX"].get<int>();
    purpleBoxMapY = levelMap[to_string(level)]["purple"]["boxY"].get<int>();
    purplePlaceholderMapX = levelMap[to_string(level)]["purple"]["holderX"].get<int>();
    purplePlaceholderMapY = levelMap[to_string(level)]["purple"]["holderY"].get<int>();
    playerX = offsetX + playerMapX * tileSize;
    playerY = offsetY + playerMapY * tileSize;
    purpleBoxX = offsetX + purpleBoxMapX * tileSize;
    purpleBoxY = offsetY + purpleBoxMapY * tileSize;
    purplePlaceholderX = offsetX + purplePlaceholderMapX * tileSize;
    purplePlaceholderY = offsetY + purplePlaceholderMapY * tileSize;
}

int main(void) {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);

    #if defined(PLATFORM_WEB)
    InitWindow(320, 640, "abductix");
    emscripten_set_main_loop(gameLoop, 30, 1);
    #else
    InitWindow(320, 640, "abductix");
    SetTargetFPS(30);
    LoadTextures();
    InitLevel();
    
    while (!WindowShouldClose()) gameLoop();
    #endif
    
    UnloadTexture(textureCornerTopLeft); UnloadTexture(textureCornerTopRight);
    UnloadTexture(textureCornerBottomLeft); UnloadTexture(textureCornerBottomRight);
    UnloadTexture(textureWallUp); UnloadTexture(textureWallDown);
    UnloadTexture(textureWallLeft); UnloadTexture(textureWallRight);
    UnloadTexture(texturePlayer);
    UnloadTexture(texturePurpleBox); UnloadTexture(texturePurplePlaceholder);
    CloseWindow();

    return 0;
}

void update() {
    if (IsWindowResized()) {
        screenWidth = GetScreenWidth();
        screenHeight = GetScreenHeight();
        InitLevel();
    }
    if (IsKeyPressed(KEY_RIGHT) && playerMapX+1<levelMap[to_string(level)]["width"].get<int>()) playerMapX++;
    if (IsKeyPressed(KEY_LEFT) && playerMapX>0) playerMapX--;

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !isPurpleBoxAttached && (playerMapX == purpleBoxMapX)) {
        purpleBoxMapY = playerMapY+1;
    }
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && isPurpleBoxAttached) {
        purpleBoxMapY = newDropMapY(purpleBoxMapX, purpleBoxMapY);
    }
    
    if (playerX < offsetX + playerMapX * tileSize) playerX += tileSize / 10;
    if (playerX > offsetX + playerMapX * tileSize) playerX -= tileSize / 10;
    if (purpleBoxY < offsetY + purpleBoxMapY * tileSize) purpleBoxY += tileSize / 10;
    if (purpleBoxY > offsetY + purpleBoxMapY * tileSize) purpleBoxY -= tileSize / 10;
    if (playerMapY+1==purpleBoxMapY) purpleBoxX = playerX;
    if (purpleBoxY<=playerY+tileSize+1) {
        isPurpleBoxAttached = true;
    } else {isPurpleBoxAttached=false;}
}

void draw() {
    ClearBackground(ColorFromHSV(302, .54, .52));
    const auto& tilemap = levelMap[to_string(level)]["tilemap"];
    for (size_t rowIndex = 0; rowIndex < tilemap.size(); ++rowIndex) {
        const auto& row = tilemap[rowIndex];
        for (size_t tileIndex = 0; tileIndex < row.size(); ++tileIndex) {
            const int tile = row[tileIndex].get<int>();
            int x = offsetX + tileIndex * tileSize;
            int y = offsetY + rowIndex * tileSize;
        
            switch (tile) {
                case 1: DrawTexture(textureCornerTopLeft, x, y, WHITE); break;
                case 2: DrawTexture(textureCornerTopRight, x, y, WHITE); break;
                case 3: DrawTexture(textureCornerBottomLeft, x, y, WHITE); break;
                case 4: DrawTexture(textureCornerBottomRight, x, y, WHITE); break;
                case 5: DrawTexture(textureWallUp, x, y, WHITE); break;
                case 6: DrawTexture(textureWallDown, x, y, WHITE); break;
                case 7: DrawTexture(textureWallLeft, x, y, WHITE); break;
                case 8: DrawTexture(textureWallRight, x, y, WHITE); break;
                default: break;
            }
        }
    }

    DrawTexture(texturePlayer, playerX, playerY, WHITE);
    if (isPurpleBoxAttached) DrawTexture(texturePlayer, 0,0 , WHITE);
    DrawTexture(texturePurplePlaceholder, purplePlaceholderX, purplePlaceholderY, WHITE);
    DrawTexture(texturePurpleBox, purpleBoxX, purpleBoxY, WHITE);
}