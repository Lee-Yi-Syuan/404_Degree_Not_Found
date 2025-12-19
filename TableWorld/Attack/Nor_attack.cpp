#include "Nor_attack.h"
#include "../../data/DataCenter.h"
#include "../../data/ImageCenter.h"
#include "../../shapes/Rectangle.h"
#include "../T_Hero.h"
#include <allegro5/allegro_primitives.h>
#include <cmath>

void NorAttack::init(int x_pos, int y_pos, bool face_left)
{
    x = x_pos;
    y = y_pos;
    facing_left = face_left;
    life_counter = 0;
    max_life = 10; // 持續 10 frames
    alive = true;
    ImageCenter *IC = ImageCenter::get_instance();
    img = IC->get("./assets/image/darksword.png");
    width = al_get_bitmap_width(img);
    height = al_get_bitmap_height(img);
    
    
    shape.reset(new Rectangle(x - width/2, y - height/2, x + width/2, y + height/2));
}

void NorAttack::update()
{
    if(alive) {
        life_counter++;
        if(life_counter >= max_life) {
            alive = false;
        }

        // Update position to follow hero
        DataCenter *DC = DataCenter::get_instance();
        if(DC->thero) {
            int hero_x = DC->thero->shape->center_x();
            int hero_y = DC->thero->shape->center_y();
            
            // Re-calculate position based on facing direction
            // Note: We keep the initial facing_left unless we want to update it too.
            // If we want the attack to flip if the hero turns mid-attack:
            // facing_left = !DC->thero->face_right; 
            
            // For now, let's just update position but keep initial direction (standard for many games)
            // Or should we update direction? "Move with character" usually implies position.
            // Let's update position relative to hero center.
            
            if(!facing_left) {
                x = hero_x + DC->thero->width()/2; 
            } else {
                x = hero_x - DC->thero->width()/2;
            }
            y = hero_y;
            
            // Update shape
            shape->update_center_x(x);
            shape->update_center_y(y);
        }
    }
}

void NorAttack::interact()
{
    // Interaction logic for NorAttack can be added here
}

void NorAttack::draw()
{
    if(!alive) return;

    float progress = (float)life_counter / max_life;
    
    // Calculate angle
    float center_angle;
    float sweep = ALLEGRO_PI / 2; // 90 degrees
    float current_angle;

    if (facing_left) {
    center_angle  = ALLEGRO_PI;                       // 左
    current_angle = center_angle - sweep/2 + sweep*progress;

    // 不翻轉，直接旋轉
    al_draw_rotated_bitmap(img, 0, height/2, x, y, current_angle, 0);
} else {
    center_angle  = 0;                                // 右
    current_angle = center_angle - sweep/2 + sweep*progress;

    al_draw_rotated_bitmap(img, 0, height/2, x, y, current_angle, 0);
}
}