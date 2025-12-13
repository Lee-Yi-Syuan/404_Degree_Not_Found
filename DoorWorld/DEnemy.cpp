#include "DEnemy.h"
#include "../data/DataCenter.h"
#include <allegro5/allegro_primitives.h>

void DEnemy::init(double x, double y, double dx, double dy) {
    shape.reset(new Rectangle(x, y, x + 30, y + 30));
    this->dx = dx;
    this->dy = dy;
}

void DEnemy::update() {
    DataCenter *DC = DataCenter::get_instance();
    
    double new_x = shape->center_x() + dx;
    double new_y = shape->center_y() + dy;
    
    double w = shape->x2 - shape->x1;
    double h = shape->y2 - shape->y1;
    double half_w = w / 2.0;
    double half_h = h / 2.0;

    if (new_x - half_w < 0 || new_x + half_w > DC->window_width) {
        dx *= -1;
        new_x += dx;
    }
    if (new_y - half_h < 0 || new_y + half_h > DC->window_height) {
        dy *= -1;
        new_y += dy;
    }
    
    shape->update_center_x(new_x);
    shape->update_center_y(new_y);
}

void DEnemy::draw() {
    if (!shape) return;
    al_draw_filled_rectangle(shape->x1, shape->y1, shape->x2, shape->y2, al_map_rgb(255, 50, 50));
}