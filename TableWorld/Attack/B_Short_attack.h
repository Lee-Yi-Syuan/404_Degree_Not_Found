#ifndef B_SHORT_ATTACK_H
#define B_SHORT_ATTACK_H

#include "../../Object.h"
#include "../../shapes/Rectangle.h"

class BShortAttack : public Object {
public:
    void init(double x, double y, bool face_left);
    void update();
    void draw() override;
    bool is_alive() const { return alive; }

private:
    int stage = 0; // 0, 1, 2 for three slashes
    int timer = 0;
    int stage_duration = 25; 
    bool alive = true;
    bool facing_left = false;
    double start_x, start_y;
};

#endif