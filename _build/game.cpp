#include "Game.h"
#include "raylib.h"
#include "rlImGui.h"
#include "Math.h"



Game::Game()
{

}

bool Game::Initialise()
{
    SetTargetFPS(60);
    InitWindow(1024, 768, "Kinematics Simulation");



    player = new Agent({ 200, 50 }, 60.0f, ORANGE);

    followMouseBehaviour = new FollowMouseBehaviour();
    player->AddBehaviour(followMouseBehaviour);
    player->maxSpeed = 100.0f;

    redEnemy = new Agent({ 860, 200 }, 60.0f, WHITE);
    seekBehaviour = new SeekBehaviour(player);
    redEnemy->AddBehaviour(seekBehaviour);

    shyEnemy = new Agent({ 100,200 }, 60.0f, PINK);
    fleeBehaviour = new FleeBehaviour(player);
    shyEnemy->AddBehaviour(fleeBehaviour);

    return 0;
}

void Game::RunLoop()
{

    while (!WindowShouldClose())
    {
        ProcessInput();
        UpdateGame(GetFrameTime());
        GenerateOutput();
    }
}


void Game::Shutdown()
{
    CloseWindow();
}

void Game::ProcessInput()
{
    //_mousePosition = GetMousePosition();
}

void Game::UpdateGame(float deltaTime)
{
    player->Update(deltaTime);
    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) == true)
        redEnemy->Update(deltaTime);
    shyEnemy->Update(deltaTime);
}

void Game::GenerateOutput()
{
    BeginDrawing();

    // clears the screen
    ClearBackground(RED);

    player->Draw();
    redEnemy->Draw();
    shyEnemy->Draw();

    EndDrawing();
}