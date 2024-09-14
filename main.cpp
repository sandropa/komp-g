#include "raylib.h"
#include <iostream>
#include <vector>
#include <cmath>
#include <string>
#include <utility>
#include <algorithm>
#include <set>

using namespace std;

struct Semicircle {
    int x;
    int y;
    int radius;
    bool bottom;
    static int sweepline_x;

    Semicircle(int x, int y, int radius, bool bottom) 
        : x{x}, y{y}, radius{radius}, bottom{bottom} {}
    
    bool operator<(const Semicircle& other) const {
        // we are using y coordinate
        if (y != other.y) {
            return y < other.y;
        }
        return bottom < other.bottom;
    }
};

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


bool fast_intersections(vector<Circle>& circles) {
    vector<pair<int, Circle>> events;
    // int represents priority, that is x coordinate
    // in the set we will add the circle if it is not there, otherwise we will remove it
    for(Circle c : circles) {
        events.push_back({c.x - c.radius, c});
        events.push_back({c.x + c.radius, c});
    }
    sort(events.begin(), events.end(), [](const pair<int, Circle>& a, const pair<int, Circle>& b) {
        return a.first < b.first;
    });

    // now events are properly sorted

    set<Semicircle> active_semicircles;

    for(auto event : events) {
        Circle curr_c = event.second;
        Semicircle curr_top_smc = Semicircle(curr_c.x, curr_c.y, curr_c.radius, false);
        Semicircle curr_bottom_smc = Semicircle(curr_c.x, curr_c.y, curr_c.radius, true);
        if(active_semicircles.count(curr_top_smc)) {
            // this means we need to add both top and bottom
            active_semicircles.erase(curr_bottom_smc);
            active_semicircles.erase(curr_top_smc);

            
            // PROVJERITI SUSJEDE
        } else {
            // this means we need to remove both top and bottom
            active_semicircles.insert(curr_bottom_smc);
            active_semicircles.insert(curr_top_smc);

            // PROVJERITI SUSJEDE
        }
    }

    return false;
}

int main() { // int main(void) ??
    // Initialization
    const int screenWidth = 1600;
    const int screenHeight = 900;
    InitWindow(screenWidth, screenHeight, "Random Circles Example");

    SetTargetFPS(60);

    vector<Circle> circles;

    /*
    We can identify circles just by their centers, and if we add bottom or top
    for the semicircles, we can identify them too (we are solving inly the general case)
    */

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
            cout<<"Number of intersections: "<<slow_intersections(circles)<<endl;
            cout<<fast_intersections(circles)<<endl;
        }
    }

    return 0;
}
