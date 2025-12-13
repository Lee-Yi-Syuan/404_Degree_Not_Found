#ifndef TWORLD_H_INCLUDED
#define TWORLD_H_INCLUDED

#include <allegro5/bitmap.h>
#include <map>
#include <string>

class TWorldScreen
{
public:
    // 定義所有可能的關卡類型
    enum class TWorldType {
        Program,
        Calculus,
        DataStructure,
        TWorldTYPE_MAX
    };

    TWorldScreen();
    void init();
    void start_level(); // Reset state when entering the level
    void update();
    void draw();

    // 讓外部設定當前的結局類型
    void set_TWorld_type(TWorldType type) { current_TWorld_type = type; }

    // 檢查是否要求按空格回到開始畫面
    bool is_restart_requested() const { return restart_requested; }
    
    // 檢查是否要求回到主畫面
    bool is_return_to_main() const { return return_to_main; }
    void reset_return_to_main() { return_to_main = false; victory = false; fail = false; }
    bool is_victory() const { return victory; }

    int get_current_TWorld_type() const { return static_cast<int>(current_TWorld_type); }
   
    enum class LevelState {
        ENTRANCE,
        WAIT_BOSS,
        BATTLE,
        VICTORY,
        FAIL
    };

private:
    TWorldType current_TWorld_type = TWorldType::Program; 
    std::map<TWorldType, std::string> tworld_img_background_paths;
    std::map<int, std::string> tworld_img_result_paths;
    std::map<int, ALLEGRO_BITMAP*> tworld_result_bitmaps;
    std::map<TWorldType, ALLEGRO_BITMAP*> tworld_background_bitmaps;
    
    bool restart_requested = false;
    bool return_to_main = false;
    bool victory = false;
    bool fail = false;
    
    LevelState level_state = LevelState::ENTRANCE;
    int state_timer = 0;
};

#endif