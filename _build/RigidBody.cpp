#include "Rigidbody.h"

Rigidbody::Rigidbody()
{
    velocity = { 0.0f, 0.0f };
    position = { 0.0f, 0.0f };
}
Rigidbody::Rigidbody(Vector2 argVelocity, Vector2 argPosition)
{
    position = argPosition;
    velocity = argVelocity;
}
Rigidbody::~Rigidbody()
{

}