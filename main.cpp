#include <string>
#include "include/raylib.h"
#include "include/nlohmann/json.hpp"
using namespace std;

#if defined(PLATFORM_WEB)
  #include <emscripten/emscripten.h>
#endif

void update(), draw();
void gameLoop() { update(); BeginDrawing(); draw(); EndDrawing(); }

char* allLevelMapFile = LoadFileText("levels.json");
nlohmann::json allLevelMap = nlohmann::json::parse(allLevelMapFile);
vector<vector<int>> tileMap;
int level = 1;
int screenWidth, screenHeight, offsetX, offsetY, tileSize;
int levelWidth, levelHeight;
int frame = 0;
int playerMapX, playerMapY, playerX, playerY;
vector<vector<int>> purpleBox, purpleBoxMap;
vector<vector<int>> purplePlaceholder, purplePlaceholderMap;
bool isPurpleBoxAttached = false;
bool inputAllowed=true;

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
    for (int index=positionY; index<levelHeight; index++) {
        if (tileMap[index][positionX]!=0) {return index-1; break;}
    }
    return -1;
}

void InitLevel() {
    levelWidth = allLevelMap[to_string(level)]["width"].get<int>();
    levelHeight = allLevelMap[to_string(level)]["height"].get<int>();
    
    tileMap.resize(levelHeight);
    for (int i = 0; i < levelHeight; ++i) {
        tileMap[i].resize(levelWidth);
        for (int j = 0; j < levelWidth; ++j) {
            tileMap[i][j] = allLevelMap[to_string(level)]["tilemap"][i][j].get<int>();
        }
    }

    screenWidth = GetScreenWidth();
    screenHeight = GetScreenHeight();

    tileSize = screenWidth / (levelWidth + 2);
    offsetX = (screenWidth - (levelWidth * tileSize)) / 2;
    offsetY = (screenHeight - (levelHeight * tileSize)) / 2;

    ResizeAndRotateTextures(tileSize);

    playerMapX = allLevelMap[to_string(level)]["playerX"].get<int>();
    playerMapY = allLevelMap[to_string(level)]["playerY"].get<int>();

    purpleBox.resize(allLevelMap[to_string(level)]["purple"]["box"].size());
    purpleBoxMap.resize(allLevelMap[to_string(level)]["purple"]["box"].size());
    purplePlaceholder.resize(allLevelMap[to_string(level)]["purple"]["holder"].size());
    purplePlaceholderMap.resize(allLevelMap[to_string(level)]["purple"]["holder"].size());
    for (int i = 0; i < purpleBoxMap.size(); ++i) {
        purpleBoxMap[i].resize(2);
        purpleBoxMap[i][0] = allLevelMap[to_string(level)]["purple"]["box"][i][0].get<int>();
        purpleBoxMap[i][1] = allLevelMap[to_string(level)]["purple"]["box"][i][1].get<int>();
        purpleBox[i].resize(2);
        purpleBox[i][0] = offsetX + purpleBoxMap[i][0] * tileSize;
        purpleBox[i][1] = offsetY + purpleBoxMap[i][1] * tileSize;
    }
    for (int i = 0; i < purplePlaceholderMap.size(); ++i) {
        purplePlaceholderMap[i].resize(2);
        purplePlaceholderMap[i][0] = allLevelMap[to_string(level)]["purple"]["holder"][i][0].get<int>();
        purplePlaceholderMap[i][1] = allLevelMap[to_string(level)]["purple"]["holder"][i][1].get<int>();
        purplePlaceholder[i].resize(2);
        purplePlaceholder[i][0] = offsetX + purplePlaceholderMap[i][0] * tileSize;
        purplePlaceholder[i][1] = offsetY + purplePlaceholderMap[i][1] * tileSize;
    }
    playerX = offsetX + playerMapX * tileSize;
    playerY = offsetY + playerMapY * tileSize;
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
    if (IsKeyPressed(KEY_RIGHT) && playerMapX+1<levelWidth) playerMapX++;
    if (IsKeyPressed(KEY_LEFT) && playerMapX>0) playerMapX--;

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !isPurpleBoxAttached && (playerMapX == purpleBoxMapX) && inputAllowed) {
        purpleBoxMapY = playerMapY+1;
    }
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && isPurpleBoxAttached && inputAllowed) {
        purpleBoxMapY = newDropMapY(purpleBoxMapX, purpleBoxMapY);
    }
    
    if (abs(purpleBoxX-(offsetX+purpleBoxMapX*tileSize)) <= tileSize/4) {purpleBoxX = offsetX+purpleBoxMapX*tileSize; inputAllowed=true;}
    if (abs(purpleBoxY-(offsetY+purpleBoxMapY*tileSize)) <= tileSize/4) {purpleBoxY = offsetY+purpleBoxMapY*tileSize; inputAllowed=true;}
    if (abs(playerX-(offsetX+playerMapX*tileSize)) <= tileSize/4) {playerX = offsetX+playerMapX*tileSize; inputAllowed=true;}
    if (abs(playerY-(offsetY+playerMapX*tileSize)) <= tileSize/4) {playerY = offsetY+playerMapY*tileSize; inputAllowed=true;}
    if (playerX < offsetX + playerMapX * tileSize) {playerX += tileSize / 4; inputAllowed=false;}
    if (playerX > offsetX + playerMapX * tileSize) {playerX -= tileSize / 4; inputAllowed=false;}
    if (purpleBoxY < offsetY + purpleBoxMapY * tileSize) {purpleBoxY += tileSize / 4; inputAllowed=false;}
    if (purpleBoxY > offsetY + purpleBoxMapY * tileSize) {purpleBoxY -= tileSize / 4; inputAllowed=false;}
    if (playerMapY+1==purpleBoxMapY) {purpleBoxMapX=playerMapX;purpleBoxX=playerX;}
    isPurpleBoxAttached = (purpleBoxY<=playerY+tileSize+1);

    if (purplePlaceholderMapX==purpleBoxMapX && purplePlaceholderMapY==purpleBoxMapY) {level+=1; InitLevel();}
}

void draw() {
    ClearBackground(ColorFromHSV(302, .54, .52));
    for (nlohmann::json::size_type rowIndex = 0; rowIndex < tileMap.size(); ++rowIndex) {
        const auto& row = tileMap[rowIndex];
        for (nlohmann::json::size_type tileIndex = 0; tileIndex < row.size(); ++tileIndex) {
            const int tile = row[tileIndex];
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
    for (int i = 0; i < purpleHolderMap.size(); ++i) DrawTexture(texturePurplePlaceholder, purplePlaceholder[i][0], purplePlaceholder[i][1], WHITE);
    for (int i = 0; i < purpleBoxMap.size(); ++i) DrawTexture(texturePurpleBox, purpleBox[i][1], purpleBox[i][1], WHITE);
}