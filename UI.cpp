#include "UI.h"
#include "Utils.h"
#include "data/DataCenter.h"
#include "character/Character.h"
#include "data/ImageCenter.h"
#include "data/FontCenter.h"
#include <algorithm>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_ttf.h>
#include "shapes/Point.h"
#include "shapes/Rectangle.h"
#include "Player.h"
//#include "towers/Tower.h"

void
UI::init() {

    debug_log("<UI> UI Initialized for new mode.\n"); 
}

void
UI::update() {
    if (showing_result) {
        interact_result_dialog();
    }
}

void UI::draw() {
    DataCenter *DC = DataCenter::get_instance();
    FontCenter *FC = FontCenter::get_instance();

    const int now_point = DC->player->now_point;
    const int now_fail = DC->player->now_fail;
    const int now_club_point = DC->player->now_club_point;
    const int now_love_point = DC->player->now_love_point;
    const int now_study_point = DC->player->now_study_point;

    // 設定顏色
    ALLEGRO_COLOR main_color = al_map_rgb(0, 0, 0);       // 黑色主文字
    ALLEGRO_COLOR outline_color = al_map_rgb(255, 255, 255); // 白色外框
    
    const int top_left_x = 20; // 稍微增加邊距，避免太貼邊
    const int top_y_start = 20;
    const int line_spacing = 35; 
    const int top_right_x = DC->window_width - 20;
    
    ALLEGRO_FONT* font = FC->courier_new[FontSize::MEDIUM];

    // --- 定義一個小 lambda 函數來簡化「畫外框字」的重複動作 ---
    auto draw_text_with_outline = [&](float x, float y, int align, const char* format, int value1, int value2 = -1) {
        char buf[100];
        if (value2 == -1) sprintf(buf, format, value1);
        else sprintf(buf, format, value1, value2);

        // 畫白色外框 (上下左右位移 2 像素)
        al_draw_text(font, outline_color, x + 2, y, align, buf);
        al_draw_text(font, outline_color, x - 2, y, align, buf);
        al_draw_text(font, outline_color, x, y + 2, align, buf);
        al_draw_text(font, outline_color, x, y - 2, align, buf);
        // 畫黑色主體
        al_draw_text(font, main_color, x, y, align, buf);
    };

    // --- 左上角：總進度 ---
    
    // 總學分 (Total Credits)
    draw_text_with_outline(top_left_x, top_y_start, ALLEGRO_ALIGN_LEFT, 
                           "Total Credits: %d / 128", now_point);

    // 不及格科目 (Failed Courses)
    draw_text_with_outline(top_left_x, top_y_start + line_spacing, ALLEGRO_ALIGN_LEFT, 
                           "Failed Courses: %d / 3", now_fail);

    // --- 右上角：各項分數 ---

    // 課業學分 (Study)
    draw_text_with_outline(top_right_x, top_y_start, ALLEGRO_ALIGN_RIGHT, 
                           "Study Credits: %d", now_study_point);

    // 戀愛學分 (Love)
    draw_text_with_outline(top_right_x, top_y_start + 1 * line_spacing, ALLEGRO_ALIGN_RIGHT, 
                           "Love Credits: %d", now_love_point);

    // 社團學分 (Club)
    draw_text_with_outline(top_right_x, top_y_start + 2 * line_spacing, ALLEGRO_ALIGN_RIGHT, 
                           "Club Credits: %d", now_club_point);

    // 原本的結果對話框邏輯
    if (showing_result) {
        draw_result_dialog();
    }
}

void UI::show_result_dialog(std::string message) {
    result_msg = message;
    showing_result = true;
}

void UI::interact_result_dialog() {
    DataCenter *DC = DataCenter::get_instance();
    
    // Stop player movement while dialog is open
    DC->character->set_movability(false);

    if ((DC->key_state[ALLEGRO_KEY_SPACE] && !DC->prev_key_state[ALLEGRO_KEY_SPACE]) ||
        (DC->key_state[ALLEGRO_KEY_ESCAPE] && !DC->prev_key_state[ALLEGRO_KEY_ESCAPE]) ||
        (DC->key_state[ALLEGRO_KEY_ENTER] && !DC->prev_key_state[ALLEGRO_KEY_ENTER])) {
        showing_result = false;
        DC->character->set_movability(true);
    }
}

void UI::draw_result_dialog() {
    DataCenter *DC = DataCenter::get_instance();
    FontCenter *FC = FontCenter::get_instance();
    
    float cx = DC->window_width / 2;
    float cy = DC->window_height / 2;
    float box_w = 600;
    float box_h = 200;
    
    al_draw_filled_rectangle(cx - box_w/2, cy - box_h/2, cx + box_w/2, cy + box_h/2, al_map_rgba(0, 0, 0, 220));
    al_draw_rectangle(cx - box_w/2, cy - box_h/2, cx + box_w/2, cy + box_h/2, al_map_rgb(255, 215, 0), 4); // Gold border
    
    al_draw_text(
        FC->courier_new[FontSize::LARGE], al_map_rgb(255, 255, 255),
        cx, cy - 20,
        ALLEGRO_ALIGN_CENTRE, result_msg.c_str());
        
    al_draw_text(
        FC->courier_new[FontSize::MEDIUM], al_map_rgb(200, 200, 200),
        cx, cy + 40,
        ALLEGRO_ALIGN_CENTRE, "Press SPACE to continue");
}



