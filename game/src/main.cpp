#include "rlImGui.h"
#include "Physics.h"
#include "Collision.h"

#include "Grid.h"
#include "World.h"
#include "Nodes.h"

using namespace std;

bool ResolveCollisions(Vector2& position, float radius, const Obstacles& obstacles)
{
    for (const Circle& obstacle : obstacles)
    {
        Vector2 mtv;
        if (CheckCollisionCircles(obstacle, { position, radius }, mtv))
        {
            position = position + mtv;
            return true;
        }
    }
    return false;
}

int main(void)
{
    World world;
    world.obstacles = LoadObstacles();
    world.points = LoadPoints();

    Player player;
    player.radius = 60.0f;
    player.dir = { 1.0f, 0.0f };
    player.angularSpeed = 100.0f;
    player.name = "Player";

    Enemy cce;
    cce.pos = { 1000.0f, 250.0f };
    cce.dir = { -1.0f, 0.0f };
    cce.angularSpeed = DEG2RAD * 100.0f;
    cce.point = 0;
    cce.speed = 300.0f;
    cce.radius = 50.0f;
    cce.detectionRadius = 300.0f;
    cce.probeLength = 100.0f;
    cce.combatRadius = 100.0f;
    cce.name = "Close-combat enemy";

    Enemy rce;
    rce.pos = { 10.0f, 10.0f };
    rce.dir = { 1.0f, 0.0f };
    rce.angularSpeed = DEG2RAD * 100.0f;
    rce.point = 0;
    rce.speed = 300.0f;
    rce.radius = 50.0f;
    rce.detectionRadius = 300.0f;
    rce.probeLength = 100.0f;
    rce.combatRadius = 400.0f;
    rce.name = "Ranged-combat enemy";

    DetectedCondition cceIsPlayerDetected(cce);
    VisibleCondition cceIsPlayerVisible(cce);
    CloseCombatCondition cceIsPlayerCombat(cce);

    PatrolAction ccePatrol(cce);
    FindVisibilityAction cceFindVisibility(cce, &ccePatrol);
    ArriveAction cceArrive(cce);
    CloseAttackAction cceAttack(cce);

    Node* cceRoot = &cceIsPlayerDetected;
    cceIsPlayerDetected.no = &ccePatrol;
    cceIsPlayerDetected.yes = &cceIsPlayerVisible;
    cceIsPlayerVisible.no = &cceFindVisibility;
    cceIsPlayerVisible.yes = &cceIsPlayerCombat;
    cceIsPlayerCombat.no = &cceArrive;
    cceIsPlayerCombat.yes = &cceAttack;

    const Color background = RAYWHITE;
    const Color playerColor = { 0, 228, 48, 128 };          // GREEN

    const Color cceColor = { 0, 121, 241, 128 };            // BLUE
    const Color cceOverlapColor = { 0, 82, 172, 128 };      // DARKBLUE
    const Color cceVisibleColor = { 102, 191, 255, 128 };   // SKYBLUE

    const Color rceColor = { 135, 60, 190, 128 };           // VIOLET
    const Color rceOverlapColor = { 112, 31, 126, 128 };    // DARKPURPLE
    const Color rceVisibleColor = { 200, 122, 255, 128 };   // PURPLE

    bool useDebug = true;
    bool useGUI = false;
    bool showPoints = false;
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Sunshine");
    rlImGuiSetup(true);
    SetTargetFPS(60);
    while (!WindowShouldClose())
    {
        const float dt = GetFrameTime();

        // Update player information
        if (IsKeyDown(KEY_E))
            player.dir = Rotate(player.dir, player.angularSpeed * dt * DEG2RAD);
        if (IsKeyDown(KEY_Q))
            player.dir = Rotate(player.dir, -player.angularSpeed * dt * DEG2RAD);

        player.pos = GetMousePosition();
        const Vector2 playerEnd = player.pos + player.dir * 500.0f;

        Traverse(cceRoot, player, world);
        cce.acc = cce.acc + Avoid(cce, cce.probeLength, dt, world.obstacles);
        Integrate(cce, dt);

        rce.acc = Patrol(world.points, rce, rce.point, rce.speed, 200.0f, 100.0f);
        rce.acc = rce.acc + Avoid(rce, rce.probeLength, dt, world.obstacles);
        Integrate(rce, dt);

        bool playerCollision = ResolveCollisions(player.pos, player.radius, world.obstacles);
        bool cceCollision = ResolveCollisions(cce.pos, cce.radius, world.obstacles);
        bool rceCollision = ResolveCollisions(rce.pos, rce.radius, world.obstacles);

        vector<Vector2> intersections;
        for (const Circle& obstacle : world.obstacles)
        {
            Vector2 poi;
            if (CheckCollisionLineCircle(player.pos, playerEnd, obstacle, poi))
                intersections.push_back(poi);
        }
        bool playerIntersection = !intersections.empty();

        BeginDrawing();
        ClearBackground(background);

        // Render debug
        if (useDebug)
        {
            Rectangle cceOverlapRec = From({ cce.pos, cce.detectionRadius });
            vector<size_t> cceVisibleTiles =
                VisibleTiles(player.Collider(), cce.detectionRadius, world.obstacles, OverlapTiles(cceOverlapRec));

            Rectangle rceOverlapRec = From({ rce.pos, rce.detectionRadius });
            vector<size_t> rceVisibleTiles =
                VisibleTiles(player.Collider(), rce.detectionRadius, world.obstacles, OverlapTiles(rceOverlapRec));
                
            DrawRectangleRec(rceOverlapRec, rceOverlapColor);
            for (size_t i : cceVisibleTiles)
                DrawRectangleV(GridToScreen(i), { TILE_WIDTH, TILE_HEIGHT }, cceVisibleColor);

            DrawRectangleRec(cceOverlapRec, cceOverlapColor);
            for (size_t i : rceVisibleTiles)
                DrawRectangleV(GridToScreen(i), { TILE_WIDTH, TILE_HEIGHT }, rceVisibleColor);
        }

        // Render entities
        DrawCircleV(cce.pos, cce.radius, cceCollision ? RED : cceColor);
        DrawCircleV(rce.pos, cce.radius, rceCollision ? RED : rceColor);
        DrawCircleV(player.pos, player.radius, playerCollision ? RED : playerColor);
        DrawLineEx(cce.pos, cce.pos + cce.dir * cce.detectionRadius, 10.0f, cceColor);
        DrawLineEx(rce.pos, rce.pos + rce.dir * rce.detectionRadius, 10.0f, rceColor);
        DrawLineEx(player.pos, playerEnd, 10.0f, playerIntersection ? RED : playerColor);

        // Avoidance lines
        DrawLineEx(cce.pos, cce.pos + Rotate(Normalize(cce.vel), -30.0f * DEG2RAD) * cce.probeLength, 5.0f, cceColor);
        DrawLineEx(cce.pos, cce.pos + Rotate(Normalize(cce.vel), -15.0f * DEG2RAD) * cce.probeLength, 5.0f, cceColor);
        DrawLineEx(cce.pos, cce.pos + Rotate(Normalize(cce.vel),  15.0f * DEG2RAD) * cce.probeLength, 5.0f, cceColor);
        DrawLineEx(cce.pos, cce.pos + Rotate(Normalize(cce.vel),  30.0f * DEG2RAD) * cce.probeLength, 5.0f, cceColor);
        DrawLineEx(rce.pos, rce.pos + Rotate(Normalize(rce.vel), -30.0f * DEG2RAD) * rce.probeLength, 5.0f, rceColor);
        DrawLineEx(rce.pos, rce.pos + Rotate(Normalize(rce.vel), -15.0f * DEG2RAD) * rce.probeLength, 5.0f, rceColor);
        DrawLineEx(rce.pos, rce.pos + Rotate(Normalize(rce.vel),  15.0f * DEG2RAD) * rce.probeLength, 5.0f, rceColor);
        DrawLineEx(rce.pos, rce.pos + Rotate(Normalize(rce.vel),  30.0f * DEG2RAD) * rce.probeLength, 5.0f, rceColor);

        // Render obstacle intersections
        Vector2 obstaclesPoi;
        if (NearestIntersection(player.pos, playerEnd, world.obstacles, obstaclesPoi))
            DrawCircleV(obstaclesPoi, 10.0f, playerIntersection ? RED : playerColor);

        // Render obstacles
        for (const Circle& obstacle : world.obstacles)
            DrawCircleV(obstacle.position, obstacle.radius, GRAY);

        // Render points
        for (size_t i = 0; i < world.points.size(); i++)
        {
            const Vector2& p0 = world.points[i];
            const Vector2& p1 = world.points[(i + 1) % world.points.size()];
            DrawLineV(p0, p1, GRAY);
            DrawCircle(p0.x, p0.y, 5.0f, LIGHTGRAY);
        }
        
        // Render GUI
        if (IsKeyPressed(KEY_GRAVE)) useGUI = !useGUI;
        if (useGUI)
        {
            rlImGuiBegin();
            ImGui::Checkbox("Use heatmap", &useDebug);
            ImGui::SliderFloat2("CCE Position", (float*)&cce.pos, 0.0f, 1200.0f);
            ImGui::SliderFloat2("RCE Position", (float*)&rce.pos, 0.0f, 1200.0f);
            ImGui::SliderFloat("CCE Detection Radius", &cce.detectionRadius, 0.0f, 1500.0f);
            ImGui::SliderFloat("RCE Detection Radius", &rce.detectionRadius, 0.0f, 1500.0f);
            ImGui::SliderFloat("CCE Probe Length", &cce.probeLength, 0.0f, 250.0f);
            ImGui::SliderFloat("RCE Probe Length", &rce.probeLength, 0.0f, 250.0f);
            
            ImGui::Separator();
            if (ImGui::Button("Save Obstacles"))
                SaveObstacles(world.obstacles);
            if (ImGui::Button("Add Obstacle"))
                world.obstacles.push_back({ {}, 10.0f });
            if (ImGui::Button("Remove Obstacle"))
                world.obstacles.pop_back();
            for (size_t i = 0; i < world.obstacles.size(); i++)
                ImGui::SliderFloat3(string("Obstacle " + to_string(i + 1)).c_str(),
                    (float*)&world.obstacles[i], 0.0f, 1200.0f);

            ImGui::Separator();
            if (ImGui::Button("Save Points"))
                SavePoints(world.points);
            if (ImGui::Button("Add Point"))
                world.points.push_back({ {}, 10.0f });
            if (ImGui::Button("Remove Point"))
                world.points.pop_back();
            for (size_t i = 0; i < world.points.size(); i++)
                ImGui::SliderFloat2(string("Point " + to_string(i + 1)).c_str(),
                    (float*)&world.points[i], 0.0f, 1200.0f);

            rlImGuiEnd();
        }

        DrawFPS(10, 10);
        EndDrawing();
    }

    rlImGuiShutdown();
    CloseWindow();

    return 0;
}

