#ifndef B_NOR_ATTACK_H
#define B_NOR_ATTACK_H

#include "../../Object.h"
#include "../../shapes/Rectangle.h"
#include <allegro5/allegro_image.h>
#include <allegro5/allegro.h>

class BNorAttack : public Object {
public:
    void init(double x, double y, double target_x, double target_y);
    void update();
    void draw() override;
    bool is_alive() const { return alive; }

private:
    ALLEGRO_BITMAP *bullet_img = nullptr;
    double vx, vy;
    double speed = 7.0;
    int life_counter = 0;
    int max_life = 120; // 2 seconds
    bool alive = true;
    double angle = 0;
};

#endif