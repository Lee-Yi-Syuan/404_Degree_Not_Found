#include "table.h"
#include "../data/DataCenter.h"
#include "../Game.h"
#include "../data/ImageCenter.h"
#include "../data/FontCenter.h"
#include "../Character/Character.h"
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>

using namespace std;

namespace TableSetting {

    //桌子的圖片路徑
    static constexpr char table_imgs_root_path[static_cast<int>(TableState::TableState_MAX)][40] = {
        "./assets/image/table.png",
        "./assets/image/table_touched.png"
    };
}

void Table::init() {
    //載入桌子的圖片路徑
    for(int i=0;i<static_cast<int>(TableState::TableState_MAX);i++)
    {
        imgPath[static_cast<TableState>(i)]=string(TableSetting::table_imgs_root_path[i]);
    }
    ImageCenter *IC = ImageCenter::get_instance();
    DataCenter *DC = DataCenter::get_instance();
    ALLEGRO_BITMAP *img = IC->get(imgPath[state]);

    //設定桌子的位置
    int wh=DC->window_height;
    int ww=DC->window_width;
    w=al_get_bitmap_width(img);
    h=al_get_bitmap_height(img);
    x1=130;
    y1=-1;

    //設定桌子的碰撞箱
    shape.reset(new Rectangle{
        x1,y1,
        x1 + w,
        y1 + h-100});

    //設定桌子的互動箱
    trigger_shape.reset(new Rectangle{
        x1-50,y1-50,
        x1 + w+50,
        y1 + h});
}

void Table::draw() {
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
            ALLEGRO_ALIGN_CENTRE, "Press E to get into the table world.");
    }
}

//回傳角色是否碰到互動箱
bool Table::is_player_touching() {
    DataCenter *DC = DataCenter::get_instance();
    Character* character=DC->character;
    
    return trigger_shape->overlap(*(character->shape));
}

//處理桌子的互動邏輯
void Table::interact(bool enabled) {
    
    DataCenter *DC = DataCenter::get_instance();

    // 若角色碰到互動箱
    if (enabled && is_player_touching())
        state = TableState::touched;
    else {
        state = TableState::untouched;
        showing_prompt = false;
    }

    // 若碰到且尚未進入桌子世界
    if(state==TableState::touched && !table_world_loaded)
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
        
        // 按下E鍵時，進入桌子世界
        if(showing_prompt&&DC->key_state[ALLEGRO_KEY_E] && !DC->prev_key_state[ALLEGRO_KEY_E])
        {
            table_world_loaded = true;
            showing_prompt = false;
            DC->character->set_movability(true);
        }
        
    }
}


