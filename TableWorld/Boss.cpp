#include "Boss.h"
#include "T_Hero.h"
#include "Attack/B_Nor_attack.h"
#include "Attack/B_Region_attack.h"
#include "Attack/B_Short_attack.h"
#include "../data/DataCenter.h"
#include "../data/GIFCenter.h"
#include "../data/ImageCenter.h"
#include "../shapes/Point.h"
#include "../shapes/Rectangle.h"
#include "../Utils.h"
#include <allegro5/allegro_primitives.h>
#include <cstdlib>

using namespace std;

namespace BossSetting {
	static constexpr char character_imgs_root_path[40] = {
		"./assets/gif/Hero/dragonite_" // Reuse hero for now
	};
	static constexpr char dir_path_postfix[][10] = {
		"left", "right", "front", "back"
	};
}

void Boss::init()
{
    
    ImageCenter *IC = ImageCenter::get_instance();
    imgPath = "./assets/gif/Hero/calculus.jpg";
	ALLEGRO_BITMAP *img = IC->get(imgPath);

	DataCenter *DC = DataCenter::get_instance();
	int wh = DC->window_height;
	int ww = DC->window_width;
    
	w = al_get_bitmap_width(img);
	h = al_get_bitmap_height(img);
    
    // 初始化位置在畫面右半邊中央
    // 使用圖片大小來計算置中位置
	shape.reset(new Rectangle{
		ww * 0.75 - w / 2.0, wh / 2.0 - h / 2.0,
		ww * 0.75 + w / 2.0, wh / 2.0 + h / 2.0
    });
    
    // 初始狀態
    state = BossState::IDLE;
    action_duration = 30; // 0.5 second
    action_timer = 0;
    
    // 初始化血量系統
    hp_system.init(500);
    is_active = false; // Default hidden
    spawn_effect_timer = 0;
}

void Boss::spawn() {
    is_active = true;
    spawn_effect_timer = 60; // 1 second effect
    state = BossState::IDLE;
    // Reset position if needed, or keep init position
}

void Boss::update()
{
    //若還沒進到boss活動狀態則不更新
    if(!is_active) return;

    if(spawn_effect_timer > 0) spawn_effect_timer--;

    // 若到了行動抉擇時間，進入抉擇函數
    if(action_timer <= 0) {
        ai_decision();
    } 
    else   //反之行動計時器減少 
    {
        action_timer--;
    }
    
    //若處於飛行狀態則飛行計時器增加
    if(state == BossState::FLY) {
        fly_timer++;
    } else {
        fly_timer = 0;
    }

    //移動量初始化
    float dx = 0, dy = 0;
    
    // 若沒有處於飛行狀態則垂直加速度增加
    if(state != BossState::FLY) {
        gravity.vy += gravity_acc;
    } 
    else
    {
        // 微調垂直加速度以達到穩定飛行效果
        if(gravity.vy > 0) gravity.vy -= 0.2;
        if(gravity.vy < 0) gravity.vy += 0.2;

        // 若高度過高或過低，則調整垂直移動量
        if(shape->center_y() > 300) dy = -fly_speed;
        else if (shape->center_y() < 100) dy = fly_speed;
        else dy = 0; //若在範圍中間則不調整高度
        
        // 水平移動，模擬漂浮
        if(dir == BossDirection::LEFT) dx = -speed * 1.2;
        else if(dir == BossDirection::RIGHT) dx = speed * 1.2;
    }

    //如果處於移動模式則依方向移動
    if(state == BossState::MOVE) 
    {
        if(dir == BossDirection::LEFT) dx = -speed;
        else if(dir == BossDirection::RIGHT) dx = speed;
    } 
    //若處於攻擊模式則根據攻擊類型決定行動
    else if (state == BossState::ATTACK && current_attack == BossAttackType::SHORT) 
    {
        // 在短距離攻擊模式下，boss邊向前邊揮刀
        float lunge_speed = speed * 0.5; //揮刀的移動較慢
        //由面對方向決定移動方向
        if(dir == BossDirection::LEFT) dx = -lunge_speed;
        else if(dir == BossDirection::RIGHT) dx = lunge_speed;
    } 

    //最後依照上面得出的垂直加速度更新垂直移動量
    dy += gravity.vy;

    //再由移動量更新位置
    shape->update_center_x(shape->center_x() + dx);
    shape->update_center_y(shape->center_y() + dy);
    
    // 邊界測試
    DataCenter *DC = DataCenter::get_instance();
    if(shape->center_x() < 0) shape->update_center_x(0 + w/2);
    if(shape->center_x() > DC->window_width) shape->update_center_x(DC->window_width - w/2);
}

void Boss::draw()
{
    //若還沒進到boss活動狀態則不繪製
    if(!is_active) return;

	ImageCenter *IC = ImageCenter::get_instance();
	ALLEGRO_BITMAP *img = IC->get(imgPath);
    
    // GIF 原圖面向左，若 Boss 面向右則翻轉
    int flags = 0;
    if (dir == BossDirection::RIGHT) {
        flags = ALLEGRO_FLIP_HORIZONTAL;
    }
    
    al_draw_bitmap(img, shape->center_x()-w/2, shape->center_y()-h/2, flags);

    hp_system.draw_boss_health();
    
    //畫Boss出場特效
    if(spawn_effect_timer > 0) {
        //ratio跟spawn_effect_timer成正比,代表逐漸變小(剩餘面積)
        float ratio = (float)spawn_effect_timer / 60.0f;
        //radius與ratio成反比，代表逐漸變大
        float radius = 100 * (1.0f - ratio) + 50; 
        //這邊畫的是一個紅色的圓圈，代表出場特效，會隨著剩餘時間越小而變大變淡
        al_draw_circle(shape->center_x(), shape->center_y(), radius, al_map_rgb(255, 0, 0), 5 * ratio);
        //這邊畫得是一個填滿的圓圈，代表出場特效，會隨著剩餘時間越小而變大變淡
        al_draw_filled_circle(shape->center_x(), shape->center_y(), radius, al_map_rgba(255, 0, 0, 100 * ratio));
    }
}

