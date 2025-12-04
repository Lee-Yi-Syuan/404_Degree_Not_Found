#include "closet.h"
#include "floor.h"
#include "../data/DataCenter.h"
#include "../Game.h"
#include "../data/ImageCenter.h"
#include "../data/FontCenter.h"
#include "../Character/Character.h"
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>

using namespace std;

namespace ClosetSetting {
    // closet image paths
    static constexpr char closet_imgs_root_path[static_cast<int>(ClosetState::ClosetState_MAX)][40] = {
        "./assets/image/closet.png",
        "./assets/image/closet_touched.png"
    };
}

void Closet::init() {
    //載入衣櫃的圖片路徑
    for(int i=0;i<static_cast<int>(ClosetState::ClosetState_MAX);i++)
    {
        imgPath[static_cast<ClosetState>(i)]=string(ClosetSetting::closet_imgs_root_path[i]);
    }
    ImageCenter *IC = ImageCenter::get_instance();
    DataCenter *DC = DataCenter::get_instance();
    ALLEGRO_BITMAP *img = IC->get(imgPath[state]);

    //設定衣櫃的位置
    int wh=DC->window_height;
    int ww=DC->window_width;
    w=al_get_bitmap_width(img);
    h=al_get_bitmap_height(img);

    //設定衣櫃的左上角(x,y)位置
    x1=DC->floor->get_x1()+250;
    y1=DC->floor->get_y1()-h+500;

    //設定衣櫃的碰撞箱
    shape.reset(new Rectangle{
        x1,y1,
        x1 + w-50,
        y1 + h-50});
    
    //設定衣櫃的互動箱
    trigger_shape.reset(new Rectangle{
        x1-50,y1,
        x1 + w+50,
        y1 + h+50});
}

void Closet::draw() {
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
            DC->window_width/2, DC->window_height - 100,
            ALLEGRO_ALIGN_CENTRE, "Press E to open the closet_world.");
    }
}

//回傳角色是否碰到互動箱
bool Closet::is_player_touching() {
    DataCenter *DC = DataCenter::get_instance();
    Character* character=DC->character;
 
    return trigger_shape->overlap(*(character->shape));
}

//處理衣櫃的互動邏輯
void Closet::interact() {
    
    DataCenter *DC = DataCenter::get_instance();

    // 是否接觸到衣櫃
    if (is_player_touching())
        state = ClosetState::touched;
    else {
        state = ClosetState::untouched;
    }

    // 若碰到且尚未進入衣櫃世界
    if(state==ClosetState::touched && !closet_world_loaded)
    {
        // 按下空白鍵時，顯示或關閉提示視窗並鎖定角色移動
        if(DC->key_state[ALLEGRO_KEY_SPACE] && !DC->prev_key_state[ALLEGRO_KEY_SPACE])
        {            
            showing_prompt = !showing_prompt;
            DC->character->set_movability(!showing_prompt);
        }
        // 按下E鍵時，進入衣櫃世界
        if(showing_prompt&&DC->key_state[ALLEGRO_KEY_E] && !DC->prev_key_state[ALLEGRO_KEY_E])
        {
            // 開啟衣櫃世界
            closet_world_loaded= true;
            showing_prompt = false;
            DC->character->set_movability(true);
        }
        
    }
}