// Sooooooo much less code thant the decision tree...
// Perhaps give this to students and ask them to implement this as a decision tree for future assignment 3?
/*
void Melee(Rigidbody& enemy, EnemyData& enemyData, const Rigidbody& player, const PlayerData& playerData,
    const Points& points, const Obstacles& obstacles)
{
    const Circle playerCircle{ player.pos, playerData.radius };

    // Player detected?
    if (CheckCollisionCircles({ enemy.pos, enemyData.sightDistance }, playerCircle))
    {
        // Player visible? (No FoV check or rotate till in FoV because this is complicated enough already)...
        if (IsCircleVisible(enemy.pos, enemy.pos + Normalize(player.pos - enemy.pos) * enemyData.sightDistance,
            playerCircle, obstacles))
        {
            // Within combat distance?
            if (CheckCollisionCircles({ enemy.pos, enemyData.combatDistance }, playerCircle))
            {
                // Close attack (must still call arrive)
                enemy.acc = Arrive(player.pos, enemy, enemyData.speed, 100.0f, 5.0f);
                printf("Melee attacking player\n");
            }
            else
            {
                // Seek player
                enemy.acc = Arrive(player.pos, enemy, enemyData.speed, 100.0f, 5.0f);
                printf("Melee seeking player\n");
            }
        }
        else
        {
            // Seek nearest visible tile
            Rectangle area = From({ enemy.pos, enemyData.radius });
            vector<size_t> overlap = OverlapTiles(area);
            vector<size_t> visible = VisibleTiles(playerCircle, enemyData.sightDistance, obstacles, overlap);
            Points points(visible.size());
            for (size_t i = 0; i < points.size(); i++)
                points[i] = GridToScreen(visible[i]);
            if (!points.empty())
                enemy.acc = Seek(NearestPoint(enemy.pos, points), enemy, enemyData.speed);
            printf("Melee seeking visibility\n");
        }
    }
    else
    {
        enemy.acc = Patrol(points, enemy, enemyData.point, enemyData.speed, 200.0f, 100.0f);
        printf("Melee patrolling\n");
    }
}

void Ranged(Rigidbody& enemy, EnemyData& enemyData, const Rigidbody& player, const PlayerData& playerData,
    const Points& points, const Obstacles& obstacles)
{

}
*/