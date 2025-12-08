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
    // Load GIF paths
	for(int i=0; i<static_cast<int>(BossDirection::BossDirection_MAX); i++)
	{
		gifPath[static_cast<BossDirection>(i)] = string(BossSetting::character_imgs_root_path) + string(BossSetting::dir_path_postfix[i]) + ".gif";
	}
	GIFCenter *GC = GIFCenter::get_instance();
	ALGIF_ANIMATION *gif = GC->get(gifPath[dir]);

	DataCenter *DC = DataCenter::get_instance();
	int wh = DC->window_height;
	int ww = DC->window_width;
    
	w = gif->width;
	h = gif->height;
    
    // Initial position (e.g., right side of screen)
	shape.reset(new Rectangle{
		ww - 200.0, wh / 2.0,
		ww - 200.0 + w,
		wh / 2.0 + h
    });
    
    state = BossState::IDLE;
    action_duration = 30; // 0.5 second
    action_timer = 0;
    
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
    if(!is_active) return;
    if(spawn_effect_timer > 0) spawn_effect_timer--;

    // AI Decision
    if(action_timer <= 0) {
        ai_decision();
    } else {
        action_timer--;
    }
    
    if(state == BossState::FLY) {
        fly_timer++;
    } else {
        fly_timer = 0;
    }

    // Movement Logic based on state
    float dx = 0, dy = 0;
    
    // Gravity
    if(state != BossState::FLY) {
        gravity.vy += gravity_acc;
    } else {
        // Flying logic (hover or move up/down)
        // For simplicity, let's just say FLY state defies gravity
        if(gravity.vy > 0) gravity.vy -= 0.2;
        if(gravity.vy < 0) gravity.vy += 0.2;
    }

    if(state == BossState::MOVE) {
        if(dir == BossDirection::LEFT) dx = -speed;
        else if(dir == BossDirection::RIGHT) dx = speed;
    } else if (state == BossState::ATTACK && current_attack == BossAttackType::SHORT) {
        // Move forward during Short Attack (Lunge)
        float lunge_speed = speed * 0.5; // Slower than run, but moving
        if(dir == BossDirection::LEFT) dx = -lunge_speed;
        else if(dir == BossDirection::RIGHT) dx = lunge_speed;
    } else if (state == BossState::FLY) {
        // Fly logic
        // If flying, try to stay above ground but not too high
        if(shape->center_y() > 300) dy = -fly_speed; // Fly up
        else if (shape->center_y() < 100) dy = fly_speed; // Fly down
        else dy = 0; // Hover
        
        // Move horizontally while flying to evade or chase
        // For now, just drift in current direction
        if(dir == BossDirection::LEFT) dx = -speed * 1.2;
        else if(dir == BossDirection::RIGHT) dx = speed * 1.2;
    }

    dy += gravity.vy;

    // Update Position
    shape->update_center_x(shape->center_x() + dx);
    shape->update_center_y(shape->center_y() + dy);
    
    // Boundary checks (simple)
    DataCenter *DC = DataCenter::get_instance();
    if(shape->center_x() < 0) shape->update_center_x(0 + w/2);
    if(shape->center_x() > DC->window_width) shape->update_center_x(DC->window_width - w/2);
}

void Boss::draw()
{
    if(!is_active) return;

	GIFCenter *GC = GIFCenter::get_instance();
	ALGIF_ANIMATION *gif = GC->get(gifPath[dir]);
    
    algif_draw_gif(gif, shape->center_x()-w/2, shape->center_y()-h/2, 0);
    
    // Draw black overlay (Shadow effect)
    al_set_blender(ALLEGRO_ADD, ALLEGRO_ALPHA, ALLEGRO_INVERSE_ALPHA);
    al_draw_filled_rectangle(shape->center_x()-w/2, shape->center_y()-h/2, shape->center_x()+w/2, shape->center_y()+h/2, al_map_rgba(0, 0, 0, 150));
    al_set_blender(ALLEGRO_ADD, ALLEGRO_ONE, ALLEGRO_INVERSE_ALPHA); // Restore blender

    hp_system.draw_boss_health();
    
    // Spawn Effect
    if(spawn_effect_timer > 0) {
        float ratio = (float)spawn_effect_timer / 60.0f;
        float radius = 100 * (1.0f - ratio) + 50; // Expand
        al_draw_circle(shape->center_x(), shape->center_y(), radius, al_map_rgb(255, 0, 0), 5 * ratio);
        al_draw_filled_circle(shape->center_x(), shape->center_y(), radius, al_map_rgba(255, 0, 0, 100 * ratio));
    }
}

// void Boss::take_damage(int damage) {
//     HP -= damage;
//     if (HP < 0) HP = 0;
// }

void Boss::ai_decision()
{
    DataCenter *DC = DataCenter::get_instance();
    THero *hero = DC->thero;
    
    if(!hero || !hero->hp_system.is_alive()) {
        state = BossState::IDLE;
        action_duration = 30;
        action_timer = action_duration;
        return;
    }

    double dist_x = hero->shape->center_x() - shape->center_x();
    double dist_y = hero->shape->center_y() - shape->center_y();
    double distance = sqrt(dist_x*dist_x + dist_y*dist_y);
    
    // Face the hero
    if(dist_x > 0) dir = BossDirection::RIGHT;
    else dir = BossDirection::LEFT;

    // If currently flying, check timer
    if(state == BossState::FLY) {
        if(fly_timer > 300) { // 5 seconds (60 FPS * 5)
            // Stop flying
            state = BossState::IDLE; // Fall down
            gravity.on_ground = false;
            fly_timer = 0;
            action_duration = 20;
            return;
        }
        
        // While flying, high frequency attacks
        // 50% chance to attack every decision tick (which is short if action_duration is low)
        int fly_attack_r = rand() % 100;
        if(fly_attack_r < 60) { // 60% chance to attack while flying
            state = BossState::ATTACK;
            if(rand() % 2 == 0) {
                current_attack = BossAttackType::NORMAL;
                action_duration = 60;
            } else {
                current_attack = BossAttackType::RANGE;
                action_duration = 180;
            }
            
            perform_attack();
        } else {
            // Just move/hover
            action_duration = 15;
        }
        return;
    }

    // Ground Logic
    if(distance < 250) {
        // Too close!
        int r = rand() % 100;
        if(r < 40) {
            // Move away
            state = BossState::MOVE;
            // Move in opposite direction of hero
            if(dist_x > 0) dir = BossDirection::LEFT; 
            else dir = BossDirection::RIGHT;
            action_duration = 20;
        } else if (r < 80) {
            // Short attack (defensive)
            state = BossState::ATTACK;
            current_attack = BossAttackType::SHORT;
            perform_attack();
            action_duration = 80;
        } else {
            // Fly up to evade
            state = BossState::FLY;
            gravity.vy = -12;
            gravity.on_ground = false;
            fly_timer = 0;
            action_duration = 25;
        }
    } else if (distance > 600) {
        // Too far, get closer
        state = BossState::MOVE;
        // Direction is already set to face hero
        action_duration = 30;
    } else {
        // Optimal distance (250-600)
        // 40% Nor, 40% Short, 10% Region, 10% Move/Idle
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
            // Adjust position slightly
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
