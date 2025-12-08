#ifndef BED_H_INCLUDED
#define BED_H_INCLUDED

#include "../shapes/Rectangle.h"
#include "../Object.h"
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <map>
#include <string>

enum class BedState {
	untouched,touched,BedState_MAX
};

class Bed: public Object
{
    public:
        void init();
        void draw();
        bool is_player_touching();
        bool bed_world(){return bed_world_loaded;}
        void interact(bool enabled = true);
        void reset_bed_world()  
        {
            bed_world_loaded=false;
        }
    private:
        //床的右上角(x,y)座標
        int x1,y1;
        //床的寬高
        int w,h;
        bool showing_prompt = false;
        bool bed_world_loaded = false;
        std::unique_ptr<Rectangle> trigger_shape;
        BedState state=BedState::untouched;
        std::map<BedState,std::string> imgPath;
};

#endif