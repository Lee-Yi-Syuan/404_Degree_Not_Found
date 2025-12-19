#ifndef THERO_H_INCLUDED
#define THERO_H_INCLUDED

#include "../Object.h"
#include "../shapes/Rectangle.h"
#include "../shapes/Point.h"
#include "Health_damage.h"
#include "../algif5/algif.h"
#include <vector>
#include <queue>
#include <map>
#include <allegro5/allegro.h>          
#include <allegro5/allegro_image.h>    

struct Gravity {
    double vy = 0;
    bool on_ground = false;
};

enum class HeroMode {
    ATTACK,
    HEALING,
    DODGE,
    HeroMode_MAX
};

// fixed settings
enum class THeroState {
    LEFT, RIGHT, Front, Back, THeroState_MAX
};

/**
 * @brief The class of a character.
 * @details Character inherits Object and takes Rectangle as its hit box.
 */
class THero : public Object
{
    public:
        void init();
        void update();
        void draw() override;
        
        HealthComponent hp_system;
        Gravity gravity;
        int width() const { return w; }
        int height() const { return h; }
        HeroMode get_mode() const { return current_mode; }
        bool is_immune() const { return is_dashing || invincible_timer > 0; }
        void hit(int damage, bool from_left); // 除裡受傷的扣血、暈眩、擊退等效果
        
        void set_input_locked(bool locked) { input_locked = locked; }   // 鎖定輸入（如過場動畫時）
        bool is_input_locked() const { return input_locked; }
        void force_move(double dx); // 遊戲開場的自動移動動畫
        
        double drop_start_y = -1.0; // For dropping down platforms

    private:
        THeroState state = THeroState::Front;
        HeroMode current_mode = HeroMode::ATTACK;
        
        bool input_locked = false;
        
        // Skills
        int heal_cooldown_timer = 0;
        int dash_timer = 0;
        int dash_cooldown_timer = 0;
        bool is_dashing = false;
        
        // Damage & Status
        int invincible_timer = 0;
        int stun_timer = 0;
        double knockback_vx = 0;
        
        // 改主角的動畫
        ALGIF_ANIMATION *gif_status = nullptr;
        int animation_tick = 0;              // 動畫計時器
        bool face_right = true;              // 左右翻轉狀態

        // 移動速度
        double speed = 5;
        // 重力加速度
        double gravity_acc = 0.5;
        // 跳躍力
        double jump_speed = 15;
        // 角色圖片路徑 (保留以相容舊代碼)
        std::map<THeroState, std::string> gitfPath;
        // 角色寬高
        int w, h;
};

#endif