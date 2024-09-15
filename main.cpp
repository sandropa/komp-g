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

    // h = y + sign * sqrt(r^2 - (sweepline_x - x)^2)
    int get_height() const {
        int sign = bottom ? 1 : -1;
        int h = y + sign * sqrt(pow(radius, 2) - pow(sweepline_x - x, 2));
        return h;
    }

    /*bool operator<(const Semicircle& other) const {
        // we will use current heights
        // we will be working with ints for heights (that should be enough)

        if (get_height() != other.get_height()) { // h instead of y
            return get_height() < other.get_height();
        }
        return bottom < other.bottom;
    }*/

    bool operator<(const Semicircle& other) const {
        // we are using y coordinate
        if (y != other.y) {
            return y < other.y;
        }
        return bottom < other.bottom;
    }

    void print() const {
        cout<<"semicircle "<<x<<" "<<y<<" "<<radius<<(bottom ? " bottom " : " top ")<<sweepline_x<<endl;
    }
};

int Semicircle::sweepline_x = 0; 

struct Circle {
    int x;
    int y;
    int radius;
    Circle(int x, int y, int radius) : x{x}, y{y}, radius{radius} {}
    void draw() {
        DrawCircleLines(x, y, radius, WHITE);
    }
};

/* bool circles_intersect(Circle a, Circle b) {
    double d = sqrt(pow(a.x - b.x, 2) + pow(a.y - b.y, 2));
    return (abs(a.radius - b.radius) < d) && (d < (a.radius + b.radius));
} */

// Function to check intersection and draw points
bool circles_intersect(Circle a, Circle b) { // ChatGPT
    // Distance between the centers of the circles
    double d = sqrt(pow(a.x - b.x, 2) + pow(a.y - b.y, 2));

    // Check if the circles intersect
    if ((abs(a.radius - b.radius) < d) && (d < (a.radius + b.radius))) {
        // Now calculate intersection points

        double aSquared = pow(a.radius, 2);
        double bSquared = pow(b.radius, 2);
        double dSquared = pow(d, 2);

        // Distance from circle A's center to the line between intersection points
        double distToLine = (aSquared - bSquared + dSquared) / (2 * d);

        // Midpoint on the line between the centers of the two circles
        double xMid = a.x + distToLine * (b.x - a.x) / d;
        double yMid = a.y + distToLine * (b.y - a.y) / d;

        // Height from the line to the intersection points
        double h = sqrt(aSquared - pow(distToLine, 2));

        // Offset from the midpoint to the intersection points
        double offsetX = h * (b.y - a.y) / d;
        double offsetY = h * (b.x - a.x) / d;

        // Intersection points
        Vector2 intersection1 = { static_cast<float>(xMid + offsetX), static_cast<float>(yMid - offsetY) };
        Vector2 intersection2 = { static_cast<float>(xMid - offsetX), static_cast<float>(yMid + offsetY) };

        // Draw the intersection points
        DrawCircleV(intersection1, 5, RED); // Draw first point in red
        DrawCircleV(intersection2, 5, RED); // Draw second point in red

        return true;
    }

    return false;
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
        
        /*cout<<"TRENUTNO STANJE"<<endl;
        for(auto c : active_semicircles)
            c.print();*/

        if(active_semicircles.count(curr_top_smc)) {
            // this means we need to erase both top and bottom
            // let's first check that if the neighbours are intersecting


            if(active_semicircles.find(curr_top_smc) != active_semicircles.begin()) {
                if(active_semicircles.find(curr_bottom_smc)++ != active_semicircles.end()) {
                    //cout<<"OVDJE ";

                    Semicircle top_neighbour = *(--active_semicircles.find(curr_top_smc));
                    Semicircle bottom_neighbour = *(++active_semicircles.find(curr_bottom_smc));

                    /*curr_top_smc.print();
                    cout<<endl<<"TOP N ";
                    top_neighbour.print();
                    cout<<endl<<"BOTTOM N ";
                    bottom_neighbour.print();*/

                    Circle top_circle{top_neighbour.x, top_neighbour.y, top_neighbour.radius};
                    Circle bottom_circle{bottom_neighbour.x, bottom_neighbour.y, bottom_neighbour.radius};
                    if(circles_intersect(top_circle, bottom_circle))
                        return true;
                }
            } 

            // active_semicircles.erase(curr_bottom_smc); // maybe for the number of intersections
            // active_semicircles.erase(curr_top_smc); // maybe for the number of intersections
        } else {
            // this means we need to insert both top and bottom
            active_semicircles.insert(curr_bottom_smc);
            active_semicircles.insert(curr_top_smc);

            // We are comparing curr_c and top_circle and bottom_circle
            // we already have curr_c
            // top_c = *iter_top
            // bottom_c = *iter_bottom
            if(active_semicircles.find(curr_top_smc) != active_semicircles.begin()) {
                Semicircle top_neighbour = *(--active_semicircles.find(curr_top_smc));
                Circle top_circle{top_neighbour.x, top_neighbour.y, top_neighbour.radius};
                if(circles_intersect(top_circle, curr_c))
                    return true;
            }
            if(++active_semicircles.find(curr_bottom_smc) != active_semicircles.end()) {
                Semicircle bottom_neighbour = *(++active_semicircles.find(curr_bottom_smc));
                Circle bottom_circle{bottom_neighbour.x, bottom_neighbour.y, bottom_neighbour.radius};
                if(circles_intersect(bottom_circle, curr_c))
                    return true;
            }
        }
    }

    return false;
}

