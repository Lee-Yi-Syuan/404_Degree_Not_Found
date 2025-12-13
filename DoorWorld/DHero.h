#ifndef DHERO_H
#define DHERO_H

#include "../shapes/Rectangle.h"
#include <memory>
#include <allegro5/allegro.h>

struct Skill {
    bool active;
    bool on_cooldown;
    float cooldown;
    float duration;
    
    void reset() {
        active = false;
        on_cooldown = false;
        cooldown = 0;
        duration = 0;
    }
};

class DHero {
public:
    void init();
    void update();
    void draw();

    std::unique_ptr<Rectangle> shape;
    
    int hearts;
    float speed;
    float base_speed;
    float boost_speed;
    
    Skill heal;
    Skill shield;
    Skill speedup;
    

};

#endif