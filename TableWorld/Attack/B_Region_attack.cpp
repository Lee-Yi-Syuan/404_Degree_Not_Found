#include "B_Region_attack.h"
#include <allegro5/allegro_primitives.h>
#include <cmath>

void BRegionAttack::init(double x, double y) {
    shape.reset(new Circle(x, y, 0.0));
    alive = true;
    wave_count = 0;
    timer = 0;
    current_radius = 0;
}

void BRegionAttack::update() {
    if(!alive) return;
    
    timer++;
    float progress = (float)timer / wave_duration;
    
    current_radius = max_radius * progress;
    
    // Update collision shape
    Circle* c = dynamic_cast<Circle*>(shape.get());
    if(c) {
        c->r = current_radius;
    }
    
    if(timer >= wave_duration) {
        wave_count++;
        timer = 0;
        current_radius = 0;
        if(wave_count >= max_waves) {
            alive = false;
        }
    }
}

void BRegionAttack::draw() {
    if(!alive) return;
    
    float progress = (float)timer / wave_duration;
    float alpha = 1.0f - progress;
    
    // Draw expanding purple circle
    al_draw_filled_circle(shape->center_x(), shape->center_y(), current_radius, al_map_rgba_f(0.5, 0.0, 0.5, alpha * 0.5));
    al_draw_circle(shape->center_x(), shape->center_y(), current_radius, al_map_rgba_f(0.5, 0.0, 0.5, alpha), 5);
}