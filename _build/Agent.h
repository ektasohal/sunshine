#pragma once

#include "raylib.h"
#include "Rigidbody.h"
#include <list>

class IBehaviour;

class Agent
{
public:
    float maxSpeed{ 100.0f };
    Vector2 acceleration;
    Rigidbody* rigidbody;

    Agent();
    Agent(Vector2 position, float radius, Color colour);

    virtual ~Agent() {};

    virtual void Update(float deltaTime);

    virtual void Draw();

    void AddBehaviour(IBehaviour* behaviour);

    void SetPosition(Vector2 position);
    Vector2 GetPosition();

private:
    float radius{ 20.0f };
    Color colour{ GREEN };
    std::list<IBehaviour*> behaviours;
};

