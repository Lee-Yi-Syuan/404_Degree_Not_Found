#include "DEnemy.h"
#include "../data/DataCenter.h"
#include <allegro5/allegro_primitives.h>

void DEnemy::init(double start_x, double start_y, double speed_x, double speed_y) {
    x = start_x;
    y = start_y;
    dx = speed_x;
    dy = speed_y;
    w = 40;
    h = 40;
    
    // Initialize shape for collision
    shape.reset(new Rectangle(x, y, x + w, y + h));
    
    // TODO: Future Image Replacement
    // Load your image here.
    // img = ImageCenter::get_instance()->get("assets/image/enemy.png");
}

void DEnemy::update() {
    DataCenter *DC = DataCenter::get_instance();
    x += dx;
    y += dy;
    
    if(x < 0 || x > DC->window_width - w) dx *= -1;
    if(y < 0 || y > DC->window_height - h) dy *= -1;
    
    // Update collision shape
    if(shape) {
        shape->update_center_x(x + w/2);
        shape->update_center_y(y + h/2);
    }
}

void DEnemy::draw() {
    // TODO: Future Image Replacement
    // Replace the following line with image drawing code:
    // if(img) {
    //     al_draw_bitmap(img, x, y, 0);
    // } else {
    //     al_draw_filled_rectangle(x, y, x + w, y + h, al_map_rgb(255, 0, 0));
    // }
    
    al_draw_filled_rectangle(x, y, x + w, y + h, al_map_rgb(255, 0, 0)); // Red Enemy
}

void DEnemy::set_position(double new_x, double new_y) {
    x = new_x;
    y = new_y;
    if(shape) {
        shape->update_center_x(x + w/2);
        shape->update_center_y(y + h/2);
    }
}