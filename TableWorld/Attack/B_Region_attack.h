#ifndef B_REGION_ATTACK_H
#define B_REGION_ATTACK_H

#include "../../Object.h"
#include "../../shapes/Circle.h"

class BRegionAttack : public Object {
public:
    void init(double x, double y);
    void update();
    void draw() override;
    bool is_alive() const { return alive; }

private:
    int wave_count = 0;
    int max_waves = 3;
    int timer = 0;
    int wave_duration = 60; // 1 second per wave
    double max_radius = 200.0;
    double current_radius = 0;
    bool alive = true;
};

#endif