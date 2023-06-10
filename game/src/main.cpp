#include "rlImGui.h"
#include "raylib.h"
#include <vector>
#include <cmath>
#include <iostream>
using namespace std;

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

class Fish {
public:
    Vector2 position;
    Vector2 velocity;
    float maxSpeed;
    float maxAcceleration;

    Fish(float xPos, float yPos, float maxSpeed, float maxAcceleration)
        : position({ xPos, yPos }), velocity({ 0, 0 }), maxSpeed(maxSpeed), maxAcceleration(maxAcceleration)
    {
    }

    void Update(Vector2 targetPosition, Vector2 predatorPosition, Vector2 obstaclePosition) {
        // Calculate steering forces for different behaviors
        Vector2 seekForce = Seek(targetPosition);
        Vector2 fleeForce = Flee(predatorPosition);
        Vector2 arrivalForce = Arrival(targetPosition);
        Vector2 obstacleAvoidanceForce = ObstacleAvoidance(obstaclePosition);

        // Combine the forces with appropriate weights
        Vector2 combinedForce = (seekForce * 0.5f) + (fleeForce * 1.0f) + (arrivalForce * 0.75f) + (obstacleAvoidanceForce * 0.75f);

        // Apply the combined force to the fish's acceleration
        velocity += combinedForce;
        velocity = Vector2Normalize(velocity) * maxSpeed;

        // Update the fish's position
        position += velocity;
    }

    void Draw() {
        DrawCircle(static_cast<int>(position.x), static_cast<int>(position.y), 10, GREEN);
    }

private:
    Vector2 Seek(Vector2 target) {
        Vector2 desiredVelocity = Vector2Normalize(target - position) * maxSpeed;
        return desiredVelocity - velocity;
    }

    Vector2 Flee(Vector2 predator) {
        const float fleeDistance = 100.0f;
        if (Vector2Distance(position, predator) < fleeDistance) {
            Vector2 desiredVelocity = Vector2Normalize(position - predator) * maxSpeed;
            return desiredVelocity - velocity;
        }
        return { 0, 0 };
    }

    Vector2 Arrival(Vector2 target) {
        const float slowingRadius = 100.0f;
        float distance = Vector2Distance(position, target);
        if (distance < slowingRadius) {
            float targetSpeed = maxSpeed * (distance / slowingRadius);
            Vector2 desiredVelocity = Vector2Normalize(target - position) * targetSpeed;
            return desiredVelocity - velocity;
        }
        return { 0, 0 };
    }

    Vector2 ObstacleAvoidance(Vector2 obstacle) {
        const float avoidanceDistance = 50.0f;
        if (Vector2Distance(position, obstacle) < avoidanceDistance) {
            Vector2 avoidanceForce = Vector2Normalize(position - obstacle);
            return avoidanceForce;
        }
        return { 0, 0 };
    }
};

class Aquarium {
public:
    std::vector<Fish> fishes;
    Vector2 foodPosition;
    Vector2 predatorPosition;
    Vector2 obstaclePosition;
    int mode;

    Aquarium()
        : foodPosition({ -1, -1 }), predatorPosition({ -1, -1 }), obstaclePosition({ -1, -1 }), mode(0)
    {
    }

    void CreateFish(float xPos, float yPos, float maxSpeed, float maxAcceleration) {
        Fish fish(xPos, yPos, maxSpeed, maxAcceleration);
        fishes.push_back(fish);
    }

    void Reset() {
        fishes.clear();
        foodPosition = { -1, -1 };
        predatorPosition = { -1, -1 };
        obstaclePosition = { -1, -1 };
        CreateFish(GetRandomValue(100, SCREEN_WIDTH - 100), GetRandomValue(100, SCREEN_HEIGHT - 100), GetRandomValue(50, 100) / 100.0f, GetRandomValue(50, 100) / 100.0f);
        CreateFish(GetRandomValue(100, SCREEN_WIDTH - 100), GetRandomValue(100, SCREEN_HEIGHT - 100), GetRandomValue(50, 100) / 100.0f, GetRandomValue(50, 100) / 100.0f);
    }

    void Update() {
        for (Fish& fish : fishes) {
            fish.Update(foodPosition, predatorPosition, obstaclePosition);

            // Keep the fish within the screen bounds
            if (fish.position.x < 0) fish.position.x = SCREEN_WIDTH;
            if (fish.position.x > SCREEN_WIDTH) fish.position.x = 0;
            if (fish.position.y < 0) fish.position.y = SCREEN_HEIGHT;
            if (fish.position.y > SCREEN_HEIGHT) fish.position.y = 0;
        }
    }

    void Draw() {
        for (const Fish& fish : fishes) {
            fish.Draw();
        }
    }
};

int main() {
    const int maxFish = 2;
    Aquarium aquarium;
    aquarium.Reset();

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Aquarium");
    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        if (IsKeyPressed(KEY_SPACE)) {
            aquarium.Reset();
        }

        if (IsKeyPressed(KEY_0)) {
            aquarium.mode = 0;
        }
        else if (IsKeyPressed(KEY_1)) {
            aquarium.mode = 1;
        }
        else if (IsKeyPressed(KEY_2)) {
            aquarium.mode = 2;
        }
        else if (IsKeyPressed(KEY_3)) {
            aquarium.mode = 3;
        }
        else if (IsKeyPressed(KEY_4)) {
            aquarium.mode = 4;
        }

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            switch (aquarium.mode) {
            case 0:
                // Mode 0: Do nothing
                break;
            case 1:
                // Mode 1: Seek behavior
                aquarium.foodPosition = GetMousePosition();
                break;
            case 2:
                // Mode 2: Flee behavior
                aquarium.predatorPosition = GetMousePosition();
                break;
            case 3:
                // Mode 3: Arrival behavior
                aquarium.foodPosition = GetMousePosition();
                break;
            case 4:
                // Mode 4: Obstacle avoidance behavior
                aquarium.obstaclePosition = GetMousePosition();
                break;
            default:
                break;
            }
        }

        BeginDrawing();
        ClearBackground(BLACK);

        aquarium.Update();
        aquarium.Draw();

        EndDrawing();
    }

    CloseWindow();

    return 0;
}