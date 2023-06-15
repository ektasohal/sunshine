#include "raylib.h"
#include <vector>

#define SCREEN_WIDTH 1000
#define SCREEN_HEIGHT 1000
#define TILE_SIZE 70

struct Tile {
    Vector2 position;
    Color color;
    bool isWall;
    std::vector<Tile*> connectedTiles;
};

struct Character {
    Vector2 position;
    Rectangle bounds;
    float speed;
};

std::vector<Tile> tiles;
Character character;

void generateTiles() {
    const int rows = 8;
    const int cols = 8;

    int totalTiles = rows * cols;
    int wallCount = static_cast<int>(totalTiles * 0.2); // 20% of total tiles as walls

    // Create all tiles as floors initially
    for (int i = 0; i < totalTiles; ++i) {
        Vector2 position = { 100 + (i % cols) * TILE_SIZE, 100 + (i / cols) * TILE_SIZE };
        Color color = { static_cast<unsigned char>(GetRandomValue(50, 205)),
                        static_cast<unsigned char>(GetRandomValue(50, 205)),
                        static_cast<unsigned char>(GetRandomValue(50, 205)),
                        255 };
        tiles.push_back({ position, color, false });
    }

    // Randomly assign walls
    for (int i = 0; i < wallCount; ++i) {
        int randomIndex = GetRandomValue(0, totalTiles - 1);
        Tile& tile = tiles[randomIndex];
        if (!tile.isWall) {
            tile.isWall = true;
            tile.color = RED;
        }
        else {
            // If the randomly chosen tile is already a wall, try again
            --i;
        }
    }
}

void drawAdjacency(const std::vector<Tile>& tiles) {
    const Color lineColor = GREEN;

    for (const Tile& tile : tiles) {
        DrawRectangle(static_cast<int>(tile.position.x), static_cast<int>(tile.position.y), TILE_SIZE, TILE_SIZE, tile.color);

        Vector2 circleCenter = { tile.position.x + TILE_SIZE / 2, tile.position.y + TILE_SIZE / 2 };
        DrawCircle(static_cast<int>(circleCenter.x), static_cast<int>(circleCenter.y), TILE_SIZE / 4, GREEN);

        for (const Tile* connectedTile : tile.connectedTiles) {
            Vector2 connectedCenter = { connectedTile->position.x + TILE_SIZE / 2, connectedTile->position.y + TILE_SIZE / 2 };
            DrawLineEx(circleCenter, connectedCenter, 2, lineColor);
        }
    }
}

void initCharacter() {
    character.position = tiles[0].position;
    character.bounds = { character.position.x + 10, character.position.y + 10, TILE_SIZE - 20, TILE_SIZE - 20 };
    character.speed = 2.0f;
}

void updateCharacterMovement() {
    // Move character based on key inputs (WASD)
    if (IsKeyDown(KEY_W) && character.position.y > 100) {
        character.position.y -= character.speed;
        character.bounds.y -= character.speed;
    }
    else if (IsKeyDown(KEY_S) && character.position.y < SCREEN_HEIGHT - TILE_SIZE - 100) {
        character.position.y += character.speed;
        character.bounds.y += character.speed;
    }
    if (IsKeyDown(KEY_A) && character.position.x > 100) {
        character.position.x -= character.speed;
        character.bounds.x -= character.speed;
    }
    else if (IsKeyDown(KEY_D) && character.position.x < SCREEN_WIDTH - TILE_SIZE - 100) {
        character.position.x += character.speed;
        character.bounds.x += character.speed;
    }

    // Check collision with walls
    for (const Tile& tile : tiles) {
        if (tile.isWall && CheckCollisionRecs(character.bounds, { tile.position.x, tile.position.y, TILE_SIZE, TILE_SIZE })) {
            // Undo the movement if there is a collision with a wall
            if (IsKeyDown(KEY_W)) {
                character.position.y += character.speed;
                character.bounds.y += character.speed;
            }
            else if (IsKeyDown(KEY_S)) {
                character.position.y -= character.speed;
                character.bounds.y -= character.speed;
            }
            if (IsKeyDown(KEY_A)) {
                character.position.x += character.speed;
                character.bounds.x += character.speed;
            }
            else if (IsKeyDown(KEY_D)) {
                character.position.x -= character.speed;
                character.bounds.x -= character.speed;
            }
            break;
        }
    }
}

void drawCharacter() {
    DrawRectangleRec(character.bounds, BLUE);
}

int main(void)
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Tile Connections");
    SetTargetFPS(60);

    generateTiles();
    initCharacter();

    // Define the adjacency connections between tiles
    const int rows = 8;
    const int cols = 8;

    for (int row = 0; row < rows; ++row) {
        for (int col = 0; col < cols; ++col) {
            int index = row * cols + col;
            Tile& tile = tiles[index];

            if (col > 0) {
                tile.connectedTiles.push_back(&tiles[index - 1]); // Connect to the left tile
            }
            if (col < cols - 1) {
                tile.connectedTiles.push_back(&tiles[index + 1]); // Connect to the right tile
            }
            if (row > 0) {
                tile.connectedTiles.push_back(&tiles[index - cols]); // Connect to the tile above
            }
            if (row < rows - 1) {
                tile.connectedTiles.push_back(&tiles[index + cols]); // Connect to the tile below
            }
        }
    }

    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        updateCharacterMovement();
        drawAdjacency(tiles);
        drawCharacter();

        EndDrawing();
    }

    CloseWindow();

    return 0;
}