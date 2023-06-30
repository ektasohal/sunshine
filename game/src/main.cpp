#include "raylib.h"
#include <vector>
#include <queue>
#include <iostream>
#include <sstream>

#define SCREEN_WIDTH 1000
#define SCREEN_HEIGHT 1000
#define TILE_SIZE 80

struct Tile {
    Vector2 position;
    Color color;
    bool isWall;
    std::vector<Tile*> connectedTiles;
    float costToReach;
    Tile* previousNode;
    bool visited;
    bool operator==(const Tile& other) const {
        return position.x == other.position.x && position.y == other.position.y;
    }
};

struct Character {
    Vector2 position;
    Rectangle bounds;
    float speed;
};

std::vector<Tile> tiles;
Character character;
Tile* startNode;
Tile* goalNode;
Tile* currentNode;
std::vector<Vector2> playerPath; // Stores player's path

std::string intToString(int value) {
    std::stringstream ss;
    ss << value;
    return ss.str();
}

void generateTiles() {
    const int rows = 8;
    const int cols = 8;

    int totalTiles = rows * cols;
    int wallCount = static_cast<int>(totalTiles * 0.2);

    for (int i = 0; i < totalTiles; ++i) {
        Vector2 position = { 100 + (i % cols) * TILE_SIZE, 100 + (i / cols) * TILE_SIZE };
        Color color = { static_cast<unsigned char>(GetRandomValue(50, 205)),
                        static_cast<unsigned char>(GetRandomValue(50, 205)),
                        static_cast<unsigned char>(GetRandomValue(50, 205)),
                        255 };
        tiles.push_back({ position, color, false, {}, FLT_MAX, nullptr, false });
    }

    for (int i = 0; i < wallCount; ++i) {
        int randomIndex = GetRandomValue(0, totalTiles - 1);
        Tile& tile = tiles[randomIndex];
        if (!tile.isWall) {
            tile.isWall = true;
            tile.color = WHITE;
        }
        else {
            --i;
        }
    }
    for (int i = 0; i < totalTiles; ++i) {
        Tile& tile = tiles[i];

        int row = i / cols;
        int col = i % cols;

        if (row > 0) {
            tile.connectedTiles.push_back(&tiles[(row - 1) * cols + col]); // Connect to tile above
        }
        if (row < rows - 1) {
            tile.connectedTiles.push_back(&tiles[(row + 1) * cols + col]); // Connect to tile below
        }
        if (col > 0) {
            tile.connectedTiles.push_back(&tiles[row * cols + (col - 1)]); // Connect to tile on the left
        }
        if (col < cols - 1) {
            tile.connectedTiles.push_back(&tiles[row * cols + (col + 1)]); // Connect to tile on the right
        }
    }
}


void drawAdjacency(const std::vector<Tile>& tiles) {
    const Color lineColor = GREEN;
    const Color wallColor = RED;

    for (const Tile& tile : tiles) {
        if (tile.isWall) {
            DrawRectangle(static_cast<int>(tile.position.x), static_cast<int>(tile.position.y), TILE_SIZE, TILE_SIZE, wallColor);
        }
        else {
            DrawRectangle(static_cast<int>(tile.position.x), static_cast<int>(tile.position.y), TILE_SIZE, TILE_SIZE, tile.color);
        }

        Vector2 circleCenter = { tile.position.x + TILE_SIZE / 2, tile.position.y + TILE_SIZE / 2 };
        DrawCircle(static_cast<int>(circleCenter.x), static_cast<int>(circleCenter.y), TILE_SIZE / 4, GREEN);

        for (const Tile* connectedTile : tile.connectedTiles) {
            Vector2 connectedCenter = { connectedTile->position.x + TILE_SIZE / 2, connectedTile->position.y + TILE_SIZE / 2 };
            DrawLineEx(circleCenter, connectedCenter, 2, lineColor);
        }
    }

    // Draw line on player's path
    if (playerPath.size() > 1) {
        for (size_t i = 0; i < playerPath.size() - 1; ++i) {
            Vector2 currentPos = playerPath[i];
            Vector2 nextPos = playerPath[i + 1];
            Vector2 lineStart = { currentPos.x + TILE_SIZE / 2, currentPos.y + TILE_SIZE / 2 };
            Vector2 lineEnd = { nextPos.x + TILE_SIZE / 2, nextPos.y + TILE_SIZE / 2 };
            DrawLineEx(lineStart, lineEnd, 2, RED);
        }
    }
}


void initCharacter() {
    character.position = tiles[0].position;
    character.bounds = { character.position.x + 10, character.position.y + 10, TILE_SIZE - 20, TILE_SIZE - 20 };
    character.speed = 2.0f;
}

void updateCharacterMovement() {
    if (IsKeyDown(KEY_W) && character.position.y > 100) {
        character.position.y -= character.speed;
        character.bounds.y -= character.speed;
    }
    else if (IsKeyDown(KEY_S) && character.position.y < (100 + TILE_SIZE * 7)) {
        character.position.y += character.speed;
        character.bounds.y += character.speed;
    }

    if (IsKeyDown(KEY_A) && character.position.x > 100) {
        character.position.x -= character.speed;
        character.bounds.x -= character.speed;
    }
    else if (IsKeyDown(KEY_D) && character.position.x < (100 + TILE_SIZE * 7)) {
        character.position.x += character.speed;
        character.bounds.x += character.speed;
    }
}

