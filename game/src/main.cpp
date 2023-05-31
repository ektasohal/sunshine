#include "raylib.h"
#include <math.h>
#include <stdlib.h>


int main(void)
{
   
    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "raylib [shapes] example - raylib logo using shapes");
    InitWindow(screenWidth, screenHeight, "raylib [shapes] example - following eyes");

    InitWindow(screenWidth, screenHeight, "raylib [shapes] example - collision area");

    SetTargetFPS(60);   
    //***************for moving eyes************
    Vector2 scleraLeftPosition = { GetScreenWidth() / 2.0f - 100.0f, GetScreenHeight() / 2.0f };
    Vector2 scleraRightPosition = { GetScreenWidth() / 2.0f + 100.0f, GetScreenHeight() / 2.0f };
    float scleraRadius = 80;

    Vector2 irisLeftPosition = { GetScreenWidth() / 2.0f - 100.0f, GetScreenHeight() / 2.0f };
    Vector2 irisRightPosition = { GetScreenWidth() / 2.0f + 100.0f, GetScreenHeight() / 2.0f };
    float irisRadius = 24;

    float angle = 0.0f;
    float dx = 0.0f, dy = 0.0f, dxx = 0.0f, dyy = 0.0f;


    //**************for colliding rectangles**********
    Rectangle boxA = { 10, GetScreenHeight() / 2.0f - 50, 200, 100 };
    int boxASpeedX = 4;

    // Box B: Mouse moved box
    Rectangle boxB = { GetScreenWidth() / 2.0f - 30, GetScreenHeight() / 2.0f - 30, 60, 60 };

    Rectangle boxCollision = { 0 }; // Collision rectangle

    int screenUpperLimit = 40;      // Top menu limits

    bool pause = false;             // Movement pause
    bool collision = false;

    while (!WindowShouldClose())    // Detect window close button or ESC key
    {

        //**************for moving eyes********
        irisLeftPosition = GetMousePosition();
        irisRightPosition = GetMousePosition();

        // Check not inside the left eye sclera
        if (!CheckCollisionPointCircle(irisLeftPosition, scleraLeftPosition, scleraRadius - 20))
        {
            dx = irisLeftPosition.x - scleraLeftPosition.x;
            dy = irisLeftPosition.y - scleraLeftPosition.y;

            angle = atan2f(dy, dx);

            dxx = (scleraRadius - irisRadius) * cosf(angle);
            dyy = (scleraRadius - irisRadius) * sinf(angle);

            irisLeftPosition.x = scleraLeftPosition.x + dxx;
            irisLeftPosition.y = scleraLeftPosition.y + dyy;
        }

        // Check not inside the right eye sclera
        if (!CheckCollisionPointCircle(irisRightPosition, scleraRightPosition, scleraRadius - 20))
        {
            dx = irisRightPosition.x - scleraRightPosition.x;
            dy = irisRightPosition.y - scleraRightPosition.y;

            angle = atan2f(dy, dx);

            dxx = (scleraRadius - irisRadius) * cosf(angle);
            dyy = (scleraRadius - irisRadius) * sinf(angle);

            irisRightPosition.x = scleraRightPosition.x + dxx;
            irisRightPosition.y = scleraRightPosition.y + dyy;
        }


        //**************for colliding rectangles**********
        if (!pause) boxA.x += boxASpeedX;

        // Bounce box on x screen limits
        if (((boxA.x + boxA.width) >= GetScreenWidth()) || (boxA.x <= 0)) boxASpeedX *= -1;

        // Update player-controlled-box (box02)
        boxB.x = GetMouseX() - boxB.width / 2;
        boxB.y = GetMouseY() - boxB.height / 2;

        // Make sure Box B does not go out of move area limits
        if ((boxB.x + boxB.width) >= GetScreenWidth()) boxB.x = GetScreenWidth() - boxB.width;
        else if (boxB.x <= 0) boxB.x = 0;

        if ((boxB.y + boxB.height) >= GetScreenHeight()) boxB.y = GetScreenHeight() - boxB.height;
        else if (boxB.y <= screenUpperLimit) boxB.y = (float)screenUpperLimit;

        // Check boxes collision
        collision = CheckCollisionRecs(boxA, boxB);

        // Get collision rectangle (only on collision)
        if (collision) boxCollision = GetCollisionRec(boxA, boxB);

        // Pause Box A movement
        if (IsKeyPressed(KEY_SPACE)) pause = !pause;



        BeginDrawing();

        ClearBackground(RAYWHITE);
        //***************for raylib reactangle***************
        DrawRectangle(screenWidth / 2 - 128, screenHeight / 2 - 128, 256, 256, BLACK);
        DrawRectangle(screenWidth / 2 - 112, screenHeight / 2 - 112, 224, 224, RAYWHITE);
        DrawText("raylib", screenWidth / 2 - 44, screenHeight / 2 + 48, 50, BLACK);

        DrawText("this is NOT a texture!", 350, 370, 10, GRAY);

        //*****************for moving eyes***************
        ClearBackground(RAYWHITE);

        DrawCircleV(scleraLeftPosition, scleraRadius, LIGHTGRAY);
        DrawCircleV(irisLeftPosition, irisRadius, BROWN);
        DrawCircleV(irisLeftPosition, 10, BLACK);

        DrawCircleV(scleraRightPosition, scleraRadius, LIGHTGRAY);
        DrawCircleV(irisRightPosition, irisRadius, DARKGREEN);
        DrawCircleV(irisRightPosition, 10, BLACK);



        //**************for colliding rectangles**********
        DrawRectangle(0, 0, screenWidth, screenUpperLimit, collision ? RED : BLACK);

        DrawRectangleRec(boxA, GOLD);
        DrawRectangleRec(boxB, BLUE);

        if (collision)
        {
            // Draw collision area
            DrawRectangleRec(boxCollision, LIME);

            // Draw collision message
            DrawText("COLLISION!", GetScreenWidth() / 2 - MeasureText("COLLISION!", 20) / 2, screenUpperLimit / 2 - 10, 20, BLACK);

            // Draw collision area
            DrawText(TextFormat("Collision Area: %i", (int)boxCollision.width * (int)boxCollision.height), GetScreenWidth() / 2 - 100, screenUpperLimit + 10, 20, BLACK);
        }

        EndDrawing();
       


    }

   
    CloseWindow();        
    return 0;
}