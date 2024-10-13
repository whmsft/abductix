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
const int maxLevel = 10;
int screenWidth, screenHeight, offsetX, offsetY, tileSize;
int levelWidth, levelHeight;
int frame = 0;
int playerMapX, playerMapY, playerX, playerY;
vector<vector<int>> box, boxMap;
vector<vector<int>> placeholder, placeholderMap;
int attachedBoxIndex = -1;
bool nextLevel = false;

Image imageCornerTopLeft, imageCornerTopRight, imageCornerBottomLeft, imageCornerBottomRight, imageWallUp, imageWallDown, imageWallLeft, imageWallRight, imagePlayer, imagePurpleBox, imagePurplePlaceholder, imageGreenBox, imageGreenPlaceholder, imageBlueBox, imageBluePlaceholder, imageRedBox, imageRedPlaceholder;
Texture2D textureCornerTopLeft, textureCornerTopRight, textureCornerBottomLeft, textureCornerBottomRight, textureWallUp, textureWallDown, textureWallLeft, textureWallRight, texturePlayer, texturePurpleBox, texturePurplePlaceholder, textureGreenBox, textureGreenPlaceholder, textureBlueBox, textureBluePlaceholder, textureRedBox, textureRedPlaceholder;

void LoadTextures() {
    UnloadImage(imageCornerTopLeft);
    UnloadImage(imageCornerTopRight);
    UnloadImage(imageCornerBottomLeft);
    UnloadImage(imageCornerBottomRight);
    UnloadImage(imageWallUp);
    UnloadImage(imageWallDown);
    UnloadImage(imageWallLeft);
    UnloadImage(imageWallRight);
    UnloadImage(imagePlayer);
    UnloadImage(imagePurpleBox);
    UnloadImage(imagePurplePlaceholder);
    UnloadImage(imageGreenBox);
    UnloadImage(imageGreenPlaceholder);
    UnloadImage(imageBlueBox);
    UnloadImage(imageBluePlaceholder);
    UnloadImage(imageRedBox);
    UnloadImage(imageRedPlaceholder);
    imageCornerTopLeft = LoadImage("assets/corner_topleft.png");
    imageCornerTopRight = LoadImage("assets/corner_topright.png");
    imageCornerBottomLeft = LoadImage("assets/corner_bottomleft.png");
    imageCornerBottomRight = LoadImage("assets/corner_bottomright.png");
    imageWallUp = LoadImage("assets/wall_up.png");
    imageWallDown = LoadImage("assets/wall_down.png");
    imageWallLeft = LoadImage("assets/wall_left.png");
    imageWallRight = LoadImage("assets/wall_right.png");
    imagePlayer = LoadImage("assets/player.png");
    imagePurpleBox = LoadImage("assets/purple_box.png");
    imagePurplePlaceholder = LoadImage("assets/purple_placeholder.png");
    imageGreenBox = LoadImage("assets/green_box.png");
    imageGreenPlaceholder = LoadImage("assets/green_placeholder.png");
    imageBlueBox = LoadImage("assets/blue_box.png");
    imageBluePlaceholder = LoadImage("assets/blue_placeholder.png");
    imageRedBox = LoadImage("assets/red_box.png");
    imageRedPlaceholder = LoadImage("assets/red_placeholder.png");
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
    ImageResizeNN(&imageGreenBox, tileSize, tileSize);
    ImageResizeNN(&imageGreenPlaceholder, tileSize, tileSize);
    ImageResizeNN(&imageBlueBox, tileSize, tileSize);
    ImageResizeNN(&imageBluePlaceholder, tileSize, tileSize);
    ImageResizeNN(&imageRedBox, tileSize, tileSize);
    ImageResizeNN(&imageRedPlaceholder, tileSize, tileSize);
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
    UnloadTexture(textureGreenBox);
    UnloadTexture(textureGreenPlaceholder);
    UnloadTexture(textureBlueBox);
    UnloadTexture(textureBluePlaceholder);
    UnloadTexture(textureRedBox);
    UnloadTexture(textureRedPlaceholder);
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
    textureGreenBox = LoadTextureFromImage(imageGreenBox);
    textureGreenPlaceholder = LoadTextureFromImage(imageGreenPlaceholder);
    textureBlueBox = LoadTextureFromImage(imageBlueBox);
    textureBluePlaceholder = LoadTextureFromImage(imageBluePlaceholder);
    textureRedBox = LoadTextureFromImage(imageRedBox);
    textureRedPlaceholder = LoadTextureFromImage(imageRedPlaceholder);
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
    box.clear();
    boxMap.clear();
    placeholder.clear();
    placeholderMap.clear();
    box.resize(allLevelMap[to_string(level)]["box"].size());
    boxMap.resize(allLevelMap[to_string(level)]["box"].size());
    placeholder.resize(allLevelMap[to_string(level)]["holder"].size());
    placeholderMap.resize(allLevelMap[to_string(level)]["holder"].size());
    for (int i = 0; i < boxMap.size(); ++i) {
        boxMap[i].resize(3);
        boxMap[i][0] = allLevelMap[to_string(level)]["box"][i][0].get<int>();
        boxMap[i][1] = allLevelMap[to_string(level)]["box"][i][1].get<int>();
        boxMap[i][2] = allLevelMap[to_string(level)]["box"][i][2].get<int>();
        box[i].resize(2);
        box[i][0] = offsetX + boxMap[i][0] * tileSize;
        box[i][1] = offsetY + boxMap[i][1] * tileSize;
    }
    for (int i = 0; i < placeholderMap.size(); ++i) {
        placeholderMap[i].resize(3);
        placeholderMap[i][0] = allLevelMap[to_string(level)]["holder"][i][0].get<int>();
        placeholderMap[i][1] = allLevelMap[to_string(level)]["holder"][i][1].get<int>();
        placeholderMap[i][2] = allLevelMap[to_string(level)]["holder"][i][2].get<int>();
        placeholder[i].resize(2);
        placeholder[i][0] = offsetX + placeholderMap[i][0] * tileSize;
        placeholder[i][1] = offsetY + placeholderMap[i][1] * tileSize;
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
    UnloadTexture(textureGreenBox); UnloadTexture(textureGreenPlaceholder);
    CloseWindow();

    return 0;
}

void update() {
    if (nextLevel) {InitLevel(); nextLevel=false;}
    bool inputCheck = true;
    if (IsWindowResized()) {
        screenWidth = GetScreenWidth();
        screenHeight = GetScreenHeight();
        LoadTextures();
    }
    bool dragging = false;
    if ((IsKeyPressed(KEY_RIGHT) || GetGestureDetected()==GESTURE_SWIPE_RIGHT) && playerMapX+1<levelWidth) {playerMapX++;dragging=true;}
    if ((IsKeyPressed(KEY_LEFT) || GetGestureDetected()==GESTURE_SWIPE_LEFT) && playerMapX>0) {playerMapX--;dragging=true;}

    if (abs(playerX-(offsetX+playerMapX*tileSize)) < tileSize/4) playerX = offsetX+playerMapX*tileSize;
    if (abs(playerY-(offsetY+playerMapX*tileSize)) < tileSize/4) playerY = offsetY+playerMapY*tileSize;
    if (playerX < offsetX + playerMapX * tileSize) playerX += tileSize / 4;
    if (playerX > offsetX + playerMapX * tileSize) playerX -= tileSize / 4;
    satisfiedPlaceholders = 0;
    int indexToLift = -1;    
    for (int i = 0; i < boxMap.size(); ++i) {
        if (attachedBoxIndex!=i) tileMap[boxMap[i][1]][boxMap[i][0]] = 10+i;
        if (box[i][1]<=playerY+tileSize+1) attachedBoxIndex = i;
        if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON) && attachedBoxIndex==-1 && (playerMapX == boxMap[i][0]) && !dragging && inputCheck) {
            if (indexToLift == -1 or box[indexToLift][1]>box[i][1]) indexToLift=i;
        }
        if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON) && attachedBoxIndex==i && !dragging && indexToLift!=i && inputCheck && newDropMapY(boxMap[i][0], boxMap[i][1])!=-1) {
            tileMap[boxMap[i][1]][boxMap[i][0]] = 0;
            attachedBoxIndex=-1;
            boxMap[i][1] = newDropMapY(boxMap[i][0], boxMap[i][1]);
            inputCheck = false;
        }
        if (abs(box[i][0]-(offsetX+boxMap[i][0]*tileSize)) <= tileSize/4) box[i][0] = offsetX+boxMap[i][0]*tileSize;
        if (abs(box[i][1]-(offsetY+boxMap[i][1]*tileSize)) <= tileSize/4) box[i][1] = offsetY+boxMap[i][1]*tileSize;
        if (box[i][1] < offsetY + boxMap[i][1] * tileSize) box[i][1] += tileSize / 4;
        if (box[i][1] > offsetY + boxMap[i][1] * tileSize) box[i][1] -= tileSize / 4;
        if (playerMapY+1==boxMap[i][1] && attachedBoxIndex==i) {boxMap[i][0]=playerMapX;box[i][0]=playerX;}
        for (int j=0; j<placeholder.size(); ++j) {
            if (placeholder[j][0] == box[i][0] && placeholder[j][1] == box[i][1] && placeholderMap[j][2]==boxMap[i][2]) ++satisfiedPlaceholders;
        }
    }
    if (indexToLift!=-1) {
        tileMap[boxMap[indexToLift][1]][boxMap[indexToLift][0]] = 0;
        boxMap[indexToLift][1] = playerMapY+1;
    }
    if (satisfiedPlaceholders==placeholderMap.size()) nextLevel = true;
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
    for (int i = 0; i < placeholderMap.size(); ++i) {
        switch (placeholderMap[i][2]) {
            case 0: DrawTexture(texturePurplePlaceholder, placeholder[i][0], placeholder[i][1], WHITE); break;
            case 1: DrawTexture(textureGreenPlaceholder, placeholder[i][0], placeholder[i][1], WHITE); break;
            case 2: DrawTexture(textureBluePlaceholder, placeholder[i][0], placeholder[i][1], WHITE); break;
            case 3: DrawTexture(textureRedPlaceholder, placeholder[i][0], placeholder[i][1], WHITE); break;
        }
    }
    for (int i = 0; i < boxMap.size(); ++i) {
        switch (boxMap[i][2]) {
            case 0: DrawTexture(texturePurpleBox, box[i][0], box[i][1], WHITE); break;
            case 1: DrawTexture(textureGreenBox, box[i][0], box[i][1], WHITE); break;
            case 2: DrawTexture(textureBlueBox, box[i][0], box[i][1], WHITE); break;
            case 3: DrawTexture(textureRedBox, box[i][0], box[i][1], WHITE); break;
        }
    }
}