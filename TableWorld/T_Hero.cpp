#include "T_Hero.h"
#include "../data/DataCenter.h"
#include "../data/GIFCenter.h"
#include "../data/ImageCenter.h"
#include "../shapes/Point.h"
#include "../shapes/Rectangle.h"
#include "../Utils.h"
#include "Attack/Nor_attack.h"
#include "Attack/Region_attack.h"
#include <allegro5/allegro_primitives.h>
#include "../data/FontCenter.h"
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <string>
using namespace std;

namespace CharacterSetting {
	static constexpr char character_imgs_root_path[40] = {
		"./assets/gif/Hero/dragonite_"
	};
	static constexpr char dir_path_postfix[][10] = {
		"left", "right", "front", "back"
	};
}

void THero::init()
{

	//載入gif路徑
	for(int i=0;i<static_cast<int>(THeroState::THeroState_MAX);i++)
	{
		gitfPath[static_cast<THeroState>(i)]=string(CharacterSetting::character_imgs_root_path)+string(CharacterSetting::dir_path_postfix[i])+".gif";
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
    
    hp_system.init(100);
    heal_cooldown_timer = 0;
    dash_timer = 0;
    dash_cooldown_timer = 0;
    is_dashing = false;
    invincible_timer = 0;
    stun_timer = 0;
    knockback_vx = 0;
    input_locked = false;
}

void THero::force_move(double dx) {
    if(dx > 0) state = THeroState::RIGHT;
    else if(dx < 0) state = THeroState::LEFT;
    
    shape->update_center_x(shape->center_x() + dx);
    
    // Apply gravity
    gravity.vy += gravity_acc;
    shape->update_center_y(shape->center_y() + gravity.vy);
}

void THero::hit(int damage, bool from_left) {
    if(invincible_timer > 0 || is_dashing) return;
    
    hp_system.take_damage(damage);
    invincible_timer = 60; // 1 second invincibility
    stun_timer = 15; // 0.25 second stun
    
    // Knockback
    knockback_vx = from_left ? 10 : -10;
    gravity.vy = -5; // Small hop
    gravity.on_ground = false;
}

void THero::update()
{
    DataCenter *DC=DataCenter::get_instance();
	//儲存舊位置
	float old_x=shape->center_x();
	float old_y=shape->center_y();

	//移動距離
	float dx=0, dy=0;

    // Update Timers
    if(heal_cooldown_timer > 0) heal_cooldown_timer--;
    if(dash_cooldown_timer > 0) dash_cooldown_timer--;
    if(invincible_timer > 0) invincible_timer--;
    if(stun_timer > 0) stun_timer--;
    
    // Knockback decay
    if(abs(knockback_vx) > 0.1) {
        knockback_vx *= 0.9;
        dx += knockback_vx;
    }

    if(stun_timer > 0) {
        // Stunned logic (cannot move or act)
        gravity.vy += gravity_acc;
        dy = gravity.vy;
        shape->update_center_x(old_x+dx);
        shape->update_center_y(old_y+dy);
        return;
    }
    
    if(is_dashing) {
        dash_timer--;
        if(dash_timer <= 0) {
            is_dashing = false;
        } else {
            // Dash movement (Force move forward)
            if(state == THeroState::LEFT) dx = -speed * 3;
            else dx = speed * 3;
            
            // Skip other inputs while dashing
            shape->update_center_x(old_x+dx);
            // Gravity still applies? Maybe not for dash
            // Let's say dash is horizontal only and defies gravity slightly or just moves fast
            // For simplicity, let gravity apply but horizontal speed is high
            gravity.vy += gravity_acc;
            dy = gravity.vy;
            shape->update_center_y(old_y+dy);
            return; 
        }
    }

	//重力作用
	gravity.vy += gravity_acc;

    if(input_locked) {
        dy = gravity.vy;
        shape->update_center_x(old_x+dx);
        shape->update_center_y(old_y+dy);
        return;
    }

    // 模式切換 (Q)
    if(DC->key_state[ALLEGRO_KEY_Q] && !DC->prev_key_state[ALLEGRO_KEY_Q])
    {
        int mode = static_cast<int>(current_mode);
        mode = (mode + 1) % static_cast<int>(HeroMode::HeroMode_MAX);
        current_mode = static_cast<HeroMode>(mode);
        printf("Switched to mode: %d\n", mode);
    }

    // 攻擊系統 (Z: 普通攻擊/技能, X: 範圍攻擊)
    if(DC->key_state[ALLEGRO_KEY_Z] && !DC->prev_key_state[ALLEGRO_KEY_Z])
    {
        if(current_mode == HeroMode::ATTACK)
        {
            printf("Normal Attack! (Mode: %d)\n", (int)current_mode);
            NorAttack *attack = new NorAttack();
            // 根據角色面向決定攻擊生成位置
            int attack_x = shape->center_x();
            int attack_y = shape->center_y();
            
            // 簡單的判斷：如果面向左，攻擊在左邊；面向右，攻擊在右邊
            // 這裡假設 THeroState::LEFT 和 THeroState::RIGHT
            bool is_left = (state == THeroState::LEFT);
            if(is_left) {
                attack_x -= w/2; // 調整位置
            } else {
                attack_x += w/2; // 調整位置
            }

            attack->init(attack_x, attack_y, is_left);
            DC->nor_attacks.push_back(attack);
        }
        else if(current_mode == HeroMode::HEALING)
        {
            if(heal_cooldown_timer <= 0) {
                printf("Healing! (Mode: %d)\n", (int)current_mode);
                int heal_amount = hp_system.get_max_hp() * 0.3;
                hp_system.heal(heal_amount);
                heal_cooldown_timer = 900; // 15 seconds * 60 FPS
            } else {
                printf("Healing Cooldown: %d\n", heal_cooldown_timer);
            }
        }
        else if(current_mode == HeroMode::DODGE)
        {
            if(!is_dashing && dash_cooldown_timer <= 0) {
                printf("Dash! (Mode: %d)\n", (int)current_mode);
                is_dashing = true;
                dash_timer = 20; // 1/3 second dash
                dash_cooldown_timer = 180; // 3 seconds cooldown
            }
        }
    }
    if(DC->key_state[ALLEGRO_KEY_X] && !DC->prev_key_state[ALLEGRO_KEY_X])
    {
        if(current_mode == HeroMode::ATTACK)
        {
            printf("Area Attack! (Mode: %d)\n", (int)current_mode);
            RegionAttack *attack = new RegionAttack();
            // 範圍攻擊以角色為中心
            int attack_x = shape->center_x();
            int attack_y = shape->center_y();
            
            // 範圍攻擊不需要區分左右，但 init 介面可能需要
            bool is_left = (state == THeroState::LEFT);
            
            attack->init(attack_x, attack_y, is_left);
            DC->region_attacks.push_back(attack);
        }
    }

	//鍵盤控制
	if(DC->key_state[ALLEGRO_KEY_LEFT])
	{
		state=THeroState::LEFT;
		dx=-speed;
		
	}
	if(DC->key_state[ALLEGRO_KEY_RIGHT])
	{
		state=THeroState::RIGHT;
		dx=speed;
	}
	if(DC->key_state[ALLEGRO_KEY_SPACE])
	{
		if(gravity.on_ground)
		{
            // Check for Drop Down (Down + Jump)
            // Use center_y() + h/2 because shape is a Shape* (base class) and doesn't have y2
            bool at_bottom = (shape->center_y() + h/2.0 >= DC->window_height - 10); 
            if(DC->key_state[ALLEGRO_KEY_DOWN] && !at_bottom) {
                gravity.on_ground = false;
                gravity.vy = 5.0; // Initial downward push
                drop_start_y = shape->center_y() + h/2.0; // Record starting foot position
            }
            else {
                // Normal Jump
			    gravity.vy = -jump_speed;
			    gravity.on_ground = false;
            }
		}
	}

	dy = gravity.vy;

	//更新位置
	shape->update_center_x(old_x+dx);
	shape->update_center_y(old_y+dy);
}
void THero::draw()
{
    // Flash if invincible
    if(invincible_timer > 0 && (invincible_timer / 5) % 2 == 0) {
        // Don't draw sprite to create flashing effect
    } else {
        GIFCenter *GC=GIFCenter::get_instance();
        ALGIF_ANIMATION *gif=GC->get(gitfPath[state]);
        algif_draw_gif(gif,
                       shape->center_x()-w/2,
                       shape->center_y()-h/2,0);
    }

    hp_system.draw_hero_health();
    
    // Draw Skill Status
    // Health bar ends at x=250, y=50-70
    int start_x = 260;
    int icon_y = 45;
    int icon_size = 30;
    int gap = 5;
    
    FontCenter *FC = FontCenter::get_instance();
    ALLEGRO_FONT *font = FC->caviar_dreams[FontSize::MEDIUM];
    
    // Draw 3 icons: Attack, Healing, Dodge
    for(int i=0; i<3; i++) {
        int icon_x = start_x + i * (icon_size + gap);
        HeroMode mode = static_cast<HeroMode>(i);
        
        // Background
        ALLEGRO_COLOR bg_color = al_map_rgb(200, 200, 200);
        if(mode == current_mode) {
            bg_color = al_map_rgb(255, 255, 200); // Highlight selected
        }
        al_draw_filled_rectangle(icon_x, icon_y, icon_x + icon_size, icon_y + icon_size, bg_color);
        
        // Border
        ALLEGRO_COLOR border_color = al_map_rgb(0, 0, 0);
        int border_thickness = 2;
        if(mode == current_mode) {
            border_color = al_map_rgb(255, 215, 0); // Gold border for selected
            border_thickness = 4;
        }
        al_draw_rectangle(icon_x, icon_y, icon_x + icon_size, icon_y + icon_size, border_color, border_thickness);
        
        // Text
        char mode_char = 'A';
        if(mode == HeroMode::HEALING) mode_char = 'H';
        else if(mode == HeroMode::DODGE) mode_char = 'D';
        
        char text[2] = {mode_char, '\0'};
        if(font) {
            al_draw_text(font, al_map_rgb(0, 0, 0), icon_x + icon_size/2, icon_y + 2, ALLEGRO_ALIGN_CENTER, text);
        }
        
        // Cooldown Overlay
        float cooldown_ratio = 0.0f;
        if(mode == HeroMode::HEALING) {
            cooldown_ratio = (float)heal_cooldown_timer / 900.0f;
        } else if(mode == HeroMode::DODGE) {
            if(is_dashing) cooldown_ratio = 1.0f;
            else cooldown_ratio = (float)dash_cooldown_timer / 180.0f;
        }
        
        if(cooldown_ratio > 0) {
            int h = (int)(icon_size * cooldown_ratio);
            al_set_blender(ALLEGRO_ADD, ALLEGRO_ALPHA, ALLEGRO_INVERSE_ALPHA);
            al_draw_filled_rectangle(icon_x, icon_y + icon_size - h, icon_x + icon_size, icon_y + icon_size, al_map_rgba(0, 0, 0, 150));
            al_set_blender(ALLEGRO_ADD, ALLEGRO_ONE, ALLEGRO_INVERSE_ALPHA);
        }
    }
}

// void THero::take_damage(int damage) {
//     HP -= damage;
//     if (HP < 0) HP = 0;
// }
