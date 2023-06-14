#include "raylib.h"
#include "raymath.h"
#include <vector>
#include <cstdlib>
#include <ctime>

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;
const int FISH_RADIUS = 20;
const float MAX_FISH_ACCELERATION = 400.0f;
const float MAX_FISH_SPEED = 200.0f;

float RemapValue(float value, float inputMin, float inputMax, float outputMin, float outputMax) {
    return outputMin + (value - inputMin) * (outputMax - outputMin) / (inputMax - inputMin);
}

struct Fish
{
    Vector2 position;
    Vector2 velocity;
    Vector2 acceleration;

    Fish(Vector2 startPos, Vector2 startVelocity)
        : position(startPos), velocity(startVelocity), acceleration({ 0, 0 }) {}

    void Update(float deltaTime) {
        // Update velocity based on acceleration
        velocity.x += acceleration.x * deltaTime;
        velocity.y += acceleration.y * deltaTime;

        // Limit velocity to max speed
        float speed = Vector2Length(velocity);
        if (speed > MAX_FISH_SPEED) {
            velocity = Vector2Normalize(velocity);
            velocity = Vector2Scale(velocity, MAX_FISH_SPEED);
        }

        // Update position based on velocity
        position.x += velocity.x * deltaTime;
        position.y += velocity.y * deltaTime;

        // Reset acceleration
        acceleration = { 0, 0 };
    }

    void ApplyForce(const Vector2& force) {
        acceleration.x += force.x;
        acceleration.y += force.y;
    }

    void Draw() const {
        DrawCircle(static_cast<int>(position.x), static_cast<int>(position.y), FISH_RADIUS, BLUE);
    }
};

class Aquarium
{
public:
    std::vector<Fish> fish;
    int mode;
    Vector2 foodPos;
    Vector2 predatorPos;
    Vector2 obstaclePos;

    Aquarium(int numFish)
        : mode(0), foodPos({ -100.0f, -100.0f }), predatorPos({ -100.0f, -100.0f }), obstaclePos({ -100.0f, -100.0f }) {
        srand(static_cast<unsigned int>(time(nullptr)));

        for (int i = 0; i < numFish; ++i) {
            int x = GetRandomValue(FISH_RADIUS, WINDOW_WIDTH - FISH_RADIUS);
            int y = GetRandomValue(FISH_RADIUS, WINDOW_HEIGHT - FISH_RADIUS);
            Vector2 startPos = { static_cast<float>(x), static_cast<float>(y) };

            // Generate random velocity
            float vx = static_cast<float>(GetRandomValue(-100, 100)) / 100.0f * MAX_FISH_SPEED;
            float vy = static_cast<float>(GetRandomValue(-100, 100)) / 100.0f * MAX_FISH_SPEED;
            Vector2 startVelocity = { vx, vy };

            fish.push_back(Fish(startPos, startVelocity));
        }
    }

    void Update(float deltaTime) {
        for (auto& fish : fish) {
            if (mode == 1) {
                Vector2 seekForce = Seek(fish, GetMousePosition());
                fish.ApplyForce(seekForce);
            }
            else if (mode == 2) {
                Vector2 fleeForce = Flee(fish, predatorPos);
                fish.ApplyForce(fleeForce);
            }
            else if (mode == 3) {
                Vector2 arrivalForce = Arrival(fish, foodPos);
                fish.ApplyForce(arrivalForce);
            }
            else if (mode == 4) {
                Vector2 avoidForce = AvoidObstacle(fish);
                fish.ApplyForce(avoidForce);
            }

            fish.Update(deltaTime);
            WrapAroundBounds(fish.position);
        }
    }

    void WrapAroundBounds(Vector2& pos) {
        if (pos.x > WINDOW_WIDTH + FISH_RADIUS)
            pos.x = -FISH_RADIUS;
        else if (pos.x < -FISH_RADIUS)
            pos.x = WINDOW_WIDTH + FISH_RADIUS;

        if (pos.y > WINDOW_HEIGHT + FISH_RADIUS)
            pos.y = -FISH_RADIUS;
        else if (pos.y < -FISH_RADIUS)
            pos.y = WINDOW_HEIGHT + FISH_RADIUS;
    }

    Vector2 Seek(const Fish& fish, const Vector2& target) const {
        Vector2 desiredVelocity = Vector2Subtract(target, fish.position);
        desiredVelocity = Vector2Normalize(desiredVelocity);
        desiredVelocity = Vector2Scale(desiredVelocity, MAX_FISH_SPEED);

        Vector2 steeringForce = Vector2Subtract(desiredVelocity, fish.velocity);
        LimitVector(steeringForce, MAX_FISH_ACCELERATION);

        return steeringForce;
    }

    Vector2 Flee(const Fish& fish, const Vector2& predator) const {
        float fleeRadius = 100.0f;
        if (Vector2Distance(fish.position, predator) > fleeRadius) {
            return { 0, 0 };
        }

        Vector2 desiredVelocity = Vector2Subtract(fish.position, predator);
        desiredVelocity = Vector2Normalize(desiredVelocity);
        desiredVelocity = Vector2Scale(desiredVelocity, MAX_FISH_SPEED);

        Vector2 steeringForce = Vector2Subtract(desiredVelocity, fish.velocity);
        LimitVector(steeringForce, MAX_FISH_ACCELERATION);

        return steeringForce;
    }

