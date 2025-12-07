#ifndef DHero_H_INCLUDED
#define DHero_H_INCLUDED

#include "../Object.h"
#include "../shapes/Rectangle.h"
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>

class DHero : public Object
{
public:
    void init();
    void update();
    void draw() override;
    
    void set_position(double x, double y);
    
    double speed = 5.0;
    int width() const { return w; }
    int height() const { return h; }

private:
    int w = 50;
    int h = 50;
    ALLEGRO_BITMAP *img = nullptr;
    
    // TODO: Future Image Replacement
    // 1. Ensure 'img' is loaded in init() using ImageCenter.
    //    img = ImageCenter::get_instance()->get("path/to/hero_image.png");
    // 2. In draw(), the code already checks for 'img'. Just make sure it's loaded correctly.
};

#endif