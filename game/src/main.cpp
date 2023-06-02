#include <iostream>
#include <vector>
#include "raylib.h"
#include "rlImGui.h"

// Rigidbody class
class Rigidbody {
public:
    Vector2 position;
    Vector2 velocity;

    Rigidbody(float posX = 0.0f, float posY = 0.0f, float velX = 0.0f, float velY = 0.0f) : position{ posX, posY }, velocity{ velX, velY } {}
};

// Agent class
class Agent {
public:
    Rigidbody rigidbody;
    // Add any extra data needed for the agent
    std::string sprite;
    float maxSpeed;
    float maxAcceleration;

    Agent(float posX = 0.0f, float posY = 0.0f, float velX = 0.0f, float velY = 0.0f, const std::string& spr = "", float maxSpd = 0.0f, float maxAcc = 0.0f)
        : rigidbody(posX, posY, velX, velY), sprite(spr), maxSpeed(maxSpd), maxAcceleration(maxAcc) {}
};

// Update function
void update(Rigidbody& rigidbody, float deltaTime) {
    // Calculate the next frame state of the Rigidbody object
    rigidbody.position.x += rigidbody.velocity.x * deltaTime;
    rigidbody.position.y += rigidbody.velocity.y * deltaTime;
}

// Flee function
Vector2 flee(Vector2 agentPos, Vector2 agentVel, Vector2 targetPos, float maxAcc, float distanceFactor) {
    Vector2 desiredVelocity = { agentPos.x - targetPos.x, agentPos.y - targetPos.y };
    float desiredVelocityLength = sqrtf(desiredVelocity.x * desiredVelocity.x + desiredVelocity.y * desiredVelocity.y);

    if (desiredVelocityLength > 0.0f) {
        desiredVelocity.x /= desiredVelocityLength;
        desiredVelocity.y /= desiredVelocityLength;
    }

    desiredVelocity.x *= maxAcc * distanceFactor;
    desiredVelocity.y *= maxAcc * distanceFactor;

    Vector2 acceleration = { desiredVelocity.x - agentVel.x, desiredVelocity.y - agentVel.y };

    float accelerationLength = sqrtf(acceleration.x * acceleration.x + acceleration.y * acceleration.y);
    if (accelerationLength > maxAcc) {
        acceleration.x *= maxAcc / accelerationLength;
        acceleration.y *= maxAcc / accelerationLength;
    }

    return acceleration;
}

// Seek function
Vector2 seek(Vector2 agentPos, Vector2 agentVel, Vector2 targetPos, float maxAcc, float distanceFactor) {
    Vector2 desiredVelocity = { targetPos.x - agentPos.x, targetPos.y - agentPos.y };
    float desiredVelocityLength = sqrtf(desiredVelocity.x * desiredVelocity.x + desiredVelocity.y * desiredVelocity.y);

    if (desiredVelocityLength > 0.0f) {
        desiredVelocity.x /= desiredVelocityLength;
        desiredVelocity.y /= desiredVelocityLength;
    }

    desiredVelocity.x *= maxAcc * distanceFactor;
    desiredVelocity.y *= maxAcc * distanceFactor;

    Vector2 acceleration = { desiredVelocity.x - agentVel.x, desiredVelocity.y - agentVel.y };

    float accelerationLength = sqrtf(acceleration.x * acceleration.x + acceleration.y * acceleration.y);
    if (accelerationLength > maxAcc) {
        acceleration.x *= maxAcc / accelerationLength;
        acceleration.y *= maxAcc / accelerationLength;
    }

    return acceleration;
}

int main(void) {
    const int screenWidth = 1280;
    const int screenHeight = 720;

    InitWindow(screenWidth, screenHeight, "Sunshine");

    SetTargetFPS(60);

    // Create a container of Rigidbody objects
    std::vector<Rigidbody> rigidbodies;

    // Create a container of Agent objects
    std::vector<Agent> agents;

    // Add some agents to the container
    agents.emplace_back(0.0f, 0.0f, 0.0f, 0.0f, "Agent1", 800.0f, 800.0f);
    agents.emplace_back(0.0f, 0.0f, 0.0f, 0.0f, "Agent2", 750.0f, 750.0f);

    // Populate rigidbodies container with the Rigidbody objects from agents
    for (const auto& agent : agents) {
        rigidbodies.push_back(agent.rigidbody);
    }

    // Initialize target position
    Vector2 targetPosition = { 0.0f, 0.0f };

    // Simulation variables
    float deltaTime = 0.01f;  // Time step for simulation

    while (!WindowShouldClose()) {
        // Update
        for (size_t i = 0; i < rigidbodies.size(); ++i) {
            auto& rigidbody = rigidbodies[i];
            auto& agent = agents[i];

            if (i == 0) {
                // Flee logic for the first agent (circle)
                Vector2 fleeAcceleration = flee(rigidbody.position, rigidbody.velocity, targetPosition, agent.maxAcceleration, 0.5f);
                rigidbody.velocity.x += fleeAcceleration.x * deltaTime;
                rigidbody.velocity.y += fleeAcceleration.y * deltaTime;
            }
            else if (i == 1) {
                // Seek logic for the second agent (rectangle)
                Vector2 seekAcceleration = seek(rigidbody.position, rigidbody.velocity, targetPosition, agent.maxAcceleration, 0.2f);
                rigidbody.velocity.x += seekAcceleration.x * deltaTime;
                rigidbody.velocity.y += seekAcceleration.y * deltaTime;
            }

            if (rigidbody.velocity.x > agent.maxSpeed) {
                rigidbody.velocity.x = agent.maxSpeed;
            }
            else if (rigidbody.velocity.x < -agent.maxSpeed) {
                rigidbody.velocity.x = -agent.maxSpeed;
            }

            if (rigidbody.velocity.y > agent.maxSpeed) {
                rigidbody.velocity.y = agent.maxSpeed;
            }
            else if (rigidbody.velocity.y < -agent.maxSpeed) {
                rigidbody.velocity.y = -agent.maxSpeed;
            }

            update(rigidbody, deltaTime);
            // Perform any additional logic for the agent
        }

        // Update target position with mouse position
        targetPosition.x = GetMouseX();
        targetPosition.y = GetMouseY();

        // Render
        BeginDrawing();

        ClearBackground(RAYWHITE);

        // Render agents (just a simple rendering logic here)
        for (const auto& rigidbody : rigidbodies) {
            DrawRectangle(static_cast<int>(rigidbody.position.x), static_cast<int>(rigidbody.position.y), 50, 50, BLACK);
        }

        // Render target (as a circle)
        DrawCircle(static_cast<int>(targetPosition.x), static_cast<int>(targetPosition.y), 25, RED);

        EndDrawing();
    }

    CloseWindow();

    return 0;
}
