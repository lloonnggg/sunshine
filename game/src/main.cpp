#include "raylib.h"
#include <vector>
#include <cstdlib>
#include <ctime>

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;
const int FISH_RADIUS = 20;
const float MAX_FISH_ACCELERATION = 400.0f;
const float MAX_FISH_SPEED = 200.0f;

struct Fish {
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
        float speed = sqrtf(velocity.x * velocity.x + velocity.y * velocity.y);
        if (speed > MAX_FISH_SPEED) {
            velocity.x = (velocity.x / speed) * MAX_FISH_SPEED;
            velocity.y = (velocity.y / speed) * MAX_FISH_SPEED;
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

class Aquarium {
public:
    std::vector<Fish> fish;

    Aquarium(int numFish) {
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
            fish.Update(deltaTime);
            WrapAroundBounds(fish.position);
        }
    }

    void WrapAroundBounds(Vector2& pos) {
        if (pos.x > WINDOW_WIDTH) {
            pos.x = 0;
        }
        else if (pos.x < 0) {
            pos.x = WINDOW_WIDTH;
        }

        if (pos.y > WINDOW_HEIGHT) {
            pos.y = 0;
        }
        else if (pos.y < 0) {
            pos.y = WINDOW_HEIGHT;
        }
    }

    void Draw() const {
        for (const Fish& fish : fish) {
            fish.Draw();
        }
    }
};

int main() {
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Aquarium Simulation");
    SetTargetFPS(60);

    Aquarium aquarium(5);

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        aquarium.Update(GetFrameTime());
        aquarium.Draw();

        EndDrawing();
    }

    CloseWindow();

    return 0;
}