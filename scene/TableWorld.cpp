#include "TableWorld.h"
#include "../TableWorld/T_Floor.h"
#include "../TableWorld/T_Hero.h"
#include "../TableWorld/Boss.h"
#include "../TableWorld/Attack/Nor_attack.h"
#include "../TableWorld/Attack/Region_attack.h"
#include "../TableWorld/Attack/B_Nor_attack.h"
#include "../TableWorld/Attack/B_Region_attack.h"
#include "../TableWorld/Attack/B_Short_attack.h"
#include "../TableWorld/Health_damage.h"
#include "../data/DataCenter.h"
#include "../data/ImageCenter.h"
#include "../data/FontCenter.h"
#include "../Player.h"
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include "../Utils.h"
#include <cstdlib>

TWorldScreen::TWorldScreen()
{
    // 隨機給預設關卡
    int random_type = rand() % (int)TWorldType::TWorldTYPE_MAX;
    current_TWorld_type = static_cast<TWorldType>(random_type);
};

void TWorldScreen::init()
{
    ImageCenter *IC = ImageCenter::get_instance();
    DataCenter *DC = DataCenter::get_instance();

    // 每關不同背景的圖片
    tworld_img_background_paths[TWorldType::Program]    = "./assets/image/Background/DataStructure.png";
    tworld_img_background_paths[TWorldType::Calculus] = "./assets/image/Background/DataStructure.png";
    tworld_img_background_paths[TWorldType::DataStructure]    = "./assets/image/Background/DataStructure.png";


    for (const auto& pair : tworld_img_background_paths) {
        tworld_background_bitmaps[pair.first] = IC->get(pair.second.c_str());
    }

    //勝利與失敗圖片
    tworld_img_result_paths[0] = "./assets/image/tworld/success.png";
    tworld_img_result_paths[1] = "./assets/image/tworld/fail.png";
    //載入勝利與失敗圖片
    for (const auto& pair : tworld_img_result_paths) {
  
        tworld_result_bitmaps[pair.first] = IC->get(pair.second.c_str());
    }
};

void TWorldScreen::start_level()
{
    DataCenter *DC = DataCenter::get_instance();
    
    // Initialize Level State
    level_state = LevelState::ENTRANCE;
    state_timer = 0;
    victory = false;
    fail = false;
    return_to_main = false;
    
    // Setup Hero for Entrance
    if(DC->thero) {
        // Reset HP if needed, or just position
        DC->thero->hp_system.set_hp(DC->thero->hp_system.get_max_hp()); // Optional: Full heal on entry?
        DC->thero->shape->update_center_x(-50); // Start off-screen left
        DC->thero->shape->update_center_y(DC->window_height - 120); // Ensure on ground
        DC->thero->set_input_locked(true);
    }
    
    // Setup Boss
    if(DC->boss) {
        DC->boss->init(); // Reset boss state/HP
        DC->boss->set_active(false);
    }
    
    // Load Map
    if(DC->tfloor) {
        DC->tfloor->load_map(get_current_TWorld_type());
    }
    
    // Clear attacks
    DC->nor_attacks.clear();
    DC->region_attacks.clear();
    DC->boss_nor_attacks.clear();
    DC->boss_region_attacks.clear();
    DC->boss_short_attacks.clear();
}

void TWorldScreen::update()
{
    DataCenter *DC = DataCenter::get_instance();
    TFloor* tfloor=DC->tfloor;
    THero* thero=DC->thero;
    Boss* boss=DC->boss;
    
    switch(level_state) {
        case LevelState::ENTRANCE:
            thero->update();
            tfloor->interact();
            
            // Auto-walk right
            thero->force_move(2.0); // Move right
            
            // Check if reached position
            if(thero->shape->center_x() > 100) {
                level_state = LevelState::WAIT_BOSS;
                state_timer = 0;
            }
            break;
            
        case LevelState::WAIT_BOSS:
            thero->update();
            tfloor->interact();
            
            state_timer++;
            if(state_timer > 300) { // 5 seconds at 60fps
                level_state = LevelState::BATTLE;
                boss->spawn();
                thero->set_input_locked(false);
            }
            break;
            
        case LevelState::BATTLE:
            // 如果Boss或角色都還活著，才更新它們
            if(thero->hp_system.is_alive()) thero->update();
            if(boss->hp_system.is_alive()) boss->update();
            tfloor->interact();
            
            // 更新攻擊特效物件
            for(auto attack : DC->nor_attacks) attack->update();
            for(auto attack : DC->region_attacks) attack->update();
            for(auto attack : DC->boss_nor_attacks) attack->update();
            for(auto attack : DC->boss_region_attacks) attack->update();
            for(auto attack : DC->boss_short_attacks) attack->update();
            
            // 處理傷害碰撞
            DamageSystem::update();
            
            // 確認Boss是否還活著或主角死亡以決定關卡結束
            if(!boss->hp_system.is_alive()) {
                victory = true;
                level_state = LevelState::VICTORY;
            }
            if(!thero->hp_system.is_alive()) {
                fail = true;
                level_state = LevelState::FAIL;
                DC->player->now_fail++;
            }
            break;
            
        case LevelState::VICTORY:
            thero->update();
            tfloor->interact();
            if(thero->shape->center_x() > DC->window_width - 50) {
                return_to_main = true;
            }
            break;
            
        case LevelState::FAIL:
            if(DC->key_state[ALLEGRO_KEY_E] && !DC->prev_key_state[ALLEGRO_KEY_E]) {
                return_to_main = true;
            }
            break;
    }
}