    Vector2 Arrival(const Fish& fish, const Vector2& target) const {
        float arrivalRadius = 50.0f;
        float slowingRadius = 100.0f;

        Vector2 desiredVelocity = Vector2Subtract(target, fish.position);
        float distance = Vector2Length(desiredVelocity);
        desiredVelocity = Vector2Normalize(desiredVelocity);

        if (distance < arrivalRadius) {
            float m = RemapValue(distance, 0.0f, arrivalRadius, 0.0f, MAX_FISH_SPEED);
            desiredVelocity = Vector2Scale(desiredVelocity, m);
        }
        else if (distance < slowingRadius) {
            float m = RemapValue(distance, arrivalRadius, slowingRadius, MAX_FISH_SPEED, 0.0f);
            desiredVelocity = Vector2Scale(desiredVelocity, m);
        }
        else {
            desiredVelocity = Vector2Scale(desiredVelocity, MAX_FISH_SPEED);
        }

        Vector2 steeringForce = Vector2Subtract(desiredVelocity, fish.velocity);
        LimitVector(steeringForce, MAX_FISH_ACCELERATION);

        return steeringForce;
    }

    Vector2 AvoidObstacle(const Fish& fish) const {
        float avoidanceRadius = 50.0f;

        if (Vector2Distance(fish.position, obstaclePos) > avoidanceRadius) {
            return { 0, 0 };
        }

        Vector2 desiredVelocity = Vector2Subtract(fish.position, obstaclePos);
        desiredVelocity = Vector2Normalize(desiredVelocity);
        desiredVelocity = Vector2Scale(desiredVelocity, MAX_FISH_SPEED);

        Vector2 steeringForce = Vector2Subtract(desiredVelocity, fish.velocity);
        LimitVector(steeringForce, MAX_FISH_ACCELERATION);

        return steeringForce;
    }

    void LimitVector(Vector2& vec, float max) const {
        float length = Vector2Length(vec);
        if (length > max) {
            vec = Vector2Normalize(vec);
            vec = Vector2Scale(vec, max);
        }
    }

    void Draw() const {
        for (const Fish& fish : fish) {
            fish.Draw();
        }

        if (mode == 3) {
            DrawCircle(static_cast<int>(foodPos.x), static_cast<int>(foodPos.y), FISH_RADIUS, GREEN);
        }

        if (mode == 2) {
            DrawCircle(static_cast<int>(predatorPos.x), static_cast<int>(predatorPos.y), FISH_RADIUS, RED);
        }

        if (mode == 4) {
            DrawCircle(static_cast<int>(obstaclePos.x), static_cast<int>(obstaclePos.y), FISH_RADIUS, DARKGRAY);
        }
    }

    void Reset() {
        mode = 0;
        foodPos = { -100.0f, -100.0f };
        predatorPos = { -100.0f, -100.0f };
        obstaclePos = { -100.0f, -100.0f };
        fish.clear();

        srand(static_cast<unsigned int>(time(nullptr)));

        for (int i = 0; i < 5; ++i) {
            int x = GetRandomValue(FISH_RADIUS, WINDOW_WIDTH - FISH_RADIUS);
            int y = GetRandomValue(FISH_RADIUS, WINDOW_HEIGHT - FISH_RADIUS);
            Vector2 startPos = { static_cast<float>(x), static_cast<float>(y) };

            // Generate random velocity
            float vx = static_cast<float>(GetRandomValue(-100, 100)) / 100.0f * MAX_FISH_SPEED;
            float vy = static_cast<float>(GetRandomValue(-100, 100)) / 100.0f * MAX_FISH_SPEED;
            Vector2 startVelocity = { vx, vy };

            fish.push_back(Fish(startPos, startVelocity));
        }
    }
};

int main()
{
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Aquarium Simulation");
    SetTargetFPS(60);
    Aquarium aquarium(5);

    while (!WindowShouldClose()) {
        if (IsKeyPressed(KEY_ZERO)) {
            aquarium.mode = 0;
        }
        else if (IsKeyPressed(KEY_ONE)) {
            aquarium.mode = 1;
        }
        else if (IsKeyPressed(KEY_TWO)) {
            aquarium.mode = 2;
        }
        else if (IsKeyPressed(KEY_THREE)) {
            aquarium.mode = 3;
        }
        else if (IsKeyPressed(KEY_FOUR)) {
            aquarium.mode = 4;
        }
        else if (IsKeyPressed(KEY_SPACE)) {
            aquarium.Reset();
        }

        if (aquarium.mode == 1 && IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
            Vector2 targetPos = GetMousePosition();

            for (auto& fish : aquarium.fish) {
                Vector2 seekForce = aquarium.Seek(fish, targetPos);
                fish.ApplyForce(seekForce);
            }
        }
        else if (aquarium.mode == 2 && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            aquarium.predatorPos = GetMousePosition();
        }
        else if (aquarium.mode == 3 && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            aquarium.foodPos = GetMousePosition();
        }
        else if (aquarium.mode == 4 && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            aquarium.obstaclePos = GetMousePosition();
        }

        aquarium.Update(GetFrameTime());

        BeginDrawing();
        ClearBackground(RAYWHITE);

        aquarium.Draw();

        EndDrawing();
    }

    CloseWindow();

    return 0;
}