#include <iostream>
#include <raylib.h>

using namespace std;

int main () {

    const int screenWidth = 1600;
    const int screenHeight = 900;

    InitWindow(screenWidth, screenHeight, "Projekat iz kompjutacione geometrije.");
    SetTargetFPS(60);

    while (WindowShouldClose() == false){
        BeginDrawing();

        // DrawCircleLines(100,200,50, WHITE);
        for (int i = 0; i < 20; i++)
        {
            int x = GetRandomValue(0, screenWidth);
            int y = GetRandomValue(0, screenHeight);
            int radius = GetRandomValue(0, 100);
            Color color = (Color){ GetRandomValue(0, 255), GetRandomValue(0, 255), GetRandomValue(0, 255), 255 };

            DrawCircleLines(x, y, radius, color);
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}