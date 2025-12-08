#include "bed.h"
#include "../data/DataCenter.h"
#include "../Game.h"
#include "../data/ImageCenter.h"
#include "../data/FontCenter.h"
#include "../Character/Character.h"
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>

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
    
    // 旋轉後的寬高 (原本的寬變成高，高變成寬)
    int original_w = al_get_bitmap_width(img);
    int original_h = al_get_bitmap_height(img);
    w = original_h;
    h = original_w;
    
    // 設定位置在左下角 (Floor left: 190, Floor bottom: wh - 10)
    x1 = 190;
    y1 = (wh - 10) - h;

    //設定床的碰撞箱
    shape.reset(new Rectangle{
        x1,y1,
        x1 + w,
        y1 + h});

    //設定床的互動箱
    trigger_shape.reset(new Rectangle{
        x1-50,y1-50,
        x1 + w+50,
        y1 + h+50});
}

void Bed::draw() {
    ImageCenter *IC = ImageCenter::get_instance();
    ALLEGRO_BITMAP *img = IC->get(imgPath[state]);
    DataCenter *DC = DataCenter::get_instance();
    
    // 繪製旋轉後的圖片
    float cx = al_get_bitmap_width(img) / 2.0f;
    float cy = al_get_bitmap_height(img) / 2.0f;
    
    // 逆時針旋轉 90 度 (-PI/2)
    al_draw_rotated_bitmap(img, cx, cy, 
        shape->center_x(), shape->center_y(), 
        -ALLEGRO_PI / 2, 0);

    //如果處於要顯示對話框的狀態
    if(showing_prompt)
    {
        FontCenter *FC = FontCenter::get_instance();
        
        // Draw dialog box
        float cx = DC->window_width/2;
        float cy = DC->window_height - 100;
        float box_w = 800;
        float box_h = 100;
        
        al_draw_filled_rectangle(cx - box_w/2, cy - box_h/2, cx + box_w/2, cy + box_h/2, al_map_rgba(0, 0, 0, 200));
        al_draw_rectangle(cx - box_w/2, cy - box_h/2, cx + box_w/2, cy + box_h/2, al_map_rgb(255, 255, 255), 3);
        
        al_draw_text(
            FC->courier_new[FontSize::LARGE], al_map_rgb(255, 255, 255),
            cx, cy - 15,
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
void Bed::interact(bool enabled) {
    
    DataCenter *DC = DataCenter::get_instance();

    // 若角色碰到互動箱
    if (enabled && is_player_touching())
        state = BedState::touched;
    else {
        state = BedState::untouched;
        showing_prompt = false;
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
        
        // 按下ESC鍵時，關閉提示視窗並恢復角色移動
        if(showing_prompt && DC->key_state[ALLEGRO_KEY_ESCAPE] && !DC->prev_key_state[ALLEGRO_KEY_ESCAPE])
        {
            showing_prompt = false;
            DC->character->set_movability(true);
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


