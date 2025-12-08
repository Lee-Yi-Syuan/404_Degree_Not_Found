#include "closet.h"
#include "floor.h"
#include "../data/DataCenter.h"
#include "../Game.h"
#include "../data/ImageCenter.h"
#include "../data/FontCenter.h"
#include "../Character/Character.h"
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>

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
    x1=ww - w - 100;
    y1=30;

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
            ALLEGRO_ALIGN_CENTRE, "Press E to open the closet.");
    }

    // Show Menu
    if (showing_menu) {
        FontCenter *FC = FontCenter::get_instance();
        float cx = DC->window_width/2;
        float cy = DC->window_height/2;
        float box_w = 400;
        float box_h = 300;
        
        al_draw_filled_rectangle(cx - box_w/2, cy - box_h/2, cx + box_w/2, cy + box_h/2, al_map_rgba(0, 0, 0, 220));
        al_draw_rectangle(cx - box_w/2, cy - box_h/2, cx + box_w/2, cy + box_h/2, al_map_rgb(255, 255, 255), 3);
        
        std::string hat_str = "1. Hat " + std::string(DC->character->is_equipped(EquippedItem::Hat) ? "[x]" : "[ ]");
        std::string clothes_str = "2. Clothes " + std::string(DC->character->is_equipped(EquippedItem::Clothes) ? "[x]" : "[ ]");
        std::string glasses_str = "3. Sunglasses " + std::string(DC->character->is_equipped(EquippedItem::Sunglasses) ? "[x]" : "[ ]");

        al_draw_text(FC->courier_new[FontSize::LARGE], al_map_rgb(255, 255, 255), cx, cy - 100, ALLEGRO_ALIGN_CENTRE, "Choose Items:");
        al_draw_text(FC->courier_new[FontSize::MEDIUM], al_map_rgb(255, 255, 255), cx, cy - 40, ALLEGRO_ALIGN_CENTRE, hat_str.c_str());
        al_draw_text(FC->courier_new[FontSize::MEDIUM], al_map_rgb(255, 255, 255), cx, cy + 0, ALLEGRO_ALIGN_CENTRE, clothes_str.c_str());
        al_draw_text(FC->courier_new[FontSize::MEDIUM], al_map_rgb(255, 255, 255), cx, cy + 40, ALLEGRO_ALIGN_CENTRE, glasses_str.c_str());
        al_draw_text(FC->courier_new[FontSize::MEDIUM], al_map_rgb(200, 200, 200), cx, cy + 100, ALLEGRO_ALIGN_CENTRE, "Press ESC to close");
    }
}

//回傳角色是否碰到互動箱
bool Closet::is_player_touching() {
    DataCenter *DC = DataCenter::get_instance();
    Character* character=DC->character;
 
    return trigger_shape->overlap(*(character->shape));
}

//處理衣櫃的互動邏輯
void Closet::interact(bool enabled) {
    
    DataCenter *DC = DataCenter::get_instance();

    // If menu is open, handle menu input
    if (showing_menu) {
        DC->character->set_movability(false);
        
        if (DC->key_state[ALLEGRO_KEY_ESCAPE] && !DC->prev_key_state[ALLEGRO_KEY_ESCAPE]) {
            showing_menu = false;
            DC->character->set_movability(true);
        }
        
        if (DC->key_state[ALLEGRO_KEY_1] && !DC->prev_key_state[ALLEGRO_KEY_1]) {
            DC->character->toggle_equipped_item(EquippedItem::Hat);
        }
        if (DC->key_state[ALLEGRO_KEY_2] && !DC->prev_key_state[ALLEGRO_KEY_2]) {
            DC->character->toggle_equipped_item(EquippedItem::Clothes);
        }
        if (DC->key_state[ALLEGRO_KEY_3] && !DC->prev_key_state[ALLEGRO_KEY_3]) {
            DC->character->toggle_equipped_item(EquippedItem::Sunglasses);
        }
        return;
    }

    // 若角色碰到互動箱
    if (enabled && is_player_touching())
        state = ClosetState::touched;
    else {
        state = ClosetState::untouched;
        showing_prompt = false;
    }

    // 若碰到且尚未進入衣櫃世界
    if(state==ClosetState::touched)
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

        // 按下E鍵時，進入衣櫃世界
        if(showing_prompt&&DC->key_state[ALLEGRO_KEY_E] && !DC->prev_key_state[ALLEGRO_KEY_E])
        {
            showing_prompt = false;
            showing_menu = true;
        }
        
    }
}


