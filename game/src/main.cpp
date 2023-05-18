#include "rlImGui.h"

//Long_Ho 101411283

//NO.1
//This function initializes a window with the specified width and height.It's useful because it sets up the basic window infrastructure required to render graphics.
// InitWindow(640, 480, "My Window");

//NO.2
// This function sets the target frame rate for the application. It's useful because it can help to smooth out animations and make the application feel more responsive.
// SetTargetFPS(60);

//NO.3
// This function sets up the rendering environment for the current frame. It's useful because it allows you to draw multiple graphics primitives within the same frame.
// BeginDrawing();

//NO.4
// This function draws a string of text on the screen at the specified coordinates. It's useful for displaying information or user interfaces.
// DrawText("Hello, world!", 100, 100, 20, BLACK);

//NO.5
// This function draws a rectangle on the screen at the specified coordinates and with the specified dimensions. It's useful for drawing shapes and backgrounds.
// DrawRectangle(200, 200, 50, 50, BLUE);

//NO.6
// This function loads an image from file into a texture. It's useful for creating custom graphics assets.
// Texture myTexture = LoadTexture("my_texture.png");

//NO.7
// This function releases the memory used by a texture. It's useful for managing resources and avoiding memory leaks.
// UnloadTexture(myTexture);

//NO.8
// This function checks whether a key is currently pressed. It's useful for implementing keyboard-based controls.
// if (IsKeyDown(KEY_SPACE)) {
    // do something
// }

//NO.9
//  This function sets the position of the mouse cursor. It's useful for implementing custom mouse-based controls.
// SetMousePosition(100, 100);

//NO.10
// This function returns a random integer value between two specified values. It's useful for implementing random events or generating random game objects.
// int myRandomValue = GetRandomValue(1, 10);


int main()
{
    // Initialize the window
    const int screenWidth = 800;
    const int screenHeight = 600;

    InitWindow(screenWidth, screenHeight, "Circle Collision");

    // Circle variables
    const int circleRadius = 50;
    Vector2 stationaryCirclePos = { screenWidth / 2, screenHeight / 2 };
    Vector2 movingCirclePos = { screenWidth / 2, screenHeight / 2 };

    // Game loop
    while (!WindowShouldClose())
    {
        // Update

        // Get the mouse position
        Vector2 mousePos = GetMousePosition();

        // Update the position of the moving circle with the mouse position
        movingCirclePos = mousePos;

        // Check collision between circles
        bool isCollision = CheckCollisionCircles(stationaryCirclePos, circleRadius, movingCirclePos, circleRadius);

        // Render

        BeginDrawing();

        ClearBackground(RAYWHITE);

        // Draw stationary circle
        DrawCircleV(stationaryCirclePos, circleRadius, isCollision ? BLACK : PINK);

        // Draw moving circle
        DrawCircleV(movingCirclePos, circleRadius, isCollision ? BLACK : GRAY);

        EndDrawing();
    }

    // Close the window
    CloseWindow();

    return 0;
}