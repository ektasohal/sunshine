#include "raylib.h"
#include <vector>
#include <cmath>
#include <random>
#include <raymath.h>
#include <rcamera.h>

#define WINDOW_WIDTH 1200
#define WINDOW_HEIGHT 800
#define MAX_SPEED 2.0f
#define MAX_ACCELERATION 0.03f
#define ARRIVAL_RADIUS 50.0f
#define AVOIDANCE_RADIUS 80.0f
#define AVOIDANCE_WEIGHT 0.6f
#define PREDATOR_AVOIDANCE_RADIUS 100.0f

const char* FISH_SPRITE_PATH ="C:/GitHub/sunshine/game/assets/Fish/peixe-listrado-03-md.png" ;

bool resetSimulation = false;
Vector2 foodPosition = { 0.0f, 0.0f };
Vector2 predatorPosition = { 0.0f, 0.0f };
bool predatorActive = false;
bool foodActive = false;
std::vector<Vector2> obstacles;

struct Fish {
    Vector2 position;
    Vector2 velocity;
    Vector2 acceleration;
    float maxSpeed;
    float maxAcceleration;
    Texture2D sprite;
    Rectangle sourceRect;
    bool reachedFood;
    bool eating;
    float maxFleeForce;


    Fish(float x, float y)
        : position({ x, y }), velocity({ 0.0f, 0.0f }), acceleration({ 0.0f, 0.0f }),
        maxSpeed(MAX_SPEED), maxAcceleration(MAX_ACCELERATION), reachedFood(false), eating(false) {
        sprite = LoadTexture(FISH_SPRITE_PATH);
        sourceRect = { 0.0f, 0.0f, static_cast<float>(sprite.width), static_cast<float>(sprite.height) };
    }

    void Update(const Vector2& target, const std::vector<Vector2>& obstacles);
    void Draw() const;
};

void Fish::Update(const Vector2& target, const std::vector<Vector2>& obstacles) {
    // Calculate desired velocity based on target position
    Vector2 desiredVelocity = Vector2Subtract(target, position);

    // Calculate steering force
    Vector2 steeringForce = Vector2Subtract(desiredVelocity, velocity);
    steeringForce = Vector2Scale(steeringForce, maxAcceleration);

    // Apply avoidance behavior
    for (const auto& obstacle : obstacles) {
        Vector2 avoidanceForce = Vector2Subtract(position, obstacle);
        float distance = Vector2Length(avoidanceForce);
        if (distance < AVOIDANCE_RADIUS) {
            avoidanceForce = Vector2Normalize(avoidanceForce);
            avoidanceForce = Vector2Scale(avoidanceForce, maxAcceleration * AVOIDANCE_WEIGHT);
            steeringForce = Vector2Add(steeringForce, avoidanceForce);
        }
    }

    // Update acceleration based on steering force
    acceleration = Vector2Add(acceleration, steeringForce);

    // Limit acceleration to maximum acceleration
    float accelerationLength = Vector2Length(acceleration);
    if (accelerationLength > maxAcceleration) {
        acceleration = Vector2Scale(acceleration, maxAcceleration / accelerationLength);
    }

    // Update velocity based on acceleration
    velocity = Vector2Add(velocity, acceleration);

    // Limit velocity to maximum speed
    float velocityLength = Vector2Length(velocity);
    if (velocityLength > maxSpeed) {
        velocity = Vector2Scale(velocity, maxSpeed / velocityLength);
    }

    // Update position based on velocity
    position = Vector2Add(position, velocity);

    // Clamp position to stay within screen boundaries
    position.x = Clamp(position.x, 0.0f, static_cast<float>(WINDOW_WIDTH));
    position.y = Clamp(position.y, 0.0f, static_cast<float>(WINDOW_HEIGHT));

    // Check if fish has reached the food
    float distanceToFood = Vector2Distance(position, foodPosition);
    if (distanceToFood < ARRIVAL_RADIUS) {
        if (!eating) {
            eating = true;
            // Remove the food
            foodActive = false;
        }
    }
    else {
        reachedFood = false;
        eating = false;
    }

    // Add the following code to remove the food when the fish is eating
    if (eating) {
        // Calculate the distance between the fish's mouth and the food
        float distanceToMouth = Vector2Distance(position, foodPosition);

        // If the distance is small, remove the food
        if (distanceToMouth < ARRIVAL_RADIUS) {
            foodActive = false;
        }
    }
}



void Fish::Draw() const {
    // Draw the fish sprite
    DrawTexturePro(sprite, sourceRect, { position.x, position.y, static_cast<float>(sprite.width), static_cast<float>(sprite.height) },
        { static_cast<float>(sprite.width) * 0.5f, static_cast<float>(sprite.height) * 0.5f }, 0.0f, WHITE);
}

