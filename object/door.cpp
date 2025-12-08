#include "door.h"
#include "../data/DataCenter.h"
#include "../Game.h"
#include "floor.h"
#include "../data/ImageCenter.h"
#include "../data/FontCenter.h"
#include "../Character/Character.h"
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>

using namespace std;

namespace DoorSetting {
    // door image paths
    static constexpr char door_imgs_root_path[static_cast<int>(DoorState::DoorState_MAX)][40] = {
        "./assets/image/door.png",
        "./assets/image/door_touched.png"
    };
}

void Door::init() {
    //載入門的圖片路徑
    for(int i=0;i<static_cast<int>(DoorState::DoorState_MAX);i++)
    {
        imgPath[static_cast<DoorState>(i)]=string(DoorSetting::door_imgs_root_path[i]);
    }
    ImageCenter *IC = ImageCenter::get_instance();
    DataCenter *DC = DataCenter::get_instance();
    ALLEGRO_BITMAP *img = IC->get(imgPath[state]);

    //設定門的位置
    int wh=DC->window_height;
    int ww=DC->window_width;
    w=al_get_bitmap_width(img);
    h=al_get_bitmap_height(img);

    //設定門的左上角(x,y)位置
    x1=DC->floor->get_x2();
    y1=wh/2-200;

    //設定門的碰撞箱
    shape.reset(new Rectangle{
        x1,y1,
        x1 + w,
        y1 + h});

    //設定門的互動箱
    trigger_shape.reset(new Rectangle{
        x1-25,y1-25,
        x1 + w,
        y1 + h});
}

void Door::draw() {
    ImageCenter *IC = ImageCenter::get_instance();
    ALLEGRO_BITMAP *img = IC->get(imgPath[state]);
    DataCenter *DC = DataCenter::get_instance();
    al_draw_bitmap(img,shape->center_x()-(w/2),shape->center_y()-(h/2) ,0);

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
            ALLEGRO_ALIGN_CENTRE, "Press E to get out of game.");
    }
}

//回傳角色是否碰到互動箱
bool Door::is_player_touching() {
    DataCenter *DC = DataCenter::get_instance();
    Character* character=DC->character;
    
    return trigger_shape->overlap(*(character->shape));
}

//處理門的互動邏輯
void Door::interact(bool enabled) {
    
    DataCenter *DC = DataCenter::get_instance();

    // 若角色碰到互動箱
    if (enabled && is_player_touching())
        state = DoorState::touched;
    else {
        state = DoorState::untouched;
        showing_prompt = false;
    }

    // 若碰到且尚未進入門世界
    if(state==DoorState::touched && !door_world_loaded)
    {
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
        
        // 按下E鍵時，進入門世界
        if(showing_prompt&&DC->key_state[ALLEGRO_KEY_E] && !DC->prev_key_state[ALLEGRO_KEY_E])
        {
            door_world_loaded = true;
            showing_prompt = false;
            DC->character->set_movability(true);
        }
        
    }
}


