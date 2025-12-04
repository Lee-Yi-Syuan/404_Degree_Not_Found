#include "bed.h"
#include "../data/DataCenter.h"
#include "../Game.h"
#include "../data/ImageCenter.h"
#include "../data/FontCenter.h"
#include "../Character/Character.h"
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>

using namespace std;

namespace BedSetting {

    //床的圖片路徑
    static constexpr char bed_imgs_root_path[static_cast<int>(BedState::BedState_MAX)][40] = {
        "./assets/image/bed.png",
        "./assets/image/bed_touched.png"
    };
}

void Bed::init() {
    //載入床的圖片路徑
    for(int i=0;i<static_cast<int>(BedState::BedState_MAX);i++)
    {
        imgPath[static_cast<BedState>(i)]=string(BedSetting::bed_imgs_root_path[i]);
    }
    ImageCenter *IC = ImageCenter::get_instance();
    DataCenter *DC = DataCenter::get_instance();
    ALLEGRO_BITMAP *img = IC->get(imgPath[state]);

    //設定床的位置
    int wh=DC->window_height;
    int ww=DC->window_width;
    w=al_get_bitmap_width(img);
    h=al_get_bitmap_height(img);
    x1=ww - w - 100;
    y1=wh - h;

    //設定床的碰撞箱
    shape.reset(new Rectangle{
        x1,y1,
        x1 + w,
        y1 + h});

    //設定床的互動箱
    trigger_shape.reset(new Rectangle{
        x1-50,y1-50,
        x1 + w,
        y1 + h});
}

void Bed::draw() {
    ImageCenter *IC = ImageCenter::get_instance();
    ALLEGRO_BITMAP *img = IC->get(imgPath[state]);
    DataCenter *DC = DataCenter::get_instance();
    al_draw_bitmap(img,shape->center_x()-(w/2),shape->center_y()-(h/2) ,0);

    //如果處於要顯示對話框的狀態
    if(showing_prompt)
    {
        FontCenter *FC = FontCenter::get_instance();
        al_draw_text(
            FC->courier_new[FontSize::LARGE], al_map_rgb(255, 255, 255),
            DC->window_width/2-50, DC->window_height - 100,
            ALLEGRO_ALIGN_CENTRE, "Press E to get into the bed world.");
    }
}

//回傳角色是否碰到互動箱
bool Bed::is_player_touching() {
    DataCenter *DC = DataCenter::get_instance();
    Character* character=DC->character;
    
    return trigger_shape->overlap(*(character->shape));
}

//處理床的互動邏輯
void Bed::interact() {
    
    DataCenter *DC = DataCenter::get_instance();

    // 若角色碰到互動箱
    if (is_player_touching())
        state = BedState::touched;
    else {
        state = BedState::untouched;
    }

    // 若碰到且尚未進入床世界
    if(state==BedState::touched && !bed_world_loaded)
    {
        // 按下空白鍵時，顯示或關閉提示視窗並鎖定角色移動
        if(DC->key_state[ALLEGRO_KEY_SPACE] && !DC->prev_key_state[ALLEGRO_KEY_SPACE])
        {
            showing_prompt = !showing_prompt;
            DC->character->set_movability(!showing_prompt);
        }
        
        // 按下E鍵時，進入床世界
        if(showing_prompt&&DC->key_state[ALLEGRO_KEY_E] && !DC->prev_key_state[ALLEGRO_KEY_E])
        {
            bed_world_loaded = true;
            showing_prompt = false;
            DC->character->set_movability(true);
        }
        
    }
}


