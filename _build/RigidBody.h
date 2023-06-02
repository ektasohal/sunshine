#include "raylib.h"

class Rigidbody
{
public:
    Rigidbody();
    Rigidbody(Vector2 position, Vector2 velocity);
    ~Rigidbody();
public:
    Vector2 position = { 0.0f, 0.0f };
    Vector2 velocity = { 0.0f, 0.0f };
};

