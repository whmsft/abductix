#include <string>
#include "include/raylib.h"
#include "include/nlohmann/json.hpp"
using namespace std;

#if defined(PLATFORM_WEB)
  #include <emscripten/emscripten.h>
#endif

void update(), draw();
void gameLoop() { update(); BeginDrawing(); draw(); EndDrawing(); }

const char* allLevelMapFile = LoadFileText("levels.json");
const nlohmann::json allLevelMap = nlohmann::json::parse(allLevelMapFile);
vector<vector<int>> tileMap;
int level = 0;
int satisfiedPlaceholders = 0;
const int maxLevel = 2;
int screenWidth, screenHeight, offsetX, offsetY, tileSize;
int levelWidth, levelHeight;
int frame = 0;
int playerMapX, playerMapY, playerX, playerY;
vector<vector<int>> purpleBox, purpleBoxMap;
vector<vector<int>> purplePlaceholder, purplePlaceholderMap;
int attachedPurpleBoxIndex = -1;
bool inputAllowed=true;

Image imageCornerTopLeft = LoadImage("assets/corner_topleft.png");
Image imageCornerTopRight = LoadImage("assets/corner_topright.png");
Image imageCornerBottomLeft = LoadImage("assets/corner_bottomleft.png");
Image imageCornerBottomRight = LoadImage("assets/corner_bottomright.png");
Image imageWallUp = LoadImage("assets/wall_up.png");
Image imageWallDown = LoadImage("assets/wall_down.png");
Image imageWallLeft = LoadImage("assets/wall_left.png");
Image imageWallRight = LoadImage("assets/wall_right.png");
Image imagePlayer = LoadImage("assets/player.png");
Image imagePurpleBox = LoadImage("assets/purple_box.png");
Image imagePurplePlaceholder = LoadImage("assets/purple_placeholder.png");
Texture2D textureCornerTopLeft, textureCornerTopRight, textureCornerBottomLeft, textureCornerBottomRight, textureWallUp, textureWallDown, textureWallLeft, textureWallRight, texturePlayer, texturePurpleBox, texturePurplePlaceholder;

void LoadTextures() {
    ImageResizeNN(&imageCornerTopLeft, tileSize, tileSize);
    ImageResizeNN(&imageCornerTopRight, tileSize, tileSize);
    ImageResizeNN(&imageCornerBottomLeft, tileSize, tileSize);
    ImageResizeNN(&imageCornerBottomRight, tileSize, tileSize);
    ImageResizeNN(&imageWallUp, tileSize, tileSize);
    ImageResizeNN(&imageWallDown, tileSize, tileSize);
    ImageResizeNN(&imageWallLeft, tileSize, tileSize);
    ImageResizeNN(&imageWallRight, tileSize, tileSize);
    ImageResizeNN(&imagePlayer, tileSize, tileSize);
    ImageResizeNN(&imagePurpleBox, tileSize, tileSize);
    ImageResizeNN(&imagePurplePlaceholder, tileSize, tileSize);
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
    for (int index=positionY+1; index<levelHeight; index++) {
        if (tileMap[index][positionX]!=0) {return index-1; break;}
    }
    return -1;
}

