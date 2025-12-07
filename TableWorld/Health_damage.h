#ifndef HEALTH_DAMAGE_H
#define HEALTH_DAMAGE_H

#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>

class HealthComponent {
public:
    void init(int max_hp);
    void take_damage(int damage);
    void set_hp(int hp_value) { hp = hp_value; }
    void heal(int amount);
    int get_hp() const { return hp; }
    int get_max_hp() const { return max_hp; }
    bool is_alive() const { return hp > 0; }
    
    // Drawing functions
    void draw_hero_health();
    void draw_boss_health();

private:
    int hp;
    int max_hp;
};

class DamageSystem {
public:
    // Handles all damage interactions between Hero, Boss, and Attacks
    static void update();
};

#endif