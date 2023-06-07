#include <iostream>
#include <cmath>
#include <vector>
#include "raylib.h"

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

class Agent {
public:
    double x, y;  // Agent position

    Agent(double initialX, double initialY) {
        x = initialX;
        y = initialY;
    }

    void ObstacleAvoidance(const std::vector<Vector2>& obstacles, double whiskerLength, double avoidanceAngle) {
        double avoidanceDistance = whiskerLength;

        for (const auto& obstacle : obstacles) {
            double distance = std::sqrt(std::pow(obstacle.x - x, 2) + std::pow(obstacle.y - y, 2));

            if (distance <= avoidanceDistance) {
                double angleToObstacle = std::atan2(obstacle.y - y, obstacle.x - x);
                double desiredX = x + avoidanceDistance * std::cos(angleToObstacle + avoidanceAngle);
                double desiredY = y + avoidanceDistance * std::sin(angleToObstacle + avoidanceAngle);

                x = desiredX;
                y = desiredY;
            }
        }
    }

    void Seek(const Vector2& target, double speed) {
        double dx = target.x - x;
        double dy = target.y - y;
        double distance = std::sqrt(dx * dx + dy * dy);

        if (distance > 0) {
            double vx = (dx / distance) * speed;
            double vy = (dy / distance) * speed;

            x += vx;
            y += vy;
        }
    }
};

int main(void)
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Obstacle Avoidance");
    SetTargetFPS(60);

    Agent agent(SCREEN_WIDTH / 2.0, SCREEN_HEIGHT / 2.0);
    std::vector<Vector2> obstacles;

    double whiskerLength = 100.0;
    double avoidanceAngle = PI / 4.0;
    double leftWhiskerAngle = PI / 3.0; // Angle for the left whisker
    double rightWhiskerAngle = -PI / 3.0; // Angle for the right whisker

    Vector2 target = { SCREEN_WIDTH / 2.0, SCREEN_HEIGHT / 2.0 };
    double seekSpeed = 2.0;

    while (!WindowShouldClose())
    {
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            Vector2 mousePosition = GetMousePosition();
            obstacles.push_back(mousePosition);
        }

        agent.ObstacleAvoidance(obstacles, whiskerLength, avoidanceAngle);
        agent.Seek(target, seekSpeed);

        // Calculate left and right whisker positions
        double leftWhiskerX = agent.x + whiskerLength * std::cos(avoidanceAngle + leftWhiskerAngle);
        double leftWhiskerY = agent.y + whiskerLength * std::sin(avoidanceAngle + leftWhiskerAngle);
        double rightWhiskerX = agent.x + whiskerLength * std::cos(avoidanceAngle + rightWhiskerAngle);
        double rightWhiskerY = agent.y + whiskerLength * std::sin(avoidanceAngle + rightWhiskerAngle);

        BeginDrawing();
        ClearBackground(RAYWHITE);

        for (const auto& obstacle : obstacles) {
            DrawCircle(obstacle.x, obstacle.y, 15,PINK);
        }

        // Draw left and right whiskers
        DrawLine(agent.x, agent.y, leftWhiskerX, leftWhiskerY, BLACK);
        DrawLine(agent.x, agent.y, rightWhiskerX, rightWhiskerY, GREEN);

        DrawCircle(agent.x, agent.y, 30,PURPLE);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
