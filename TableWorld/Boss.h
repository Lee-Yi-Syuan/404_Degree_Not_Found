#ifndef BOSS_H_INCLUDED
#define BOSS_H_INCLUDED

#include "../Object.h"
#include "../shapes/Rectangle.h"
#include "../shapes/Point.h"
#include "Health_damage.h"
#include <vector>
#include <queue>
#include <map>

struct BossGravity {
    double vy = 0;
    bool on_ground = false;
};

enum class BossState {
    IDLE,
    MOVE,
    FLY,
    ATTACK,
    BossState_MAX
};

enum class BossAttackType {
    NONE,
    NORMAL, // Mode 1
    RANGE,  // Mode 2
    SHORT,  // Mode 3 (New)
    SPECIAL // Mode 4
};

enum class BossDirection {
	LEFT, RIGHT, Front, Back, BossDirection_MAX
};

class Boss : public Object
{
	public:
		void init();
		void update();
		void draw() override;
        //void take_damage(int damage);
        //int get_HP() const { return HP; }
        //int get_maxHP() const { return maxHP; }

        //載入血量系統
        HealthComponent hp_system;

        //重力系統
		BossGravity gravity;
		int width() const { return w; }
		int height() const { return h; }
        int fly_timer = 0;
        
        //設定boss是否在移動狀態
        void set_active(bool active) { is_active = active; }
        //確認boss是否在移動狀態
        bool get_active() const { return is_active; }


        
        void spawn(); // Activate with effect
        BossDirection get_dir() const { return dir; }
        
	private:
        bool is_active = false;
        int spawn_effect_timer = 0;
		BossDirection dir = BossDirection::Front;
        BossState state = BossState::IDLE;
        BossAttackType current_attack = BossAttackType::NONE;

		//移動速度
		double speed = 3;
		//重力加速度
		double gravity_acc = 0.5;
		//跳躍/飛行力
		double jump_speed = 15;
        double fly_speed = 5;

		//角色圖片路徑
		std::string imgPath;
		//角色寬高
		int w, h;

        //int HP = 500;
        //int maxHP = 500;

        // AI Control
        int action_timer = 0;
        int action_duration = 0;
        
        void ai_decision();
        void perform_attack();
};

#endif