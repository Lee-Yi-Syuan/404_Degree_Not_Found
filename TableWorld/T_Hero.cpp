#include "T_Hero.h"
#include "../data/DataCenter.h"
#include "../data/GIFCenter.h"
#include "../data/ImageCenter.h"
#include "../shapes/Point.h"
#include "../shapes/Rectangle.h"
#include "../Utils.h"
#include "Attack/Nor_attack.h"
#include "Attack/Region_attack.h"
#include <allegro5/allegro_primitives.h>
#include "../data/FontCenter.h"
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <string>
#include <iostream>
#include "Boss.h" 

using namespace std;


namespace CharacterSetting {
    static constexpr char character_imgs_root_path[40] = {
        "./assets/gif/Hero/dragonite_"
    };
    static constexpr char dir_path_postfix[][10] = {
        "left", "right", "front", "back"
    };
}

void THero::init()
{
    // 1. 載入兩張輪播圖片
    move_img1 = al_load_bitmap("./assets/image/player1.png");
    move_img2 = al_load_bitmap("./assets/image/player2.png");

    // 2. 獲取數據中心實例
    DataCenter *DC = DataCenter::get_instance();
    int wh = DC->window_height;
    int ww = DC->window_width;

    // 3. 設定角色邏輯寬高 (放大兩倍版)
    w = 100.0f; 
    h = 100.0f; 

    // 4. 設定角色碰撞箱 (置中)
    shape.reset(new Rectangle{
        ww / 2.0 - w / 2.0, wh / 2.0 - h / 2.0,
        ww / 2.0 + w / 2.0, wh / 2.0 + h / 2.0});
    
    // 5. 初始化所有原本的戰鬥與計時系統
    hp_system.init(100);
    heal_cooldown_timer = 0;
    dash_timer = 0;
    dash_cooldown_timer = 0;
    is_dashing = false;
    invincible_timer = 0;
    stun_timer = 0;
    knockback_vx = 0;
    input_locked = false;
    animation_tick = 0;
    face_right = true; // 預設向右
    state = THeroState::RIGHT;

    if (!move_img1) {
        std::cerr << "Table Hero Error: Player images not found!" << std::endl;
    }
}

void THero::force_move(double dx) {
    if(dx > 0) state = THeroState::RIGHT;
    else if(dx < 0) state = THeroState::LEFT;
    
    shape->update_center_x(shape->center_x() + dx);
    
    // Apply gravity
    gravity.vy += gravity_acc;
    shape->update_center_y(shape->center_y() + gravity.vy);
}

void THero::hit(int damage, bool from_left) {
    if(invincible_timer > 0 || is_dashing) return;
    
    hp_system.take_damage(damage);
    invincible_timer = 60; // 1 second invincibility
    stun_timer = 15; // 0.25 second stun
    
    // Knockback
    knockback_vx = from_left ? 10 : -10;
    gravity.vy = -5; // Small hop
    gravity.on_ground = false;
}


void THero::update()
{
    DataCenter *DC = DataCenter::get_instance();
    // 儲存舊位置
    float old_x = shape->center_x();
    float old_y = shape->center_y();

    // 移動距離
    float dx = 0, dy = 0;

    // 更新計時器 (Timers)
    if(heal_cooldown_timer > 0) heal_cooldown_timer--;
    if(dash_cooldown_timer > 0) dash_cooldown_timer--;
    if(invincible_timer > 0) invincible_timer--;
    if(stun_timer > 0) stun_timer--;
    
    // 擊退衰減 (Knockback decay)
    if(abs(knockback_vx) > 0.1) {
        knockback_vx *= 0.9;
        dx += knockback_vx;
    }

    // 暈眩判斷 (Stunned)
    if(stun_timer > 0) {
        gravity.vy += gravity_acc;
        dy = gravity.vy;
        shape->update_center_x(old_x + dx);
        shape->update_center_y(old_y + dy);
        return;
    }
    
    // 衝刺判斷 (Dashing)
    if(is_dashing) {
        dash_timer--;
        if(dash_timer <= 0) {
            is_dashing = false;
        } else {
            // 衝刺移動速度 (三倍速)
            if(face_right) dx = speed * 3;
            else dx = -speed * 3;
            
            shape->update_center_x(old_x + dx);
            gravity.vy += gravity_acc;
            dy = gravity.vy;
            shape->update_center_y(old_y + dy);
            return; 
        }
    }

    // 重力作用
    gravity.vy += gravity_acc;

    if(input_locked) {
        dy = gravity.vy;
        shape->update_center_x(old_x + dx);
        shape->update_center_y(old_y + dy);
        return;
    }

    // 模式切換 (Q)
    if(DC->key_state[ALLEGRO_KEY_Q] && !DC->prev_key_state[ALLEGRO_KEY_Q])
    {
        int mode = static_cast<int>(current_mode);
        mode = (mode + 1) % static_cast<int>(HeroMode::HeroMode_MAX);
        current_mode = static_cast<HeroMode>(mode);
        printf("Switched to mode: %d\n", mode);
    }

    // 攻擊系統 (Z: 普通攻擊/技能, X: 範圍攻擊)
    if(DC->key_state[ALLEGRO_KEY_Z] && !DC->prev_key_state[ALLEGRO_KEY_Z])
    {
        if(current_mode == HeroMode::ATTACK)
        {
            printf("Normal Attack! (Mode: %d)\n", (int)current_mode);
            NorAttack *attack = new NorAttack();
            int attack_x = shape->center_x();
            int attack_y = shape->center_y();
            
            // 根據 face_right 決定攻擊方向
            if(!face_right) {
                attack_x -= w/2; 
            } else {
                attack_x += w/2; 
            }

            attack->init(attack_x, attack_y, !face_right);
            DC->nor_attacks.push_back(attack);
        }
        else if(current_mode == HeroMode::HEALING)
        {
            if(heal_cooldown_timer <= 0) {
                printf("Healing! (Mode: %d)\n", (int)current_mode);
                int heal_amount = hp_system.get_max_hp() * 0.3;
                hp_system.heal(heal_amount);
                heal_cooldown_timer = 900; // 15 seconds
            }
        }
        else if(current_mode == HeroMode::DODGE)
        {
            if(!is_dashing && dash_cooldown_timer <= 0) {
                printf("Dash! (Mode: %d)\n", (int)current_mode);
                is_dashing = true;
                dash_timer = 20; 
                dash_cooldown_timer = 180; // 3 seconds
            }
        }
    }
    if(DC->key_state[ALLEGRO_KEY_X] && !DC->prev_key_state[ALLEGRO_KEY_X])
    {
        if(current_mode == HeroMode::ATTACK)
        {
            printf("Area Attack! (Mode: %d)\n", (int)current_mode);
            RegionAttack *attack = new RegionAttack();
            int attack_x = shape->center_x();
            int attack_y = shape->center_y();
            attack->init(attack_x, attack_y, !face_right);
            DC->region_attacks.push_back(attack);
        }
    }

    // 鍵盤移動控制
    bool is_moving = false;
    if(DC->key_state[ALLEGRO_KEY_LEFT])
    {
        state = THeroState::LEFT;
        dx = -speed;
        face_right = false;
        is_moving = true;
    }
    else if(DC->key_state[ALLEGRO_KEY_RIGHT])
    {
        state = THeroState::RIGHT;
        dx = speed;
        face_right = true;
        is_moving = true;
    }

    // 動畫計數器
    if(is_moving) animation_tick = (animation_tick + 1) % 40;
    else animation_tick = 0;

    // 跳躍邏輯 (Space)
    if(DC->key_state[ALLEGRO_KEY_SPACE])
    {
        if(gravity.on_ground)
        {
            bool at_bottom = (shape->center_y() + h/2.0 >= DC->window_height - 10); 
            if(DC->key_state[ALLEGRO_KEY_DOWN] && !at_bottom) {
                gravity.on_ground = false;
                gravity.vy = 5.0; 
                drop_start_y = shape->center_y() + h/2.0;
            }
            else {
                gravity.vy = -jump_speed;
                gravity.on_ground = false;
            }
        }
    }

    dy = gravity.vy;

    // 更新位置
    shape->update_center_x(old_x + dx);
    shape->update_center_y(old_y + dy);
}

