#include "Mouse.h"
#include "Agent.h"
#include "raymath.h"

FollowMouseBehaviour::FollowMouseBehaviour()
{

}

FollowMouseBehaviour::~FollowMouseBehaviour()
{

}

void FollowMouseBehaviour::Update(Agent* agent, float deltaTime)
{
    Vector2 mousePosition = GetMousePosition();

    Vector2 direction = Vector2Subtract(mousePosition, agent->GetPosition());
    agent->acceleration = Vector2Scale(Vector2Normalize(direction), (agent->maxSpeed * deltaTime));
}