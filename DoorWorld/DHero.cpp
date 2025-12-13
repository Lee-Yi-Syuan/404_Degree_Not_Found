#include "DHero.h"
#include "../data/DataCenter.h"
#include <allegro5/allegro_primitives.h>
#include <cstdio>
#include <cstring>

void DHero::init() {
    DataCenter *DC = DataCenter::get_instance();
    
    shape.reset(new Rectangle(
        DC->window_width / 2.0 - 25, 
        DC->window_height / 2.0 - 25,
        DC->window_width / 2.0 + 25, 
        DC->window_height / 2.0 + 25
    ));

    hearts = 5;       
    base_speed = 5.0;
    boost_speed = 8.0;
    speed = base_speed;

    heal.reset();
    shield.reset();
    speedup.reset();
    

}

void DHero::update() {
    DataCenter *DC = DataCenter::get_instance();
    float delta_time = 1.0 / 60.0; 

    // ... (技能冷卻邏輯保持不變) ...
    // 技能冷卻
    if (heal.on_cooldown) {
        heal.cooldown -= delta_time;
        if (heal.cooldown <= 0) heal.on_cooldown = false;
    }
    if (shield.active) {
        shield.duration -= delta_time;
        if (shield.duration <= 0) shield.active = false;
    }
    if (shield.on_cooldown) {
        shield.cooldown -= delta_time;
        if (shield.cooldown <= 0) shield.on_cooldown = false;
    }
    if (speedup.active) {
        speedup.duration -= delta_time;
        if (speedup.duration <= 0) {
            speedup.active = false;
            speed = base_speed;
        }
    }
    if (speedup.on_cooldown) {
        speedup.cooldown -= delta_time;
        if (speedup.cooldown <= 0) speedup.on_cooldown = false;
    }

    // 鍵盤觸發技能
    // 原來的 J/K/L 技能鍵改成 A/S/D
    
    // 技能 A: 治療 (原本是 J)
    if (DC->key_state[ALLEGRO_KEY_A] && !DC->prev_key_state[ALLEGRO_KEY_A]) {
        if (!heal.on_cooldown) {
            hearts++; 
            heal.on_cooldown = true;
            heal.cooldown = 15.0; 
        }
    }
    // 技能 S: 護盾 (原本是 K)
    if (DC->key_state[ALLEGRO_KEY_S] && !DC->prev_key_state[ALLEGRO_KEY_S]) {
        if (!shield.on_cooldown) {
            shield.active = true;
            shield.duration = 3.0;
            shield.on_cooldown = true;
            shield.cooldown = 10.0;
        }
    }
    // 技能 D: 加速 (原本是 L)
    if (DC->key_state[ALLEGRO_KEY_D] && !DC->prev_key_state[ALLEGRO_KEY_D]) {
        if (!speedup.on_cooldown) {
            speedup.active = true;
            speed = boost_speed;
            speedup.duration = 5.0;
            speedup.on_cooldown = true;
            speedup.cooldown = 8.0;
        }
    }

    // 移動控制
    // 原來的 W/A/S/D 移動鍵改成 上/下/左/右 方向鍵
    double dx = 0, dy = 0;
    if (DC->key_state[ALLEGRO_KEY_UP]) dy -= speed;
    if (DC->key_state[ALLEGRO_KEY_DOWN]) dy += speed;
    if (DC->key_state[ALLEGRO_KEY_LEFT]) dx -= speed;
    if (DC->key_state[ALLEGRO_KEY_RIGHT]) dx += speed;

    double new_x = shape->center_x() + dx;
    double new_y = shape->center_y() + dy;
    
    // ... (邊界檢查邏輯保持不變) ...
    // 邊界檢查
    double w = shape->x2 - shape->x1;
    double h = shape->y2 - shape->y1;
    double half_w = w / 2.0;
    double half_h = h / 2.0;
    
    if (new_x - half_w < 0) new_x = half_w;
    if (new_x + half_w > DC->window_width) new_x = DC->window_width - half_w;
    if (new_y - half_h < 0) new_y = half_h;
    if (new_y + half_h > DC->window_height) new_y = DC->window_height - half_h;

    shape->update_center_x(new_x);
    shape->update_center_y(new_y);
}

void DHero::draw() {
    if (!shape) return; // 防呆

    ALLEGRO_COLOR color = al_map_rgb(100, 100, 255);
    if (shield.active) {
        color = al_map_rgb(200, 200, 255);
        double w = shape->x2 - shape->x1;
        al_draw_circle(shape->center_x(), shape->center_y(), w/2 + 5, al_map_rgb(255, 215, 0), 3);
    }
    
    al_draw_filled_rectangle(shape->x1, shape->y1, shape->x2, shape->y2, color);
    
    if (speedup.active) {
        al_draw_rectangle(shape->x1-2, shape->y1-2, shape->x2+2, shape->y2+2, al_map_rgb(0, 255, 0), 2);
    }
}