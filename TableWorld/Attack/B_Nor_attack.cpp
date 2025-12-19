#include "B_Nor_attack.h"
#include "../../data/DataCenter.h"
#include "../Boss.h"
#include "../../data/ImageCenter.h"
#include <allegro5/allegro_primitives.h>
#include <cmath>

void BNorAttack::init(double x, double y, double target_x, double target_y) 
{
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

    // 取得子彈貼圖（依你的專案換成正確路徑/Key）
    ImageCenter *IC = ImageCenter::get_instance();
    bullet_img = IC->get("assets/image/dydx.png");

    alive = true;
    life_counter = 0;
}

void BNorAttack::update() {
    if(!alive) return;
    
    // 用速度向量計算出的改變量更新位置
    shape->update_center_x(shape->center_x() + vx);
    shape->update_center_y(shape->center_y() + vy);
    
    life_counter++;
    if(life_counter > max_life) {
        alive = false;
    }
    
    DataCenter *DC = DataCenter::get_instance();
    // 確認邊界
    if(shape->center_x() < 0 || shape->center_x() > DC->window_width ||
       shape->center_y() < 0 || shape->center_y() > DC->window_height) {
        alive = false;
    }
}

void BNorAttack::draw() {
    if(!alive || !bullet_img) return;

    float cx = (float)shape->center_x();
    float cy = (float)shape->center_y();

    float w = (float)al_get_bitmap_width(bullet_img);
    float h = (float)al_get_bitmap_height(bullet_img);

    // 以貼圖中心旋轉，畫在子彈中心
    al_draw_rotated_bitmap(bullet_img, w/2, h/2, cx, cy, (float)angle, 0);
}