void InitLevel() {
    if (level==maxLevel) level = 0;
    level+=1;
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

    LoadTextures();

    playerMapX = allLevelMap[to_string(level)]["playerX"].get<int>();
    playerMapY = allLevelMap[to_string(level)]["playerY"].get<int>();
    purpleBox.clear();
    purpleBoxMap.clear();
    purplePlaceholder.clear();
    purplePlaceholderMap.clear();
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
        LoadTextures();
    }
    if (IsKeyPressed(KEY_RIGHT) && playerMapX+1<levelWidth && inputAllowed) playerMapX++;
    if (IsKeyPressed(KEY_LEFT) && playerMapX>0 && inputAllowed) playerMapX--;

    if (abs(playerX-(offsetX+playerMapX*tileSize)) < tileSize/4) {playerX = offsetX+playerMapX*tileSize; inputAllowed=true;}
    if (abs(playerY-(offsetY+playerMapX*tileSize)) < tileSize/4) {playerY = offsetY+playerMapY*tileSize; inputAllowed=true;}
    if (playerX < offsetX + playerMapX * tileSize) {playerX += tileSize / 4; inputAllowed=false;}
    if (playerX > offsetX + playerMapX * tileSize) {playerX -= tileSize / 4; inputAllowed=false;}
    satisfiedPlaceholders = 0;
    int indexToLift = -1;
    for (int i = 0; i < purpleBoxMap.size(); ++i) {
        if (attachedPurpleBoxIndex!=i) tileMap[purpleBoxMap[i][1]][purpleBoxMap[i][0]] = 10+i;
        if (purpleBox[i][1]<=playerY+tileSize+1) attachedPurpleBoxIndex = i;
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && attachedPurpleBoxIndex==-1 && (playerMapX == purpleBoxMap[i][0]) && inputAllowed) {
            if (indexToLift == -1) {indexToLift = i;} else if (purpleBoxMap[indexToLift][1]>purpleBoxMap[i][1]) {indexToLift=i;}
        }
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && attachedPurpleBoxIndex==i && inputAllowed) {
            tileMap[purpleBoxMap[i][1]][purpleBoxMap[i][0]] = 0;
            attachedPurpleBoxIndex=-1;
            purpleBoxMap[i][1] = newDropMapY(purpleBoxMap[i][0], purpleBoxMap[i][1]);
        }
        if (abs(purpleBox[i][0]-(offsetX+purpleBoxMap[i][0]*tileSize)) <= tileSize/4) {purpleBox[i][0] = offsetX+purpleBoxMap[i][0]*tileSize; inputAllowed=true;}
        if (abs(purpleBox[i][1]-(offsetY+purpleBoxMap[i][1]*tileSize)) <= tileSize/4) {purpleBox[i][1] = offsetY+purpleBoxMap[i][1]*tileSize; inputAllowed=true;}
        if (purpleBox[i][1] < offsetY + purpleBoxMap[i][1] * tileSize) {purpleBox[i][1] += tileSize / 4; inputAllowed=false;}
        if (purpleBox[i][1] > offsetY + purpleBoxMap[i][1] * tileSize) {purpleBox[i][1] -= tileSize / 4; inputAllowed=false;}
        if (playerMapY+1==purpleBoxMap[i][1] && attachedPurpleBoxIndex==i) {purpleBoxMap[i][0]=playerMapX;purpleBox[i][0]=playerX;}
        for (int j=0; j<purplePlaceholder.size(); ++j) {
            if (purplePlaceholder[j][0] == purpleBox[i][0] && purplePlaceholder[j][1] == purpleBox[i][1]) ++satisfiedPlaceholders;
        }
    }
    if (indexToLift!=-1) {
        tileMap[purpleBoxMap[indexToLift][1]][purpleBoxMap[indexToLift][0]] = 0;
        purpleBoxMap[indexToLift][1] = playerMapY+1;
    }
    if (satisfiedPlaceholders==purplePlaceholderMap.size()) InitLevel();
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
    for (int i = 0; i < purplePlaceholder.size(); ++i) {
        DrawTexture(texturePurplePlaceholder, purplePlaceholder[i][0], purplePlaceholder[i][1], WHITE);
        DrawText(TextFormat("satisfied: %i",satisfiedPlaceholders), 10, 10, 20, WHITE);
    }
    for (int i = 0; i < purpleBox.size(); ++i) {
        DrawTexture(texturePurpleBox, purpleBox[i][0], purpleBox[i][1], WHITE);
    }
    // DrawText(TextFormat("attachedPurpleBoxIndex: %i", attachedPurpleBoxIndex),10,10,20,WHITE);
}