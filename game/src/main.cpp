 #include "rlImGui.h"
#include <math.h>   

int main()
{
    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "Circle Example");
    InitWindow(screenWidth, 600, "Draw Triangle");
    SetTargetFPS(60);

    Vector2 circlePosition = { 700, 100 };
    Vector2 mousePosition;

    float circleRadius = 20;
    Color circleColor = ORANGE;


    const int virtualScreenWidth = 160;
    const int virtualScreenHeight = 90;

    const float virtualRatio = (float)screenWidth / (float)virtualScreenWidth;

    InitWindow(screenWidth, screenHeight, "raylib [core] example - smooth pixel-perfect camera");

    Camera2D worldSpaceCamera = { 0 };  // Game world camera
    worldSpaceCamera.zoom = 1.0f;

    Camera2D screenSpaceCamera = { 0 }; // Smoothing camera
    screenSpaceCamera.zoom = 1.0f;

    RenderTexture2D target = LoadRenderTexture(virtualScreenWidth, virtualScreenHeight); // This is where we'll draw all our objects.

    Rectangle rec01 = { 70.0f, 35.0f, 20.0f, 20.0f };
    Rectangle rec02 = { 90.0f, 55.0f, 30.0f, 10.0f };
    Rectangle rec03 = { 80.0f, 65.0f, 15.0f, 25.0f };

    // The target's height is flipped (in the source Rectangle), due to OpenGL reasons
    Rectangle sourceRec = { 0.0f, 0.0f, (float)target.texture.width, -(float)target.texture.height };
    Rectangle destRec = { -virtualRatio, -virtualRatio, screenWidth + (virtualRatio * 2), screenHeight + (virtualRatio * 2) };

    Vector2 origin = { 0.0f, 0.0f };

    float rotation = 0.0f;

    float cameraX = 0.0f;
    float cameraY = 0.0f;

    //**********************************************************
    //rendering of two circle and changing colours when overlapping
    //************************************************************


    while (!WindowShouldClose())
    {
        mousePosition = GetMousePosition();

        if (CheckCollisionCircles(circlePosition, circleRadius, mousePosition, circleRadius))
        {
            circleColor = GREEN;
        }
        else
        {
            circleColor = ORANGE;
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawCircleV(circlePosition, circleRadius, circleColor);
        DrawCircleV(mousePosition, circleRadius, circleColor);

        //*******************************************************
        //making a face using yellow circle and 2 black circles for eyes
        //******************************************************
        DrawCircle(400, 50, 30, YELLOW);
        DrawCircle(390, 50, 7, BLACK);
        DrawCircle(410, 50, 7, BLACK);
       
        //*****************************************
       // drawn a rectangle to make neck
        //***************************************
        DrawRectangle(400, 70, 10, 20, BLUE);


        //draw a triangle 
        DrawTriangle( {400, 300 },{600, 240}, {200, 240}, VIOLET);

       // to make body of character
         DrawEllipse(400,170 , 50, 80, RED);
       
         //*************************************
         //to make arms of character
       DrawLine(370,105 ,280 , 180, BLUE);
       DrawLine(430, 105, 520, 180, BLUE);

     
       // ***************
       //to draw moving ractangles
       //************************
       rotation += 60.0f * GetFrameTime();   // Rotate the rectangles, 60 degrees per second

       // Make the camera move to demonstrate the effect
       cameraX = (sinf(GetTime()) * 50.0f) - 10.0f;
       cameraY = cosf(GetTime()) * 30.0f;

       // Set the camera's target to the values computed above
       screenSpaceCamera.target = { cameraX, cameraY };

       // Round worldSpace coordinates, keep decimals into screenSpace coordinates
       worldSpaceCamera.target.x = (int)screenSpaceCamera.target.x;
       screenSpaceCamera.target.x -= worldSpaceCamera.target.x;
       screenSpaceCamera.target.x *= virtualRatio;

       worldSpaceCamera.target.y = (int)screenSpaceCamera.target.y;
       screenSpaceCamera.target.y -= worldSpaceCamera.target.y;
       screenSpaceCamera.target.y *= virtualRatio;


       // Draw
       
      // BeginTextureMode(target);
       ClearBackground(RAYWHITE);

       BeginMode2D(worldSpaceCamera);
       DrawRectanglePro(rec01, origin, rotation, BLACK);
       DrawRectanglePro(rec02, origin, -rotation, RED);
       DrawRectanglePro(rec03, origin, rotation + 45.0f, BLUE);
       EndMode2D();
       EndTextureMode();

       
       //ClearBackground(RED);

       BeginMode2D(screenSpaceCamera);
       DrawTexturePro(target.texture, sourceRec, destRec, origin, 0.0f, WHITE);
       EndMode2D();

       DrawFPS(GetScreenWidth() - 95, 10);

        EndDrawing();
    }

    CloseWindow();

    return 0;
}

