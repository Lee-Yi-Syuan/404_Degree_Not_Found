#ifndef Character_H_INCLUDED
#define Character_H_INCLUDED


#include "../Object.h"
#include "../shapes/Rectangle.h"
#include "../shapes/Point.h"
#include <allegro5/allegro.h>         
#include <allegro5/allegro_image.h>  
#include <vector>
#include <queue>
#include <map>

enum class EquippedItem {
    None,
    Hat,
    Clothes,
    Sunglasses
};

// fixed settings
enum class CharacterState {
	LEFT, RIGHT, Front, Back,CharacterState_MAX
};

/**
 * @brief The class of a character.
 * @details Character inherits Object and takes Rectangle as its hit box.
 */
class Character : public Object
{
	public:
		void init();
		void update();
		void draw() override;
		void set_movability(bool movable){this->movable=movable;}
        void toggle_equipped_item(EquippedItem item);
        bool is_equipped(EquippedItem item) const;
	private:
		CharacterState state=CharacterState::Front;
        bool has_hat = false;
        bool has_clothes = false;
        bool has_sunglasses = false;

		//移動速度
		double speed=5;
		//角色圖片路徑
		std::map<CharacterState,std::string> gitfPath;
		//角色寬高
		int w,h;
		//角色是否可移動
		bool movable=true;
		// 在 Character 類別的 private 或 protected 區塊

		//我這邊有改一下主角的長相
		private:
			ALLEGRO_BITMAP* move_img1 = nullptr;
			ALLEGRO_BITMAP* move_img2 = nullptr;
			int animation_tick = 0;      // 用於計時
			bool face_right = true;      // 記錄水平翻轉狀態
};

#endif
