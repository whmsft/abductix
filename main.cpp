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

Image imageCornerTopLeft, imageCornerTopRight, imageCornerBottomLeft, imageCornerBottomRight, imageWallUp, imageWallDown, imageWallLeft, imageWallRight;
Texture2D textureCornerTopLeft, textureCornerTopRight, textureCornerBottomLeft, textureCornerBottomRight, textureWallUp, textureWallDown, textureWallLeft, textureWallRight;

void LoadTextures() {
    imageCornerTopLeft = LoadImage("assets/corner.png");
    imageCornerTopRight = LoadImage("assets/corner.png");
    imageCornerBottomLeft = LoadImage("assets/corner.png");
    imageCornerBottomRight = LoadImage("assets/corner.png");
    imageWallUp = LoadImage("assets/wall.png");
    imageWallDown = LoadImage("assets/wall.png");
    imageWallLeft = LoadImage("assets/wall.png");
    imageWallRight = LoadImage("assets/wall.png");

    // Set initial textures (they will be resized dynamically)
    textureCornerTopLeft = LoadTextureFromImage(imageCornerTopLeft);
    textureCornerTopRight = LoadTextureFromImage(imageCornerTopRight);
    textureCornerBottomLeft = LoadTextureFromImage(imageCornerBottomLeft);
    textureCornerBottomRight = LoadTextureFromImage(imageCornerBottomRight);
    textureWallUp = LoadTextureFromImage(imageWallUp);
    textureWallDown = LoadTextureFromImage(imageWallDown);
    textureWallLeft = LoadTextureFromImage(imageWallLeft);
    textureWallRight = LoadTextureFromImage(imageWallRight);
}

void ResizeAndRotateTextures(int newTileSize) {
    // Resize images to the new tile size
    ImageResizeNN(&imageCornerTopLeft, newTileSize, newTileSize);
    ImageResizeNN(&imageCornerTopRight, newTileSize, newTileSize);
    ImageResizeNN(&imageCornerBottomLeft, newTileSize, newTileSize);
    ImageResizeNN(&imageCornerBottomRight, newTileSize, newTileSize);
    ImageResizeNN(&imageWallUp, newTileSize, newTileSize);
    ImageResizeNN(&imageWallDown, newTileSize, newTileSize);
    ImageResizeNN(&imageWallLeft, newTileSize, newTileSize);
    ImageResizeNN(&imageWallRight, newTileSize, newTileSize);

    // Rotate images for corners and walls based on desired orientation
    ImageRotateCW(&imageCornerTopRight);      // Rotate top-right 90 degrees clockwise
    ImageRotateCCW(&imageCornerBottomLeft);    // Rotate bottom-left 90 degrees counterclockwise
    ImageRotateCW(&imageCornerBottomRight);   // Rotate bottom-right 180 degrees
    ImageRotateCW(&imageCornerBottomRight);   // Rotate bottom-right 180 degrees

    ImageRotateCW(&imageWallDown);            // Rotate wall down 180 degrees
    ImageRotateCW(&imageWallDown);            // Rotate wall down 180 degrees
    ImageRotateCCW(&imageWallLeft);            // Rotate wall left 90 degrees counterclockwise
    ImageRotateCW(&imageWallRight);           // Rotate wall right 90 degrees clockwise

    // Reload textures from resized images
    UnloadTexture(textureCornerTopLeft);  // Unload previous textures to avoid memory leaks
    UnloadTexture(textureCornerTopRight);
    UnloadTexture(textureCornerBottomLeft);
    UnloadTexture(textureCornerBottomRight);
    UnloadTexture(textureWallUp);
    UnloadTexture(textureWallDown);
    UnloadTexture(textureWallLeft);
    UnloadTexture(textureWallRight);

    textureCornerTopLeft = LoadTextureFromImage(imageCornerTopLeft);
    textureCornerTopRight = LoadTextureFromImage(imageCornerTopRight);
    textureCornerBottomLeft = LoadTextureFromImage(imageCornerBottomLeft);
    textureCornerBottomRight = LoadTextureFromImage(imageCornerBottomRight);
    textureWallUp = LoadTextureFromImage(imageWallUp);
    textureWallDown = LoadTextureFromImage(imageWallDown);
    textureWallLeft = LoadTextureFromImage(imageWallLeft);
    textureWallRight = LoadTextureFromImage(imageWallRight);
}

void InitGame() {
    levelWidth = levelMap[to_string(level)]["width"].get<int>();
    levelHeight = levelMap[to_string(level)]["height"].get<int>();
    
    screenWidth = GetScreenWidth();
    screenHeight = GetScreenHeight();
    
    // Calculate tile size based on screen dimensions and level size
    tileSize = screenWidth / (levelWidth + 2); // Add space on both sides for centering
    offsetX = (screenWidth - (levelWidth * tileSize)) / 2;  // Center horizontally
    offsetY = (screenHeight - (levelHeight * tileSize)) / 2; // Center vertically

    ResizeAndRotateTextures(tileSize);  // Resizing and rotating textures based on the new tile size
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
    InitGame();
    
    while (!WindowShouldClose()) gameLoop();
    #endif
    
    // Cleanup
    UnloadTexture(textureCornerTopLeft); UnloadTexture(textureCornerTopRight);
    UnloadTexture(textureCornerBottomLeft); UnloadTexture(textureCornerBottomRight);
    UnloadTexture(textureWallUp); UnloadTexture(textureWallDown);
    UnloadTexture(textureWallLeft); UnloadTexture(textureWallRight);
    CloseWindow();

    return 0;
}

void update() {
    if (IsWindowResized()) {
        screenWidth = GetScreenWidth();
        screenHeight = GetScreenHeight();
        InitGame();  // Recalculate offsets and resize textures on window resize
    }
}

void draw() {
    ClearBackground(ColorFromHSV(302, .54, .52));
    
    const auto& tilemap = levelMap[to_string(level)]["tilemap"];  // Cache the tilemap reference
    for (size_t rowIndex = 0; rowIndex < tilemap.size(); ++rowIndex) {
        const auto& row = tilemap[rowIndex];
        for (size_t tileIndex = 0; tileIndex < row.size(); ++tileIndex) {
            const int tile = row[tileIndex].get<int>();
            int x = offsetX + tileIndex * tileSize;
            int y = offsetY + rowIndex * tileSize;
            
            // Simplify drawing by using a switch statement
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
}
