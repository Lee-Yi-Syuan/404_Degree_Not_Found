#ifndef DOOR_WORLD_H
#define DOOR_WORLD_H

// 注意這裡的引用路徑：要去抓 DoorWorld 資料夾底下的 header
#include "../DoorWorld/DHero.h"
#include "../DoorWorld/DEnemy.h"
#include <vector>

class DItem {
public:
    Rectangle shape;
    bool active;
    int w, h;
    double x, y;
    void draw();
};

enum class DWorldState {
    PLAYING,
    WIN,
    LOSE
};

class DWorld {
public:
    DWorld();
    ~DWorld();
    
    void init();
    void update();
    void draw();
    void reset();
    
    void start_level();
    
    bool is_return_to_main() const { return return_to_main; }
    void reset_return_to_main() { return_to_main = false; }
    bool is_victory() { return state == DWorldState::WIN; }

private:
    bool check_collision(const Rectangle& r1, const Rectangle& r2);
    void draw_ui();

    DHero dhero;
    std::vector<DEnemy*> enemies;
    std::vector<DItem> items;
    
    int score;
    int win_score;
    DWorldState state;
    bool return_to_main;

    //圖片
private:
    ALLEGRO_BITMAP *background = nullptr;
    ALLEGRO_BITMAP *player_img1 = nullptr; // 因為我想要讓他有走路的感覺所以
    ALLEGRO_BITMAP *player_img2 = nullptr; 
    ALLEGRO_BITMAP *enemy_img = nullptr;
    ALLEGRO_BITMAP *award_img = nullptr;

    int animation_tick = 0; // 用來計數，決定切換速度
    bool face_right = false; // 圖片預設朝左，所以預設設為 false (不翻轉)

};

#endif