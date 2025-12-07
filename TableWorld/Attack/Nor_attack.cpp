#include "Nor_attack.h"
#include "../../data/DataCenter.h"
#include "../../data/ImageCenter.h"
#include "../../shapes/Rectangle.h"
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
    width = 50;
    height = 50;
    
    shape.reset(new Rectangle(x - width/2, y - height/2, x + width/2, y + height/2));
}

void NorAttack::update()
{
    if(alive) {
        life_counter++;
        if(life_counter >= max_life) {
            alive = false;
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
    // 簡單的揮刀動畫：畫一條線或弧線
    // 假設半徑 60
    float radius = 60;

    if(facing_left) {
        // 面向左，從上往下揮
        // Allegro 角度: 0 右, PI/2 下, PI 左, 3PI/2 上
        // 中心角度 PI (左)
        float center_angle = ALLEGRO_PI;
        float sweep = ALLEGRO_PI / 2; // 90度
        // 從 -45度 到 +45度 (相對於中心角度)
        float current_angle = center_angle - sweep/2 + sweep * progress;
        
        // 畫刀身 (線條)
        float x2 = x + radius * cos(current_angle);
        float y2 = y + radius * sin(current_angle);
        al_draw_line(x, y, x2, y2, al_map_rgb(255, 255, 255), 5);
        
        // 畫刀光 (弧線殘影)
        al_draw_arc(x, y, radius, center_angle - sweep/2, sweep * progress, al_map_rgba(200, 200, 255, 150), 5);
    } else {
        // 面向右 (0度)
        float center_angle = 0;
        float sweep = ALLEGRO_PI / 2;
        float current_angle = center_angle - sweep/2 + sweep * progress;

        float x2 = x + radius * cos(current_angle);
        float y2 = y + radius * sin(current_angle);
        al_draw_line(x, y, x2, y2, al_map_rgb(255, 255, 255), 5);
        
        al_draw_arc(x, y, radius, center_angle - sweep/2, sweep * progress, al_map_rgba(200, 200, 255, 150), 5);
    }
}