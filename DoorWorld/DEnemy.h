#ifndef DENEMY_H
#define DENEMY_H

#include "../Object.h"
#include "../shapes/Rectangle.h"
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>

class DEnemy : public Object
{
public:
    void init(double start_x, double start_y, double speed_x, double speed_y);
    void update();
    void draw() override;

    // Getters for collision detection
    double get_x() const { return x; }
    double get_y() const { return y; }
    int get_w() const { return w; }
    int get_h() const { return h; }
    
    // Reset position (e.g. after collision)
    void set_position(double new_x, double new_y);

public:
    double x, y;
    double dx, dy;
    int w, h;
    // Rectangle shape; // Inherited from Object as std::shared_ptr<Shape> shape

private:
    // TODO: Future Image Replacement
    // 1. Add an ALLEGRO_BITMAP* member variable here:
    //    ALLEGRO_BITMAP *img = nullptr;
    // 2. In init(), load the image using ImageCenter:
    //    img = ImageCenter::get_instance()->get("path/to/enemy_image.png");
    // 3. In draw(), replace al_draw_filled_rectangle with:
    //    if(img) al_draw_bitmap(img, x, y, 0);
};

#endif