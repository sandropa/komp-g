#include "raylib.h"
#include <iostream>
#include <vector>
#include <cmath>
#include <string>

using namespace std;

struct Circle {
    int x;
    int y;
    int radius;
    Color color; // either "white" or "red"
    Circle(int x, int y, int radius, Color color) : x{x}, y{y}, radius{radius}, color{color} {}
    void draw() {
        DrawCircleLines(x, y, radius, color);
    }
};

bool circles_intersect(Circle a, Circle b) {
    double d = sqrt(pow(a.x - b.x, 2) + pow(a.y - b.y, 2));
    return (abs(a.radius - b.radius) < d) && (d < (a.radius + b.radius));
}

int slow_intersections(vector<Circle>& circles) {
    /*
    Returns the number of intersections between given circles, and changes the
    colors of the circles (so that is why we need & and without const).
    The changing of the colors is not what we will be doing in our faster function in the end, but for now
    is fine just to make sure that the algorithm is working.
    */
    int num_intersections = 0;
    int n = circles.size();
    for(int i = 0; i < n; i++) {
        for(int j = i+1; j < n; j++) {
            Circle& a = circles[i];
            Circle& b = circles[j];
            if(circles_intersect(a, b)) {
                num_intersections += 2;
                a.color = RED;
                b.color = RED;
            }
        }
    }
    return num_intersections;
}

int main(void)
{
    // Initialization
    const int screenWidth = 1600;
    const int screenHeight = 900;
    InitWindow(screenWidth, screenHeight, "Random Circles Example");

    SetTargetFPS(60);

    vector<Circle> circles;

    for(int i = 0; i < 20; i++) {
        int x = GetRandomValue(0, screenWidth);
        int y = GetRandomValue(0, screenHeight);
        int radius = GetRandomValue(0, 100);

        circles.push_back(Circle(x,y,radius, WHITE));
    }

    while(!WindowShouldClose()){
        BeginDrawing();

        // Draw random circles
        for (Circle c : circles)
            c.draw();

        EndDrawing();

        if (IsKeyPressed(KEY_ENTER))
        {
            cout<<"Number of intersections: "<<slow_intersections(circles);
        }
    }

    return 0;
}