//boss的決策系統
void Boss::ai_decision()
{
    DataCenter *DC = DataCenter::get_instance();
    THero *hero = DC->thero;
    
    //若主角不存在或已死亡則進入待機狀態
    if(!hero || !hero->hp_system.is_alive()) {
        state = BossState::IDLE;
        action_duration = 30;
        action_timer = action_duration;
        return;
    }

    //計算與主角的距離
    double dist_x = hero->shape->center_x() - shape->center_x();
    double dist_y = hero->shape->center_y() - shape->center_y();
    double distance = sqrt(dist_x*dist_x + dist_y*dist_y);
    
    // boss會根據主角位置調整面向方向
    if(dist_x > 0) dir = BossDirection::RIGHT;
    else dir = BossDirection::LEFT;

    // 如果處於飛行狀態，則優先處理飛行邏輯
    if(state == BossState::FLY) {
        if(fly_timer > 300) // 超過5秒則停止飛行
        { 
            state = BossState::IDLE;    //先將狀態設為待機
            gravity.on_ground = false;  //由於可能會在天上降落，故設為false
            fly_timer = 0;              //重置飛行計時器
            action_duration = 20;       //短暫的待機時間讓boss落地
            return;
        }
        
        // 當在飛行時，攻擊機率提高
        int fly_attack_r = rand() % 100;
        if(fly_attack_r < 60)  // 60% 機率攻擊
        { 
            state = BossState::ATTACK;
            //50% 普攻，50% 範圍攻擊
            if(rand() % 2 == 0) {
                current_attack = BossAttackType::NORMAL;
                action_duration = 60;
            } else {
                current_attack = BossAttackType::RANGE;
                action_duration = 180;
            }
            
            perform_attack();
        } else {
            // 否則繼續飛行
            action_duration = 15;
        }
        return;
    }

    // 若不在飛行模式，根據與主角距離決定行動
    if(distance < 250) {   //若距離小於250，表示太近
        int r = rand() % 100;
        if(r < 40) {
            // Boss進入到逃跑模式
            state = BossState::MOVE;
            // 往遠離主角方向移動
            if(dist_x > 0) dir = BossDirection::LEFT; 
            else dir = BossDirection::RIGHT;
            action_duration = 20;
        } else if (r < 80) {
            // 短距離攻擊 (防禦性攻擊)
            state = BossState::ATTACK;
            current_attack = BossAttackType::SHORT;
            perform_attack();
            action_duration = 80;
        } else {
            // 嘗試飛行以拉開距離
            state = BossState::FLY;
            gravity.vy = -12;
            gravity.on_ground = false;
            fly_timer = 0;
            action_duration = 25;
        }
    } else if (distance > 600) {
        // 若距離大於600，表示太遠
        state = BossState::MOVE;
        action_duration = 30;
    } else {
        // 適中距離則隨機選擇攻擊或移動
        // 40% 普攻, 40% 短距離攻擊, 10% 範圍攻擊, 10% 移動/待機
        int attack_r = rand() % 100;
        
        if(attack_r < 40) {
            state = BossState::ATTACK;
            current_attack = BossAttackType::NORMAL;
            perform_attack();
            action_duration = 60;
        } else if (attack_r < 80) {
            state = BossState::ATTACK;
            current_attack = BossAttackType::SHORT;
            perform_attack();
            action_duration = 80; // 25 * 3 = 75, + buffer
        } else if (attack_r < 90) {
            state = BossState::ATTACK;
            current_attack = BossAttackType::RANGE;
            perform_attack();
            action_duration = 180; // 60 * 3 = 180
        } else {
            // 微調位置
            state = BossState::MOVE;
            if(rand() % 2 == 0) dir = (dir == BossDirection::LEFT) ? BossDirection::RIGHT : BossDirection::LEFT;
            action_duration = 30;
        }
    }
    
    action_timer = action_duration;
}

void Boss::perform_attack()
{
    DataCenter *DC = DataCenter::get_instance();
    THero *hero = DC->thero;
    
    if(current_attack == BossAttackType::NORMAL) {
        BNorAttack *attack = new BNorAttack();
        attack->init(shape->center_x(), shape->center_y(), hero->shape->center_x(), hero->shape->center_y());
        DC->boss_nor_attacks.push_back(attack);
    } else if (current_attack == BossAttackType::RANGE) {
        BRegionAttack *attack = new BRegionAttack();
        attack->init(shape->center_x(), shape->center_y());
        DC->boss_region_attacks.push_back(attack);
    } else if (current_attack == BossAttackType::SHORT) {
        BShortAttack *attack = new BShortAttack();
        bool face_left = (dir == BossDirection::LEFT);
        attack->init(shape->center_x(), shape->center_y(), face_left);
        DC->boss_short_attacks.push_back(attack);
    } else if (current_attack == BossAttackType::SPECIAL) {
        // Implement special attack later or reuse one of the above
        printf("Boss performs SPECIAL attack (Not implemented yet)\n");
    }
}
