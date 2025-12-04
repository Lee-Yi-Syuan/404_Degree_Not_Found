#include "End.h"
#include "../data/DataCenter.h"
#include "../data/ImageCenter.h"
#include "../data/FontCenter.h"
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include "../Utils.h"

EndScreen::EndScreen()
{
    // 預設
    current_end_type = EndType::FAIL_END;
};

void EndScreen::init()
{
    ImageCenter *IC = ImageCenter::get_instance();

    // 結局對應的圖片
    end_img_paths[EndType::FAIL_END]    = "./assets/image/ending/end_fail.jpg";
    end_img_paths[EndType::PREFECT_END] = "./assets/image/ending/end_perfect.jpg";
    end_img_paths[EndType::CLUB_END]    = "./assets/image/ending/end_club.jpg";
    end_img_paths[EndType::LOVE_END]    = "./assets/image/ending/end_love.jpg";
    end_img_paths[EndType::STUDY_END]   = "./assets/image/ending/end_study.jpg";


    for (const auto& pair : end_img_paths) {
  
        end_bitmaps[pair.first] = IC->get(pair.second.c_str());
    }
};

void EndScreen::update()
{
    DataCenter *DC = DataCenter::get_instance();

    // 每次更新都先重置旗標
    restart_requested = false;

    // 【新增】檢查是否按下空白鍵 (ALLEGRO_KEY_SPACE)
    if (DC->key_state[ALLEGRO_KEY_SPACE] && !DC->prev_key_state[ALLEGRO_KEY_SPACE]) {
        debug_log("<EndScreen> Restart requested by Space key.\n");
        restart_requested = true;
    }
}

void EndScreen::draw()
{
    DataCenter *DC = DataCenter::get_instance();
    ALLEGRO_BITMAP *current_bitmap = nullptr;

    // 根據當前的結局類型 (current_end_type) 取得要繪製的圖片
    if (end_bitmaps.count(current_end_type)) {
        current_bitmap = end_bitmaps[current_end_type];
    }

    // 繪製結局畫面
    if (current_bitmap) {
        // 繪製圖片 (假設從 (0, 0) 開始繪製)
        al_draw_bitmap(current_bitmap, 0, 0, 0);
    } else {
        // 如果找不到圖片，繪製一個錯誤提示或純色背景
        al_clear_to_color(al_map_rgb(255, 0, 0)); // 紅色錯誤背景
        FontCenter *FC = FontCenter::get_instance();
        al_draw_text(
             FC->courier_new[FontSize::LARGE], al_map_rgb(255, 255, 255),
             DC->window_width / 2, DC->window_height / 2,
             ALLEGRO_ALIGN_CENTRE, "ERROR: Ending Image Not Found");
    }
    
    
}

// 由於 EndScreen 類在 Game 結束時才被銷毀，通常不需要複雜的析構函式
// 如果您在 init 中手動創建了新的 Allegro 資源，則需要在析構函式中銷毀。
// ~EndScreen() { ... }