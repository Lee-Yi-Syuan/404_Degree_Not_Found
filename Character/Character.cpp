#include "Character.h"
#include "../object/bed.h"
#include "../object/closet.h"
#include "../object/table.h"
#include "../object/floor.h"
#include "../data/DataCenter.h"
#include "../data/ImageCenter.h"
#include "../shapes/Point.h"
#include "../shapes/Rectangle.h"
#include "../Utils.h"
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include <iostream>

void Character::init()
{
    // 初始化基本數值
    speed = 5.0f; 
    animation_tick = 0;
    face_right = true;
    state = CharacterState::Front;
    movable = true;

    // 載入兩張走路圖片
    move_img1 = al_load_bitmap("./assets/image/player1.png");
    move_img2 = al_load_bitmap("./assets/image/player2.png");

    DataCenter *DC = DataCenter::get_instance();
    
    // --- 調整重點：將邏輯寬高放大兩倍 (50 -> 100) ---
    w = 100.0f; 
    h = 100.0f; 

    if (!move_img1) {
        std::cerr << "Error: Player images not found! Drawing blocks instead." << std::endl;
    }

    // 設定初始位置 (置中)
    shape.reset(new Rectangle{
        DC->window_width / 2.0f - w / 2.0f, 
        DC->window_height / 2.0f - h / 2.0f,
        DC->window_width / 2.0f + w / 2.0f,
        DC->window_height / 2.0f + h / 2.0f});
}

void Character::update()
{
    if(!movable) return;
    DataCenter *DC = DataCenter::get_instance();

    float old_x = shape->center_x();
    float old_y = shape->center_y();
    float dx = 0, dy = 0;
    bool is_moving = false;

    // 分開偵測 X 與 Y，允許斜向移動，並更新 face_right
    if(DC->key_state[ALLEGRO_KEY_LEFT]) {
        state = CharacterState::LEFT;
        dx = -speed; face_right = false; is_moving = true;
    } else if(DC->key_state[ALLEGRO_KEY_RIGHT]) {
        state = CharacterState::RIGHT;
        dx = speed; face_right = true; is_moving = true;
    }
    
    if(DC->key_state[ALLEGRO_KEY_UP]) {
        state = CharacterState::Back;
        dy = -speed; is_moving = true;
    } else if(DC->key_state[ALLEGRO_KEY_DOWN]) {
        state = CharacterState::Front;
        dy = speed; is_moving = true;
    }

    // 更新動畫幀計時
    if (is_moving) animation_tick = (animation_tick + 1) % 40;
    else animation_tick = 0;

    // 更新位置
    shape->update_center_x(old_x + dx);
    shape->update_center_y(old_y + dy);

    // 碰撞檢查邏輯
    if(!DC->floor->is_on_floor(shape->center_x(), shape->center_y())) {
        shape->update_center_x(old_x); shape->update_center_y(old_y);
    }
    
    if(shape->center_x() < 0 || shape->center_x() > DC->window_width ||
       shape->center_y() < 0 || shape->center_y() > DC->window_height) {
        shape->update_center_x(old_x); shape->update_center_y(old_y);
    }

    if(shape->overlap(*(DC->bed->shape)) || 
       shape->overlap(*(DC->closet->shape)) || 
       shape->overlap(*(DC->table->shape))) {
        shape->update_center_x(old_x); shape->update_center_y(old_y);
    }
}

void Character::draw()
{
    float cx = shape->center_x();
    float cy = shape->center_y();
    float top = cy - h/2;

    // 繪製角色本體
    ALLEGRO_BITMAP* current_img = (animation_tick < 20) ? move_img1 : move_img2;
    if (current_img) {
        int flags = face_right ? ALLEGRO_FLIP_HORIZONTAL : 0;
        // 使用放大後的 w, h (100x100) 進行繪製
        al_draw_scaled_bitmap(current_img,
            0, 0, al_get_bitmap_width(current_img), al_get_bitmap_height(current_img),
            cx - w/2, cy - h/2, w, h, flags);
    } else {
        // 備援藍色方塊也同步放大
        al_draw_filled_rectangle(cx - w/2, cy - h/2, cx + w/2, cy + h/2, al_map_rgb(0, 0, 255));
    }

    // --- 裝備繪製參數調整 (配合 100x100 尺寸) ---
    if (has_hat) {
        // 加大帽子底邊與高度
        al_draw_filled_triangle(cx - 25, top + 10, cx + 25, top + 10, cx, top - 25, al_map_rgb(255, 0, 0));
    }
    if (has_clothes) {
        // 加大衣服寬度與覆蓋高度
        al_draw_filled_rectangle(cx - 25, top + 35, cx + 25, top + 85, al_map_rgb(0, 0, 255));
    }
    if (has_sunglasses) {
        float eye_y = top + 25; // 調整眼睛位置高度
        al_draw_filled_circle(cx - 10, eye_y, 6, al_map_rgb(0, 0, 0)); // 加大鏡片
        al_draw_filled_circle(cx + 10, eye_y, 6, al_map_rgb(0, 0, 0));
        al_draw_line(cx - 10, eye_y, cx + 10, eye_y, al_map_rgb(0, 0, 0), 3); // 加粗連線
    }
}

void Character::toggle_equipped_item(EquippedItem item) {
    switch(item) {
        case EquippedItem::Hat: has_hat = !has_hat; break;
        case EquippedItem::Clothes: has_clothes = !has_clothes; break;
        case EquippedItem::Sunglasses: has_sunglasses = !has_sunglasses; break;
        default: break;
    }
}

bool Character::is_equipped(EquippedItem item) const {
    switch(item) {
        case EquippedItem::Hat: return has_hat;
        case EquippedItem::Clothes: return has_clothes;
        case EquippedItem::Sunglasses: return has_sunglasses;
        default: return false;
    }
}