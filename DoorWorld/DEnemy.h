#ifndef DENEMY_H
#define DENEMY_H

#include "../shapes/Rectangle.h"
#include <memory>

class DEnemy {
public:
    void init(double x, double y, double dx, double dy);
    void update();
    void draw();
    
    void set_position(double x, double y) {
        double w = shape->x2 - shape->x1;
        double h = shape->y2 - shape->y1;
        shape->update_center_x(x + w/2);
        shape->update_center_y(y + h/2);
    }
    
    // 這裡我們把 width/height 計算也封裝在 .h 裡，或者也可以移到 .cpp
    double get_w() const { return shape->x2 - shape->x1; }
    double get_h() const { return shape->y2 - shape->y1; }
    
    std::unique_ptr<Rectangle> shape;
    double dx, dy;
};

#endif