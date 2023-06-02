#include "SeekBehaviour.h"
#include "Agent.h"
#include "raymath.h"

SeekBehaviour::SeekBehaviour(Agent* target) :
    target(target)
{

}

SeekBehaviour::~SeekBehaviour()
{

}

void SeekBehaviour::Update(Agent* agent, float deltaTime)
{
    Vector2 direction = Vector2Subtract(target->GetPosition(), agent->GetPosition());
    agent->acceleration = Vector2Scale(Vector2Normalize(direction), (agent->maxSpeed * deltaTime));
}