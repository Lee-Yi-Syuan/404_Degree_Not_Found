#include "Start.h"
#include "../data/DataCenter.h"
#include "../data/ImageCenter.h"
#include "../shapes/Rectangle.h"
#include "../data/FontCenter.h"
#include <allegro5/allegro_primitives.h>   
#include <allegro5/allegro_font.h>
#include <string>
using namespace std;

namespace StartSetting {

    
    static constexpr char start_imgs_root_path[static_cast<int>(StartState::StartState_MAX)][40] = {
        "./assets/image/StartBackground.jpg", // 有改初始畫面
        "./assets/image/start_special.jpg"
    };
}
StartScreen::StartScreen()
{
    start_pressed = false;
};

void StartScreen::init()
{
    //載入初始畫面的圖片路徑
    for(int i=0;i<static_cast<int>(StartState::StartState_MAX);i++)
    {
        imgPath[static_cast<StartState>(i)]=string(StartSetting::start_imgs_root_path[i]);
    }

    start_pressed = false;
};

void StartScreen::reset()
{
    start_pressed = false;
};

void StartScreen::update()
{
    DataCenter *DC = DataCenter::get_instance();
    const Point &mouse = DC->mouse;

    // check if start button is pressed
    int button_w = 260;
    int button_h = 70;

    
    int center_x = DC->window_width * 3 / 4;
    int center_y = DC->window_height * 3 / 4;
    int button_x = center_x - button_w / 2;
    int button_y = center_y - button_h / 2;
    // --------------------------------------------------------

    if(mouse.overlap(Rectangle{button_x, button_y, button_x + button_w, button_y + button_h})) {
        // click mouse left button
        if(DC->mouse_state[1] && !DC->prev_mouse_state[1]) {
            start_pressed = true;
        }
    }
}

void StartScreen::draw()
{
    DataCenter *DC = DataCenter::get_instance();
    FontCenter *FC = FontCenter::get_instance();
    ImageCenter *IC = ImageCenter::get_instance();

    al_draw_bitmap(IC->get(imgPath[state]), 0, 0, 0);

    // draw start button (pretty version)有改按鍵的位置
    int button_w = 260;
    int button_h = 70;

    int center_x = DC->window_width * 3 / 4;
    int center_y = DC->window_height * 3 / 4;
    int button_x = center_x - button_w / 2;
    int button_y = center_y - button_h / 2;

    // hover effect
    bool hovering =
        (DC->mouse.x >= button_x &&
        DC->mouse.x <= button_x + button_w &&
        DC->mouse.y >= button_y &&
        DC->mouse.y <= button_y + button_h);

    // button colors有改成黃色
    ALLEGRO_COLOR color_top    = hovering ? al_map_rgb(255, 255, 230) : al_map_rgb(255, 255, 200);
    ALLEGRO_COLOR color_bottom = hovering ? al_map_rgb(255, 235, 170) : al_map_rgb(245, 220, 150);

    // gradient background
    for (int i = 0; i < button_h; i++) 
    {
        float t = (float)i / button_h;
        ALLEGRO_COLOR blended = al_map_rgb(
            (1 - t) * (color_top.r * 255)    + t * (color_bottom.r * 255),
            (1 - t) * (color_top.g * 255)    + t * (color_bottom.g * 255),
            (1 - t) * (color_top.b * 255)    + t * (color_bottom.b * 255)
        );
        al_draw_filled_rectangle(
            button_x, button_y + i,
            button_x + button_w, button_y + i + 1,
            blended
        );
    }

    // rounded border
    al_draw_rounded_rectangle(
        button_x, button_y,
        button_x + button_w, button_y + button_h,
        15, 15,
        al_map_rgb(255, 255, 255),
        3
    );

    // START text (slightly lower for better aesthetics)也有改
    al_draw_text(
        FC->courier_new[FontSize::LARGE], al_map_rgb(0,0,0),
        DC->window_width * 3 / 4, 
        button_y + button_h / 2 - 15,
        ALLEGRO_ALIGN_CENTRE, "START"
    );
}


