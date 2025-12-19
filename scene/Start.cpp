#include "Start.h"
#include "../data/DataCenter.h"
#include "../data/ImageCenter.h"
#include "../shapes/Rectangle.h"
#include "../data/FontCenter.h"
#include <allegro5/allegro_primitives.h>   
#include <allegro5/allegro_font.h>
#include <string>
#include <iostream> // 方便 Debug

using namespace std;

namespace StartSetting {
    static constexpr char start_imgs_root_path[static_cast<int>(StartState::StartState_MAX)][40] = {
        "./assets/image/StartBackground.jpg", 
        "./assets/image/start_special.jpg"
    };
   
    static constexpr char story_imgs_path[2][50] = {
        "./assets/image/story1.jpg", 
        "./assets/image/story2.jpg"
    };
}

StartScreen::StartScreen()
{
    start_pressed = false;
    sub_state = 0; 
}

void StartScreen::init()
{
    // 載入路徑
    for(int i=0; i<static_cast<int>(StartState::StartState_MAX); i++)
    {
        imgPath[static_cast<StartState>(i)] = string(StartSetting::start_imgs_root_path[i]);
    }
    
    // 載入劇情圖路徑
    storyPath[0] = string(StartSetting::story_imgs_path[0]);
    storyPath[1] = string(StartSetting::story_imgs_path[1]);

    start_pressed = false;
    sub_state = 0;
}

void StartScreen::reset()
{
    start_pressed = false;
    sub_state = 0;
}

void StartScreen::update()
{
    DataCenter *DC = DataCenter::get_instance();
    
    // 階段 0: 初始畫面
    if (sub_state == 0) {
        const Point &mouse = DC->mouse;
        int button_w = 260;
        int button_h = 70;
        int center_x = DC->window_width * 3 / 4;
        int center_y = DC->window_height * 3 / 4;
        int button_x = center_x - button_w / 2;
        int button_y = center_y - button_h / 2;

        if(mouse.overlap(Rectangle{
            (float)button_x, 
            (float)button_y, 
            (float)(button_x + button_w), 
            (float)(button_y + button_h)
        })) {
            if(DC->mouse_state[1] && !DC->prev_mouse_state[1]) {
                sub_state = 1; // 切換到劇情1
            }
        }
    } 
    // 階段 1: 劇情圖 1
    else if (sub_state == 1) {
        // 使用 prev_key_state 確保只偵測「按下那一瞬間」，防止連跳
        if (DC->key_state[ALLEGRO_KEY_SPACE] && !DC->prev_key_state[ALLEGRO_KEY_SPACE]) {
            sub_state = 2;
        }
    }
    // 階段 2: 劇情圖 2
    else if (sub_state == 2) {
        if (DC->key_state[ALLEGRO_KEY_SPACE] && !DC->prev_key_state[ALLEGRO_KEY_SPACE]) {
            start_pressed = true; // 這裡才真正讓外部 Game.cpp 切換到 Main
        }
    }
}

void StartScreen::draw()
{
    DataCenter *DC = DataCenter::get_instance();
    FontCenter *FC = FontCenter::get_instance();
    ImageCenter *IC = ImageCenter::get_instance();

    if (sub_state == 1) {
        // 畫劇情1
        ALLEGRO_BITMAP* b1 = IC->get(storyPath[0]);
        if(b1) al_draw_bitmap(b1, 0, 0, 0);
        
    }
    else if (sub_state == 2) {
        // 畫劇情2
        ALLEGRO_BITMAP* b2 = IC->get(storyPath[1]);
        if(b2) al_draw_bitmap(b2, 0, 0, 0);

       
    } 
    else {
        // 初始按鈕畫面
        al_draw_bitmap(IC->get(imgPath[state]), 0, 0, 0);

        int button_w = 260;
        int button_h = 70;
        int center_x = DC->window_width * 3 / 4;
        int center_y = DC->window_height * 3 / 4;
        int button_x = center_x - button_w / 2;
        int button_y = center_y - button_h / 2;

        bool hovering = (DC->mouse.x >= button_x && DC->mouse.x <= button_x + button_w &&
                         DC->mouse.y >= button_y && DC->mouse.y <= button_y + button_h);

        ALLEGRO_COLOR color_top    = hovering ? al_map_rgb(255, 255, 230) : al_map_rgb(255, 255, 200);
        ALLEGRO_COLOR color_bottom = hovering ? al_map_rgb(255, 235, 170) : al_map_rgb(245, 220, 150);

        for (int i = 0; i < button_h; i++) {
            float t = (float)i / button_h;
            ALLEGRO_COLOR blended = al_map_rgb(
                (1 - t) * (color_top.r * 255) + t * (color_bottom.r * 255),
                (1 - t) * (color_top.g * 255) + t * (color_bottom.g * 255),
                (1 - t) * (color_top.b * 255) + t * (color_bottom.b * 255)
            );
            al_draw_filled_rectangle(button_x, button_y + (float)i, button_x + button_w, button_y + i + 1, blended);
        }

        al_draw_rounded_rectangle(button_x, button_y, button_x + button_w, button_y + button_h, 15, 15, al_map_rgb(255, 255, 255), 3);
        al_draw_text(FC->courier_new[FontSize::LARGE], al_map_rgb(0,0,0), center_x, button_y + button_h / 2 - 15, ALLEGRO_ALIGN_CENTRE, "START");
    }
}