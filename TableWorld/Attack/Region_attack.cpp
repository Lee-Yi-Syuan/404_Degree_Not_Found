#include "Region_attack.h"
#include "../../data/DataCenter.h"
#include "../../data/ImageCenter.h"
#include "../../shapes/Circle.h"
#include <allegro5/allegro_primitives.h>
#include <cmath>

void RegionAttack::init(int x_pos, int y_pos, bool face_left)
{
    x = x_pos;
    y = y_pos;
    facing_left = face_left;
    life_counter = 0;
    max_life = 20; // 持續 20 frames
    alive = true;
    width = 100;
    height = 100;
    
    shape.reset(new Circle(x, y, 100));
}

void RegionAttack::update()
{
    if(alive) {
        life_counter++;
        if(life_counter >= max_life) {
            alive = false;
        }
    }
}

void RegionAttack::interact()
{
    // Interaction logic for RegionAttack can be added here
}

void RegionAttack::draw()
{
    if(!alive) return;

    float progress = (float)life_counter / max_life;
    // 紅色範圍攻擊：擴散的圓圈
    float max_radius = 100;
    float current_radius = max_radius * progress;
    
    // 顏色從不透明變透明
    float alpha = 1.0f - progress;
    
    // 畫實心圓
    al_draw_filled_circle(x, y, current_radius, al_map_rgba_f(1.0, 0.0, 0.0, alpha * 0.5));
    
    // 畫圓框
    al_draw_circle(x, y, current_radius, al_map_rgba_f(1.0, 0.0, 0.0, alpha), 5);
}