void runDijkstra() {
    std::priority_queue<std::pair<float, Tile*>, std::vector<std::pair<float, Tile*>>, std::greater<std::pair<float, Tile*>>> pq;
    pq.push({ 0, startNode });

    while (!pq.empty()) {
        Tile* current = pq.top().second;
        pq.pop();

        if (current == goalNode) {
            break;
        }

        current->visited = true;

        for (Tile* neighbor : current->connectedTiles) {
            if (!neighbor->visited && !neighbor->isWall) {
                float newCost = current->costToReach + 1;
                if (newCost < neighbor->costToReach) {
                    neighbor->costToReach = newCost;
                    neighbor->previousNode = current;
                    pq.push({ newCost, neighbor });
                }
            }
        }

        currentNode = current;
    }
}

void visualizeDijkstra() {
    const Color pathColor = PINK;
    const Color visitedColor = GREEN;
    const Color textColor = GRAY;

    std::vector<Tile>& mutableTiles = const_cast<std::vector<Tile>&>(tiles);

    Tile* pathNode = goalNode;

    while (pathNode != nullptr) {
        mutableTiles[pathNode - &tiles[0]].color = pathColor;
        pathNode = pathNode->previousNode;
    }

    for (Tile& tile : mutableTiles) {
        if (tile.isWall) {
            DrawRectangle(static_cast<int>(tile.position.x), static_cast<int>(tile.position.y), TILE_SIZE, TILE_SIZE, RED);
        }
        else if (tile.visited) {
            DrawRectangle(static_cast<int>(tile.position.x), static_cast<int>(tile.position.y), TILE_SIZE, TILE_SIZE, visitedColor);
            DrawText(intToString(static_cast<int>(tile.costToReach)).c_str(), static_cast<int>(tile.position.x + TILE_SIZE / 2 - MeasureText(intToString(static_cast<int>(tile.costToReach)).c_str(), 20) / 2), static_cast<int>(tile.position.y + TILE_SIZE / 2 - 10), 20, textColor);
        }
        else {
            DrawRectangle(static_cast<int>(tile.position.x), static_cast<int>(tile.position.y), TILE_SIZE, TILE_SIZE, tile.color);
            DrawText(intToString(static_cast<int>(tile.costToReach)).c_str(), static_cast<int>(tile.position.x + TILE_SIZE / 2 - MeasureText(intToString(static_cast<int>(tile.costToReach)).c_str(), 20) / 2), static_cast<int>(tile.position.y + TILE_SIZE / 2 - 10), 20, textColor);
        }
    }

    // Draw line on player's path
    if (playerPath.size() > 1) {
        for (size_t i = 0; i < playerPath.size() - 1; ++i) {
            Vector2 currentPos = playerPath[i];
            Vector2 nextPos = playerPath[i + 1];
            Vector2 lineStart = { currentPos.x + TILE_SIZE / 2, currentPos.y + TILE_SIZE / 2 };
            Vector2 lineEnd = { nextPos.x + TILE_SIZE / 2, nextPos.y + TILE_SIZE / 2 };
            DrawLineEx(lineStart, lineEnd, 2, pathColor);
        }
    }
}


void resetTiles() {
    for (Tile& tile : tiles) {
        tile.visited = false;
        tile.costToReach = FLT_MAX;
        tile.previousNode = nullptr;
    }

    playerPath.clear();
}

int main() {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Dijkstra's Algorithm");

    generateTiles();

    startNode = &tiles[0];
    goalNode = &tiles[63];
    currentNode = startNode;

    initCharacter();

    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        updateCharacterMovement();

        for (Tile& tile : tiles) {
            if (CheckCollisionRecs(character.bounds, { tile.position.x, tile.position.y, TILE_SIZE, TILE_SIZE })) {
                if (tile.isWall) {
                    character.position = currentNode->position;
                    character.bounds.x = currentNode->position.x + 10;
                    character.bounds.y = currentNode->position.y + 10;
                }
                else {
                    currentNode = &tile;
                    playerPath.push_back(currentNode->position); // Store player's path
                }
            }
        }

       /* for (Tile& tile : tiles) {
            tile.color = WHITE;
        }
        */
        for (Tile& tile : tiles) {
            tile.visited = false;
            tile.costToReach = FLT_MAX;
            tile.previousNode = nullptr;
        }

        resetTiles();
        runDijkstra();
        visualizeDijkstra();
        drawAdjacency(tiles);

        DrawRectangle(static_cast<int>(character.bounds.x), static_cast<int>(character.bounds.y), static_cast<int>(character.bounds.width), static_cast<int>(character.bounds.height), YELLOW);
        DrawRectangleRec(character.bounds, BLUE);

        EndDrawing();
    }

    CloseWindow();

    return 0;
}