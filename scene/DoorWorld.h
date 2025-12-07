#ifndef DWorld_H_INCLUDED
#define DWorld_H_INCLUDED
#include "../shapes/Rectangle.h"
#include "../Object.h"
#include "../DoorWorld/DHero.h"
#include "../DoorWorld/DEnemy.h" // Added
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <string>
#include <map>
#include <vector>

enum class DWorldState {
	PLAYING, WIN, LOSE
};

// DEnemy struct removed, using class from Enemy.h

struct DItem {
    double x, y;
    int w, h;
    bool active;
    Rectangle shape;
    
    void draw();
};

class DWorld
{
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
        bool is_victory() const { return state == DWorldState::WIN; }
        
        bool is_finished() const { return state != DWorldState::PLAYING; }
        
    private:
        DWorldState state = DWorldState::PLAYING;
        bool return_to_main = false;
        DHero dhero;
        
        std::vector<DEnemy*> enemies;
        std::vector<DItem> items;
        
        int score = 0;
        int lives = 3;
        int win_score = 50;
        
        // Helper
        bool check_collision(const Rectangle& r1, const Rectangle& r2);
};
#endif