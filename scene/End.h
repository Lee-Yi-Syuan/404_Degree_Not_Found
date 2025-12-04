#ifndef END_H_INCLUDED
#define END_H_INCLUDED

#include <allegro5/bitmap.h>
#include <map>
#include <string>

class EndScreen
{
public:
    // 定義所有可能的結局類型
    enum class EndType {
        FAIL_END,     
        PREFECT_END,  
        CLUB_END,    
        LOVE_END,    
        STUDY_END,   
        
        ENDTYPE_MAX
    };

    EndScreen();
    void init();
    void update();
    void draw();

    // 讓外部設定當前的結局類型
    void set_end_type(EndType type) { current_end_type = type; }

    // 檢查是否要求按空格回到開始畫面
    bool is_restart_requested() const { return restart_requested; }

private:
    EndType current_end_type = EndType::FAIL_END; // 預設fail 聽起來很難過
    
 
    std::map<EndType, std::string> end_img_paths;
    

    std::map<EndType, ALLEGRO_BITMAP*> end_bitmaps;
    
    bool restart_requested = false;
};

#endif