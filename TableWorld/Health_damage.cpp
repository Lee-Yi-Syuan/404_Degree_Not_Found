#include "Health_damage.h"
#include "../data/DataCenter.h"
#include "T_Hero.h"
#include "Boss.h"
#include "Attack/Nor_attack.h"
#include "Attack/Region_attack.h"
#include "Attack/B_Nor_attack.h"
#include "Attack/B_Region_attack.h"
#include "Attack/B_Short_attack.h"
#include "../shapes/Rectangle.h"

// --- HealthComponent Implementation ---

void HealthComponent::init(int max_hp) {
    this->max_hp = max_hp;
    this->hp = max_hp;
}

void HealthComponent::take_damage(int damage) {
    hp -= damage;
    if (hp < 0) hp = 0;
}

void HealthComponent::heal(int amount) {
    hp += amount;
    if (hp > max_hp) hp = max_hp;
}

void HealthComponent::draw_hero_health() {
    // Draw Health Bar (Top Left)
    al_draw_filled_rectangle(50, 50, 250, 70, al_map_rgb(100, 100, 100)); // Background
    float hp_ratio = (float)hp / max_hp;
    if(hp_ratio < 0) hp_ratio = 0;
    al_draw_filled_rectangle(50, 50, 50 + 200 * hp_ratio, 70, al_map_rgb(255, 0, 0)); // HP
    al_draw_rectangle(50, 50, 250, 70, al_map_rgb(0, 0, 0), 2); // Border
}

void HealthComponent::draw_boss_health() {
    DataCenter *DC = DataCenter::get_instance();
    int bar_w = 600;
    int bar_h = 30;
    int bar_x = DC->window_width / 2 - bar_w / 2;
    int bar_y = 20;
    
    al_draw_filled_rectangle(bar_x, bar_y, bar_x + bar_w, bar_y + bar_h, al_map_rgb(100, 100, 100)); // Background
    float hp_ratio = (float)hp / max_hp;
    if(hp_ratio < 0) hp_ratio = 0;
    al_draw_filled_rectangle(bar_x, bar_y, bar_x + bar_w * hp_ratio, bar_y + bar_h, al_map_rgb(255, 0, 0)); // HP
    al_draw_rectangle(bar_x, bar_y, bar_x + bar_w, bar_y + bar_h, al_map_rgb(0, 0, 0), 2); // Border
}

// --- DamageSystem Implementation ---

void DamageSystem::update() {
    DataCenter *DC = DataCenter::get_instance();
    Boss* boss = DC->boss;
    THero* thero = DC->thero;

    if(!boss || !thero) return;
    
    // Only process damage if both are alive (or at least the victim is alive)
    bool boss_alive = boss->hp_system.is_alive();
    bool hero_alive = thero->hp_system.is_alive();

    // Collision: Hero Attacks vs Boss
    if(boss_alive) {
        for(auto attack : DC->nor_attacks) {
            if(attack->is_alive() && attack->shape->overlap(*(boss->shape))) {
                 boss->hp_system.take_damage(1); // Normal attack damage
            }
        }
        for(auto attack : DC->region_attacks) {
            if(attack->is_alive() && attack->shape->overlap(*(boss->shape))) {
                 boss->hp_system.take_damage(2); // Region attack damage
            }
        }
    }
    
    // Collision: Boss vs Hero (Body Damage)
    if(boss_alive && hero_alive) {
        // Check for immunity (Dash)
        if(thero->is_immune()) return;

        if(boss->shape->overlap(*(thero->shape))) {
            bool from_left = boss->shape->center_x() < thero->shape->center_x();
            thero->hit(1, from_left); // Contact damage
        }
        
        // Collision: Boss Attacks vs Hero
        for(auto attack : DC->boss_nor_attacks) {
            if(attack->is_alive() && attack->shape->overlap(*(thero->shape))) {
                 bool from_left = attack->shape->center_x() < thero->shape->center_x();
                 thero->hit(5, from_left); // Boss Normal attack damage
            }
        }
        for(auto attack : DC->boss_region_attacks) {
            if(attack->is_alive() && attack->shape->overlap(*(thero->shape))) {
                 bool from_left = attack->shape->center_x() < thero->shape->center_x();
                 thero->hit(2, from_left); // Boss Region attack damage (continuous)
            }
        }
        for(auto attack : DC->boss_short_attacks) {
            if(attack->is_alive() && attack->shape->overlap(*(thero->shape))) {
                 bool from_left = attack->shape->center_x() < thero->shape->center_x();
                 thero->hit(8, from_left); // Boss Short attack damage (High damage)
            }
        }
    }
}