int main() {
    // TMP =============================================================================
    Semicircle::sweepline_x = -50;
    Semicircle a{0, 0, 50, false};
    Semicircle b{0, 0, 50, true};
    Semicircle c{40, 30, 5, true};
    Semicircle d{40, 30, 5, false};

    set<Semicircle> smcs;
    smcs.insert(a);
    smcs.insert(b);
    Semicircle::sweepline_x = 40 - 5;
    smcs.insert(c);
    smcs.insert(d);

    for(auto it = smcs.begin(); it != smcs.end(); it++)
        it->print();

    // TMP =============================================================================

    // Initialization
    const int screenWidth = 1600;
    const int screenHeight = 900;
    InitWindow(screenWidth, screenHeight, "Intersecting circles");

    SetTargetFPS(2);

    vector<Circle> circles;

    /*
    We can identify circles just by their centers, and if we add bottom or top
    for the semicircles, we can identify them too (we are solving inly the general case)
    */

    // EXAMPLE: RANDOM
    /*for(int i = 0; i < 20; i++) {
        int x = GetRandomValue(0, screenWidth);
        int y = GetRandomValue(0, screenHeight);
        int radius = GetRandomValue(0, 100);

        circles.push_back(Circle(x,y,radius));
    }*/

    // EXAMPLE: 3 CIRCLES
    // circles.push_back(Circle(400, 300, 140));
    // circles.push_back(Circle(300, 100, 100));
    // circles.push_back(Circle(260,220, 10));
    

    int num_c = 0;
    Vector2 center_pos;
    while(!WindowShouldClose()){
        BeginDrawing();

        for (Circle c : circles)
            c.draw();

        EndDrawing();

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            // Get mouse position
            Vector2 mousePos = GetMousePosition();

            if(num_c % 2 == 0) {
                center_pos = mousePos;
            } else {
                circles.push_back(Circle(center_pos.x, center_pos.y, sqrt(pow(center_pos.y - mousePos.y, 2) + pow((center_pos.x - mousePos.x), 2))));
            }

            // Draw a dot (small circle) at the mouse position
            DrawCircleV(mousePos, 2, GREEN);  // 5 is the radius, RED is the color
            num_c++;
        }

        if (IsKeyPressed(KEY_ENTER))
        {
            cout<<"Circles intersect: ";
            // cout<<slow_intersections(circles)<<endl;
            cout<<fast_intersections(circles)<<endl;
        }
    }

    return 0;
}
