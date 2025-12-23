#include "raylib.h"
#include <vector>
#include <cmath>
#include <string>
#include <algorithm>
#include <ctime>
//click f5
// ================= WINDOW =================
const int SCREEN_WIDTH  = 540;
const int SCREEN_HEIGHT = 760;

// ================= PHYSICS =================
const float GRAVITY = 1400.0f;
const float FLOOR_DAMPING = 0.25f;
const float WALL_DAMPING  = 0.45f;
const float SETTLE_VEL    = 20.0f;

// ================= GAME =================
const float WALL_THICKNESS = 24.0f;
const int   NUM_FRUIT_LEVELS = 11;
const float MERGE_COOLDOWN = 0.4f;

const int MIN_SPAWN_LEVEL = 0;
const int MAX_SPAWN_LEVEL = 4;

const float GAME_OVER_LINE = 90.0f;

// Fruit sizes
const float FRUIT_RADIUS[NUM_FRUIT_LEVELS] = {
    20, 26, 32, 38, 44,
    52, 60, 68, 78, 88, 100
};

// ================= STRUCT =================
struct Fruit {
    Vector2 pos;
    Vector2 vel;
    int level;
    float radius;
    float mergeCooldown;
    bool settled;
    bool remove;
};

// ================= GLOBALS =================
std::vector<Fruit> fruits;
Texture2D textures[NUM_FRUIT_LEVELS];

bool gameOver = false;
bool hasActiveFruit = false;
long long score = 0;

int nextFruitLevel;

// ================= HELPERS =================
int GetNextFruitLevel() {
    return GetRandomValue(MIN_SPAWN_LEVEL, MAX_SPAWN_LEVEL);
}

void SpawnFruit(float x, int level) {
    Fruit f{};
    f.pos = { x, 60 };
    f.vel = { 0, 0 };
    f.level = level;
    f.radius = FRUIT_RADIUS[level];
    f.mergeCooldown = MERGE_COOLDOWN;
    f.settled = false;
    f.remove = false;
    fruits.push_back(f);
    hasActiveFruit = true;
}

// ================= PHYSICS =================
void UpdatePhysics(float dt) {
    hasActiveFruit = false;

    for (auto &f : fruits) {
        f.vel.y += GRAVITY * dt;
        f.pos.x += f.vel.x * dt;
        f.pos.y += f.vel.y * dt;

        if (f.mergeCooldown > 0)
            f.mergeCooldown -= dt;

        // Floor
        if (f.pos.y + f.radius >= SCREEN_HEIGHT - WALL_THICKNESS) {
            f.pos.y = SCREEN_HEIGHT - WALL_THICKNESS - f.radius;
            f.vel.y *= -FLOOR_DAMPING;
            if (fabs(f.vel.y) < SETTLE_VEL) {
                f.vel.y = 0;
                f.settled = true;
            }
        }

        // Walls
        if (f.pos.x - f.radius <= WALL_THICKNESS) {
            f.pos.x = WALL_THICKNESS + f.radius;
            f.vel.x *= -WALL_DAMPING;
        }
        if (f.pos.x + f.radius >= SCREEN_WIDTH - WALL_THICKNESS) {
            f.pos.x = SCREEN_WIDTH - WALL_THICKNESS - f.radius;
            f.vel.x *= -WALL_DAMPING;
        }

        if (!f.settled)
            hasActiveFruit = true;

        if (f.settled && f.pos.y - f.radius < GAME_OVER_LINE)
            gameOver = true;
    }
}

