#include "B_Short_attack.h"
#include <allegro5/allegro_primitives.h>
#include <cmath>

void BShortAttack::init(double x, double y, bool face_left) {
    start_x = x;
    start_y = y;
    facing_left = face_left;
    alive = true;
    stage = 0;
    timer = 0;
    
    // Initial shape
    double offset = facing_left ? -60 : 60;
    shape.reset(new Rectangle(x + offset - 30, y - 30, x + offset + 30, y + 30));
}

void BShortAttack::update() {
    if(!alive) return;
    
    timer++;
    if(timer >= stage_duration) {
        timer = 0;
        stage++;
        if(stage >= 3) {
            alive = false;
            return;
        }
    }
    
    // Update shape based on stage (move forward slightly)
    double offset = facing_left ? -60 : 60;
    double stage_offset = (facing_left ? -30 : 30) * stage;
    
    double cx = start_x + offset + stage_offset;
    double cy = start_y;
    
    shape->update_center_x(cx);
    shape->update_center_y(cy);
}

void BShortAttack::draw() {
    if(!alive) return;
    
    float progress = (float)timer / stage_duration;
    float radius = 100; // Slightly larger than player's range
    
    double cx = shape->center_x();
    double cy = shape->center_y();
    
    float start_angle, end_angle;
    
    if (facing_left) {
        if (stage % 2 == 0) { // Top-Down
            start_angle = 5 * ALLEGRO_PI / 4; // Up-Left
            end_angle = 3 * ALLEGRO_PI / 4;   // Down-Left
        } else { // Bottom-Up
            start_angle = 3 * ALLEGRO_PI / 4;
            end_angle = 5 * ALLEGRO_PI / 4;
        }
    } else {
        if (stage % 2 == 0) { // Top-Down
            start_angle = -ALLEGRO_PI / 4; // Up-Right
            end_angle = ALLEGRO_PI / 4;    // Down-Right
        } else { // Bottom-Up
            start_angle = ALLEGRO_PI / 4;
            end_angle = -ALLEGRO_PI / 4;
        }
    }
    
    float current_angle = start_angle + (end_angle - start_angle) * progress;
    
    ALLEGRO_COLOR color, trail_color;
    if(stage == 0) {
        color = al_map_rgb(255, 165, 0); // Orange
        trail_color = al_map_rgba(255, 165, 0, 150);
    } else if(stage == 1) {
        color = al_map_rgb(255, 0, 0); // Red
        trail_color = al_map_rgba(255, 0, 0, 150);
    } else {
        color = al_map_rgb(139, 0, 0); // Dark Red
        trail_color = al_map_rgba(139, 0, 0, 150);
    }
    
    // Draw Line (Sword)
    float x2 = cx + radius * cos(current_angle);
    float y2 = cy + radius * sin(current_angle);
    al_draw_line(cx, cy, x2, y2, color, 5);
    
    // Draw Arc (Trail)
    al_draw_arc(cx, cy, radius, start_angle, current_angle - start_angle, trail_color, 5);
}