int main() {
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Aquarium Simulation");
    SetTargetFPS(60);

    std::vector<Fish> fishes;

    // Create fish agents
    int numFish = 4;
    for (int i = 0; i < numFish; i++) {
        float x = static_cast<float>(GetRandomValue(50, WINDOW_WIDTH - 50));
        float y = static_cast<float>(GetRandomValue(50, WINDOW_HEIGHT - 50));
        Fish fish(x, y);
        fishes.push_back(fish);
    }

    // Variables for steering behaviors
    bool seekMode = false;
    bool fleeMode = false;
    bool arrivalMode = false;
    bool avoidanceMode = false;
    Vector2 targetPosition = { 0.0f, 0.0f };
    std::vector<Vector2> visibleObstacles;

    // Main game loop
    while (!WindowShouldClose()) {
        // Update
        if (IsKeyPressed(KEY_ZERO)) {
            // Mode 0: Do nothing
            seekMode = false;
            fleeMode = false;
            arrivalMode = false;
            avoidanceMode = false;
            obstacles.clear();
            predatorActive = false;
            foodActive = false;
        }
        else if (IsKeyPressed(KEY_ONE)) {
            // Mode 1: Seek
            seekMode = true;
            fleeMode = false;
            arrivalMode = false;
            avoidanceMode = false;
        }
        else if (IsKeyPressed(KEY_TWO)) {
            // Mode 2: Flee
            seekMode = false;
            fleeMode = true;
            arrivalMode = false;
            avoidanceMode = false;
        }
        else if (IsKeyPressed(KEY_THREE)) {
            // Mode 3: Arrival
            seekMode = false;
            fleeMode = false;
            arrivalMode = true;
            avoidanceMode = false;
        }
        else if (IsKeyPressed(KEY_FOUR)) {
            // Mode 4: Obstacle Avoidance
            seekMode = false;
            fleeMode = false;
            arrivalMode = false;
            avoidanceMode = true;
        }
        else if (IsKeyPressed(KEY_SPACE)) {
            // Reset simulation
            resetSimulation = true;
        }

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            Vector2 mousePosition = GetMousePosition();
            if (seekMode) {
                targetPosition = mousePosition;
            }
            else if (fleeMode) {
                predatorPosition = mousePosition;
                predatorActive = true;
            }
            else if (arrivalMode) {
                foodPosition = mousePosition;
                foodActive = true;
            }
            else if (avoidanceMode) {
                obstacles.push_back(mousePosition);
            }
        }

        if (resetSimulation) {
            // Reset simulation
            fishes.clear();
            for (int i = 0; i < numFish; i++) {
                float x = static_cast<float>(GetRandomValue(50, WINDOW_WIDTH - 50));
                float y = static_cast<float>(GetRandomValue(50, WINDOW_HEIGHT - 50));
                Fish fish(x, y);
                fishes.push_back(fish);
            }
            resetSimulation = false;
            targetPosition = { 0.0f, 0.0f };
            predatorPosition = { 0.0f, 0.0f };
            predatorActive = false;
            foodPosition = { 0.0f, 0.0f };
            foodActive = false;
            obstacles.clear();
        }

        // Update fish and steering behaviors
        for (Fish& fish : fishes) {
            visibleObstacles.clear();
            if (avoidanceMode) {
                // Calculate visible obstacles based on a vision cone
                for (const auto& obstacle : obstacles) {
                    float distance = Vector2Distance(fish.position, obstacle);
                    if (distance < AVOIDANCE_RADIUS) {
                        Vector2 heading = Vector2Normalize(fish.velocity);
                        Vector2 obstacleDirection = Vector2Normalize(Vector2Subtract(obstacle, fish.position));
                        float dotProduct = Vector2DotProduct(heading, obstacleDirection);
                        if (dotProduct > 0.8f) {
                            visibleObstacles.push_back(obstacle);
                        }
                    }
                }
            }

            Vector2 target = { 0.0f, 0.0f };
            if (seekMode)
            {
                target = targetPosition;
            }
            else if (fleeMode && predatorActive)
            {
                target = predatorPosition;
            }
            else if (arrivalMode && foodActive)
            {
                target = foodPosition;
            }

            fish.Update(target, visibleObstacles);
        }

        // Draw
        BeginDrawing();
        ClearBackground(RAYWHITE);

        for (const Fish& fish : fishes)
        {
            fish.Draw();
        }

        for (const auto& obstacle : obstacles) {
            DrawCircle(static_cast<int>(obstacle.x), static_cast<int>(obstacle.y), 10, RED);
        }

        if (predatorActive) {
            DrawCircle(static_cast<int>(predatorPosition.x), static_cast<int>(predatorPosition.y), 10, BLUE);
        }

        if (foodActive) {
            DrawCircle(static_cast<int>(foodPosition.x), static_cast<int>(foodPosition.y), 10, GREEN);
        }

        EndDrawing();
    }


    // Unload textures and close window
    for (Fish& fish : fishes) {
        UnloadTexture(fish.sprite);
    }
    CloseWindow();

    return 0;
}