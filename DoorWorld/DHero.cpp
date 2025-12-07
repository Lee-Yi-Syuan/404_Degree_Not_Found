#include "DHero.h"
#include "../data/DataCenter.h"
#include "../data/ImageCenter.h"
#include "../shapes/Rectangle.h"
#include <allegro5/allegro_primitives.h>

void DHero::init()
{
    // Load image if available, otherwise use placeholder
    // ImageCenter *IC = ImageCenter::get_instance();
    // img = IC->get("./assets/image/dhero.png"); 
    
    // TODO: Future Image Replacement
    // Uncomment the lines above and ensure the path is correct to load the hero image.
    // If img is loaded, draw() will automatically use it.
    
    w = 40;
    h = 40;
    
    DataCenter *DC = DataCenter::get_instance();
    shape.reset(new Rectangle(DC->window_width / 2, DC->window_height / 2, 
                              DC->window_width / 2 + w, DC->window_height / 2 + h));
}

void DHero::update()
{
    DataCenter *DC = DataCenter::get_instance();
    double dx = 0, dy = 0;
    
    if(DC->key_state[ALLEGRO_KEY_UP]) dy -= speed;
    if(DC->key_state[ALLEGRO_KEY_DOWN]) dy += speed;
    if(DC->key_state[ALLEGRO_KEY_LEFT]) dx -= speed;
    if(DC->key_state[ALLEGRO_KEY_RIGHT]) dx += speed;
    
    shape->update_center_x(shape->center_x() + dx);
    shape->update_center_y(shape->center_y() + dy);
    
    // Boundary Check
    if(shape->center_x() < w/2) shape->update_center_x(w/2);
    if(shape->center_x() > DC->window_width - w/2) shape->update_center_x(DC->window_width - w/2);
    if(shape->center_y() < h/2) shape->update_center_y(h/2);
    if(shape->center_y() > DC->window_height - h/2) shape->update_center_y(DC->window_height - h/2);
}

void DHero::draw()
{
    if(img) {
        al_draw_bitmap(img, shape->center_x() - w/2, shape->center_y() - h/2, 0);
    } else {
        al_draw_filled_rectangle(shape->center_x() - w/2, shape->center_y() - h/2,
                                 shape->center_x() + w/2, shape->center_y() + h/2,
                                 al_map_rgb(0, 255, 0)); // Green box
    }
}

void DHero::set_position(double x, double y) {
    shape->update_center_x(x);
    shape->update_center_y(y);
}