void THero::draw()
{
    float cx = shape->center_x();
    float cy = shape->center_y();

    // 受傷閃爍效果
    if(invincible_timer > 0 && (invincible_timer / 5) % 2 == 0) {
        // 不繪製
    } else {
        // 繪製角色圖片輪播
        ALLEGRO_BITMAP* current_img = (animation_tick < 20) ? move_img1 : move_img2;
        if (current_img) {
            int flags = face_right ? ALLEGRO_FLIP_HORIZONTAL : 0;
            al_draw_scaled_bitmap(current_img,
                0, 0, al_get_bitmap_width(current_img), al_get_bitmap_height(current_img),
                cx - w/2, cy - h/2, w, h, flags);
        } else {
            // 備援藍色方塊
            al_draw_filled_rectangle(cx - w/2, cy - h/2, cx + w/2, cy + h/2, al_map_rgb(0, 0, 255));
        }
    }

    // 血條 (HP System)
    hp_system.draw_hero_health();
    
    // 繪製技能狀態圖示 (原本的 UI 代碼)
    int start_x = 260;
    int icon_y = 45;
    int icon_size = 30;
    int gap = 5;
    
    FontCenter *FC = FontCenter::get_instance();
    ALLEGRO_FONT *font = FC->caviar_dreams[FontSize::MEDIUM];
    
    for(int i=0; i<3; i++) {
        int icon_x = start_x + i * (icon_size + gap);
        HeroMode mode = static_cast<HeroMode>(i);
        
        ALLEGRO_COLOR bg_color = al_map_rgb(200, 200, 200);
        if(mode == current_mode) bg_color = al_map_rgb(255, 255, 200);
        al_draw_filled_rectangle(icon_x, icon_y, icon_x + icon_size, icon_y + icon_size, bg_color);
        
        ALLEGRO_COLOR border_color = (mode == current_mode) ? al_map_rgb(255, 215, 0) : al_map_rgb(0, 0, 0);
        int border_thickness = (mode == current_mode) ? 4 : 2;
        al_draw_rectangle(icon_x, icon_y, icon_x + icon_size, icon_y + icon_size, border_color, border_thickness);
        
        const char* label = (mode == HeroMode::ATTACK) ? "A" : (mode == HeroMode::HEALING ? "H" : "D");
        if(font) al_draw_text(font, al_map_rgb(0, 0, 0), icon_x + icon_size/2, icon_y + 2, ALLEGRO_ALIGN_CENTER, label);
        
        float cooldown_ratio = 0.0f;
        if(mode == HeroMode::HEALING) cooldown_ratio = (float)heal_cooldown_timer / 900.0f;
        else if(mode == HeroMode::DODGE) {
            if(is_dashing) cooldown_ratio = 1.0f;
            else cooldown_ratio = (float)dash_cooldown_timer / 180.0f;
        }
        
        if(cooldown_ratio > 0) {
            int ch = (int)(icon_size * cooldown_ratio);
            al_draw_filled_rectangle(icon_x, icon_y + icon_size - ch, icon_x + icon_size, icon_y + icon_size, al_map_rgba(0, 0, 0, 150));
        }
    }
}