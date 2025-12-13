#include "Character.h"
#include "../object/bed.h"
#include "../object/closet.h"
#include "../object/table.h"
#include "../object/floor.h"
#include "../data/DataCenter.h"
#include "../data/GIFCenter.h"
#include "../data/ImageCenter.h"
#include "../shapes/Point.h"
#include "../shapes/Rectangle.h"
#include "../Utils.h"
#include <allegro5/allegro_primitives.h>

using namespace std;

namespace CharacterSetting {
	static constexpr char character_imgs_root_path[40] = {
		"./assets/gif/Hero/dragonite_"
	};
	static constexpr char dir_path_postfix[][10] = {
		"left", "right", "front", "back"
	};
}

void Character::init()
{

	//載入gif路徑
	for(int i=0;i<static_cast<int>(CharacterState::CharacterState_MAX);i++)
	{
		gitfPath[static_cast<CharacterState>(i)]=string(CharacterSetting::character_imgs_root_path)+string(CharacterSetting::dir_path_postfix[i])+".gif";
	}
	GIFCenter *GC=GIFCenter::get_instance();
	ALGIF_ANIMATION *gif=GC->get(gitfPath[state]);

	//設定角色初始位置
	DataCenter *DC=DataCenter::get_instance();
	int wh=DC->window_height;
	int ww=DC->window_width;
	//設定角色寬高
	w=gif->width;
	h=gif->height;
	//設定角色碰撞箱
	shape.reset(new Rectangle{
		ww /2.0 , wh / 2.0,
		ww / 2.0 + w,
		wh / 2.0 + h});
}
void Character::update()
{
	//移動
	if(!movable)	return;
	DataCenter *DC=DataCenter::get_instance();


	//儲存舊位置
	float old_x=shape->center_x();
	float old_y=shape->center_y();

	//移動距離
	float dx=0, dy=0;

	//鍵盤控制
	if(DC->key_state[ALLEGRO_KEY_LEFT])
	{
		state=CharacterState::LEFT;
		dx=-speed;
		
	}
	if(DC->key_state[ALLEGRO_KEY_RIGHT])
	{
		state=CharacterState::RIGHT;
		dx=speed;
	}
	if(DC->key_state[ALLEGRO_KEY_UP])
	{
		state=CharacterState::Back;
		dy=-speed;
	}
	if(DC->key_state[ALLEGRO_KEY_DOWN])
	{
		state=CharacterState::Front;
		dy=speed;
	}


	//更新位置
	shape->update_center_x(old_x+dx);
	shape->update_center_y(old_y+dy);


	//確認角色是否在地板區域內
	if(!DC->floor->is_on_floor(shape->center_x(),shape->center_y()))
	{
		//若否，移回原位置
		shape->update_center_x(old_x);
		shape->update_center_y(old_y);
	}

	//確認角色是否超出視窗範圍
	if(shape->center_x()<0)
		shape->update_center_x(old_x);
	if(shape->center_x()>DC->window_width)
		shape->update_center_x(old_x);
	if(shape->center_y()<0)
		shape->update_center_y(old_y);
	if(shape->center_y()>DC->window_height)
		shape->update_center_y(old_y);

	//確認角色是否與床發生碰撞
	if(shape->overlap(*(DC->bed->shape)))
	{
		//若是，移回原位置
		shape->update_center_x(old_x);
		shape->update_center_y(old_y);
	}

	//確認角色是否與衣櫃發生碰撞
	if(shape->overlap(*(DC->closet->shape)))
	{
		//若是，移回原位置
		shape->update_center_x(old_x);
		shape->update_center_y(old_y);
	}

	if(shape->overlap(*(DC->table->shape)))
	{
		//若是，移回原位置
		shape->update_center_x(old_x);
		shape->update_center_y(old_y);
	}
	
	
}
void Character::draw()
{
	GIFCenter *GC=GIFCenter::get_instance();
	ALGIF_ANIMATION *gif=GC->get(gitfPath[state]);
	algif_draw_gif(gif,
				   shape->center_x()-w/2,
				   shape->center_y()-h/2,0);

    // Draw equipped item
    float cx = shape->center_x();
    float cy = shape->center_y();
    float top = cy - h/2;
    
    if (has_hat) {
        // Draw a red hat
        al_draw_filled_triangle(cx - 15, top + 10, cx + 15, top + 10, cx, top - 15, al_map_rgb(255, 0, 0));
    }
    if (has_clothes) {
        // Draw a blue shirt (simple rectangle over body)
        al_draw_filled_rectangle(cx - 15, top + 30, cx + 15, top + 60, al_map_rgb(0, 0, 255));
    }
    if (has_sunglasses) {
        // Draw black sunglasses
        al_draw_filled_circle(cx - 5, top + 15, 4, al_map_rgb(0, 0, 0));
        al_draw_filled_circle(cx + 5, top + 15, 4, al_map_rgb(0, 0, 0));
        al_draw_line(cx - 5, top + 15, cx + 5, top + 15, al_map_rgb(0, 0, 0), 2);
    }
}

void Character::toggle_equipped_item(EquippedItem item) {
    switch(item) {
        case EquippedItem::Hat:
            has_hat = !has_hat;
            break;
        case EquippedItem::Clothes:
            has_clothes = !has_clothes;
            break;
        case EquippedItem::Sunglasses:
            has_sunglasses = !has_sunglasses;
            break;
        case EquippedItem::None:
            break;
    }
}

bool Character::is_equipped(EquippedItem item) const {
    switch(item) {
        case EquippedItem::Hat: return has_hat;
        case EquippedItem::Clothes: return has_clothes;
        case EquippedItem::Sunglasses: return has_sunglasses;
        default: return false;
    }
}
