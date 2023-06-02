#include "FleeBehaviour.h"
#include "Agent.h"
#include "raymath.h"


FleeBehaviour::FleeBehaviour(Agent* target) :
    target(target)
{

}

FleeBehaviour::~FleeBehaviour()
{

}

void FleeBehaviour::Update(Agent* agent, float deltaTime)
{
    float distance = Vector2Distance(target->GetPosition(), agent->GetPosition());

    // only flee when the target gets close
    if (distance < 150.0f)
    {
        Vector2 direction = Vector2Subtract(agent->GetPosition(), target->GetPosition());
        agent->acceleration = Vector2Scale(Vector2Normalize(direction), (agent->maxSpeed * deltaTime));
    }
    else
    {
        // slow down the agent
        agent->acceleration.x -= (agent->rigidbody)->velocity.x * deltaTime;
        agent->acceleration.y -= (agent->rigidbody)->velocity.y * deltaTime;
    }
}


