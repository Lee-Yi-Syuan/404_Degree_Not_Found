#include "B_Nor_attack.h"
#include "../../data/DataCenter.h"
#include <allegro5/allegro_primitives.h>
#include <cmath>

void BNorAttack::init(double x, double y, double target_x, double target_y) {
    double dx = target_x - x;
    double dy = target_y - y;
    double len = sqrt(dx*dx + dy*dy);
    
    if(len != 0) {
        vx = (dx / len) * speed;
        vy = (dy / len) * speed;
    } else {
        vx = speed;
        vy = 0;
    }
    
    angle = atan2(dy, dx);
    
    shape.reset(new Rectangle(x - 20, y - 20, x + 20, y + 20));
    
    alive = true;
    life_counter = 0;
}

void BNorAttack::update() {
    if(!alive) return;
    
    shape->update_center_x(shape->center_x() + vx);
    shape->update_center_y(shape->center_y() + vy);
    
    life_counter++;
    if(life_counter > max_life) {
        alive = false;
    }
    
    // Boundary check
    DataCenter *DC = DataCenter::get_instance();
    if(shape->center_x() < 0 || shape->center_x() > DC->window_width ||
       shape->center_y() < 0 || shape->center_y() > DC->window_height) {
        alive = false;
    }
}

void BNorAttack::draw() {
    if(!alive) return;
    
    // Draw a "slash" wave (rotated rectangle)
    // Since we don't have rotated rect in Shape, we just draw it visually
    
    ALLEGRO_TRANSFORM t;
    al_identity_transform(&t);
    al_rotate_transform(&t, angle);
    al_translate_transform(&t, shape->center_x(), shape->center_y());
    al_use_transform(&t);
    
    al_draw_filled_rectangle(-20, -10, 20, 10, al_map_rgb(150, 0, 150)); // Purple slash
    al_draw_rectangle(-20, -10, 20, 10, al_map_rgb(255, 255, 255), 2);
    
    al_identity_transform(&t);
    al_use_transform(&t);
}