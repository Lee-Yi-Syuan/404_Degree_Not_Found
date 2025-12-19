#ifndef Character_H_INCLUDED
#define Character_H_INCLUDED

#include "../Object.h"
#include "../shapes/Rectangle.h"
#include "../shapes/Point.h"
#include "../algif5/algif.h"
#include <allegro5/allegro.h>         
#include <allegro5/allegro_image.h>  
#include <vector>
#include <string>
#include <map>

enum class EquippedItem {
    None,
    Hat,
    Clothes, // 程式邏輯中對應 wind.png
    Sunglasses
};

enum class CharacterState {
    LEFT, RIGHT, Front, Back, CharacterState_MAX
};

class Character : public Object
{
    public:
        void init();
        void update();
        void draw() override;
        void set_movability(bool movable){this->movable = movable;}
        void toggle_equipped_item(EquippedItem item);
        bool is_equipped(EquippedItem item) const;

    private:
        // 狀態標記
        CharacterState state = CharacterState::Front;
        bool has_hat = false;
        bool has_clothes = false;
        bool has_sunglasses = false;

        // 數值設定
        double speed = 5;
        int w, h; // 這裡我們會設為 100, 100
        bool movable = true;

        // 主角動畫相關
        ALGIF_ANIMATION* gif_status = nullptr;
        int animation_tick = 0;
        bool face_right = true;

        // --- 新增：裝備圖片指標 ---
        // 使用 ALLEGRO_BITMAP 來儲存載入後的 PNG
        ALLEGRO_BITMAP* hat_img = nullptr;
        ALLEGRO_BITMAP* glasses_img = nullptr;
        ALLEGRO_BITMAP* wind_img = nullptr;

		
};

#endif