// ================= COLLISION + MERGE =================
void ResolveCollisions() {
    for (size_t i = 0; i < fruits.size(); i++) {
        for (size_t j = i + 1; j < fruits.size(); j++) {

            Fruit &a = fruits[i];
            Fruit &b = fruits[j];
            if (a.remove || b.remove) continue;

            Vector2 d = { b.pos.x - a.pos.x, b.pos.y - a.pos.y };
            float dist = sqrtf(d.x*d.x + d.y*d.y);
            float minDist = a.radius + b.radius;

            if (dist > 0 && dist < minDist) {

                // ✅ CORRECT MERGE
                if (a.level == b.level &&
                    a.mergeCooldown <= 0 &&
                    b.mergeCooldown <= 0 &&
                    a.level < NUM_FRUIT_LEVELS - 1) {

                    // Upgrade ONE fruit
                    a.level++;
                    a.radius = FRUIT_RADIUS[a.level];
                    a.mergeCooldown = MERGE_COOLDOWN;
                    a.settled = false;

                    // Remove the other
                    b.remove = true;

                    // Score once
                    score += (long long)(a.level) * 10;

                    return; // ONE merge per frame
                }

                // Push apart
                Vector2 n = { d.x / dist, d.y / dist };
                float overlap = minDist - dist;

                a.pos.x -= n.x * overlap * 0.5f;
                a.pos.y -= n.y * overlap * 0.5f;
                b.pos.x += n.x * overlap * 0.5f;
                b.pos.y += n.y * overlap * 0.5f;
            }
        }
    }

    fruits.erase(
        std::remove_if(fruits.begin(), fruits.end(),
            [](const Fruit& f){ return f.remove; }),
        fruits.end()
    );
}

// ================= DRAW =================
void DrawFruit(const Fruit& f) {
    Texture2D t = textures[f.level];
    float scale = (f.radius * 2) / t.width;

    DrawTextureEx(
        t,
        { f.pos.x - t.width*scale*0.5f,
          f.pos.y - t.height*scale*0.5f },
        0, scale, WHITE
    );
}

// ================= MAIN =================
int main() {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Suika Game - Raylib");
    SetTargetFPS(60);
    SetRandomSeed((unsigned)time(nullptr));

    for (int i = 0; i < NUM_FRUIT_LEVELS; i++) {
        textures[i] = LoadTexture(("assets/circle" + std::to_string(i) + ".png").c_str());
    }

    nextFruitLevel = GetNextFruitLevel();

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();

        if (!gameOver) {

            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !hasActiveFruit) {
                float x = GetMouseX();
                float minX = WALL_THICKNESS + FRUIT_RADIUS[nextFruitLevel];
                float maxX = SCREEN_WIDTH - WALL_THICKNESS - FRUIT_RADIUS[nextFruitLevel];
                if (x < minX) x = minX;
                if (x > maxX) x = maxX;

                SpawnFruit(x, nextFruitLevel);
                nextFruitLevel = GetNextFruitLevel();
            }

            UpdatePhysics(dt);
            ResolveCollisions();
        }

        BeginDrawing();
        ClearBackground({45,52,71,255});

        DrawRectangle(0,0,WALL_THICKNESS,SCREEN_HEIGHT,GRAY);
        DrawRectangle(SCREEN_WIDTH-WALL_THICKNESS,0,WALL_THICKNESS,SCREEN_HEIGHT,GRAY);
        DrawRectangle(0,SCREEN_HEIGHT-WALL_THICKNESS,SCREEN_WIDTH,WALL_THICKNESS,GRAY);

        for (auto &f : fruits)
            DrawFruit(f);

        DrawText(TextFormat("Score: %lld", score), 20, 12, 22, WHITE);
        DrawText("NEXT", SCREEN_WIDTH - 90, 12, 20, WHITE);

        float previewScale = (FRUIT_RADIUS[nextFruitLevel] * 1.2f) / textures[nextFruitLevel].width;
        DrawTextureEx(textures[nextFruitLevel],
                      { SCREEN_WIDTH - 70, 42 },
                      0, previewScale, WHITE);

        if (gameOver) {
            DrawRectangle(0,0,SCREEN_WIDTH,SCREEN_HEIGHT,{0,0,0,180});
            DrawText("GAME OVER", SCREEN_WIDTH/2 - 120, SCREEN_HEIGHT/2 - 40, 36, RED);
            DrawText("Press R to Restart", SCREEN_WIDTH/2 - 130, SCREEN_HEIGHT/2 + 10, 22, WHITE);

            if (IsKeyPressed(KEY_R)) {
                fruits.clear();
                score = 0;
                gameOver = false;
                hasActiveFruit = false;
                nextFruitLevel = GetNextFruitLevel();
            }
        }

        EndDrawing();
    }

    for (int i = 0; i < NUM_FRUIT_LEVELS; i++)
        UnloadTexture(textures[i]);

    CloseWindow();
    return 0;
}