void TWorldScreen::draw()
{
    DataCenter *DC = DataCenter::get_instance();
    //繪製背景
    ALLEGRO_BITMAP *current_background_bitmap = nullptr;

    // 根據當前的關卡類型 (current_TWorld_type) 取得要繪製的圖片
    if (tworld_background_bitmaps.count(current_TWorld_type)) {
        current_background_bitmap = tworld_background_bitmaps[current_TWorld_type];
    }

    // 繪製背景畫面
    if (current_background_bitmap) {
        // 繪製圖片 (假設從 (0, 0) 開始繪製)
        al_draw_bitmap(current_background_bitmap, 0, 0, 0);
    }

    //繪製地板與角色
    TFloor* tfloor=DC->tfloor;
    THero* thero=DC->thero;
    Boss* boss=DC->boss;
    tfloor->draw();
    
    
    if(boss->hp_system.is_alive() && boss->get_active()) {
        boss->draw();
    }

    if(thero->hp_system.is_alive()) {
        thero->draw();
    }

    // 繪製所有攻擊物件
    for(auto attack : DC->nor_attacks) {
        attack->draw();
    }
    for(auto attack : DC->region_attacks) {
        attack->draw();
    }
    for(auto attack : DC->boss_nor_attacks) {
        attack->draw();
    }
    for(auto attack : DC->boss_region_attacks) {
        attack->draw();
    }
    for(auto attack : DC->boss_short_attacks) {
        attack->draw();
    }
    
    // Sunset Overlay
    if(level_state == LevelState::WAIT_BOSS) {
        al_draw_filled_rectangle(0, 0, DC->window_width, DC->window_height, al_map_rgba(255, 100, 0, 60));
    }
    
    // Draw Result Overlay
    if(victory) {
        if(tworld_result_bitmaps.count(0)) {
             // Draw centered or full screen? Assuming full screen or centered overlay
             // Let's draw it centered
             ALLEGRO_BITMAP *bmp = tworld_result_bitmaps[0];
             int bw = al_get_bitmap_width(bmp);
             int bh = al_get_bitmap_height(bmp);
             al_draw_bitmap(bmp, DC->window_width/2 - bw/2, DC->window_height/2 - bh/2, 0);
             
             // Draw hint text
             FontCenter *FC = FontCenter::get_instance();
             al_draw_text(FC->caviar_dreams[FontSize::LARGE], al_map_rgb(255, 255, 255), 
                          DC->window_width/2, DC->window_height/2 + bh/2 + 20, 
                          ALLEGRO_ALIGN_CENTER, "Go Right -> to Exit");
        }
    } else if (fail) {
        if(tworld_result_bitmaps.count(1)) {
             ALLEGRO_BITMAP *bmp = tworld_result_bitmaps[1];
             int bw = al_get_bitmap_width(bmp);
             int bh = al_get_bitmap_height(bmp);
             al_draw_bitmap(bmp, DC->window_width/2 - bw/2, DC->window_height/2 - bh/2, 0);
             
             // Draw hint text
             FontCenter *FC = FontCenter::get_instance();
             al_draw_text(FC->caviar_dreams[FontSize::LARGE], al_map_rgb(255, 255, 255), 
                          DC->window_width/2, DC->window_height/2 + bh/2 + 20, 
                          ALLEGRO_ALIGN_CENTER, "Press E to Return");
        }
    }
}

