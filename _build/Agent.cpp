#include <iostream>
#include "Agent.h"
#include "raymath.h"
#include "IBehaviour.h"

Agent::Agent()
{
    rigidbody = new Rigidbody();
}

Agent::Agent(Vector2 position, float radius, Color colour) :
    radius{ radius },
    colour{ colour }
{
    rigidbody = new Rigidbody(position, { 10.0f, 10.0f });
    rigidbody->position = position;
}

void Agent::SetPosition(Vector2 position)
{
    rigidbody->position = position;
}

Vector2 Agent::GetPosition()
{
    return rigidbody->position;
}

void Agent::Update(float deltaTime)
{
    // for each behaviour in the behavious list
    for (auto behaviour = behaviours.begin(); behaviour != behaviours.end(); behaviour++)
    {
        (*behaviour)->Update(this, deltaTime);
    }

    rigidbody->velocity.x += acceleration.x;
    rigidbody->velocity.y += acceleration.y;

    // check if the magitude of the velocity is bigger than maxSpeed
    if (Vector2Length(rigidbody->velocity) > maxSpeed)
    {
        rigidbody->velocity = Vector2Scale(Vector2Normalize(rigidbody->velocity), maxSpeed);
    }

    rigidbody->position.x += rigidbody->velocity.x * deltaTime;
    rigidbody->position.y += rigidbody->velocity.y * deltaTime;

    acceleration = { 0.0f, 0.0f };
}

void Agent::Draw()
{

    DrawCircleV(rigidbody->position, radius, colour);
}

void Agent::AddBehaviour(IBehaviour* behaviour)
{
    behaviours.push_back(behaviour);
}

