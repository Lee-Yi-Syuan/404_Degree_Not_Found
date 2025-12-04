#ifndef CLOSET_H_INCLUDED
#define CLOSET_H_INCLUDED

#include "../shapes/Rectangle.h"
#include "../Object.h"
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <map>
#include <string>

enum class ClosetState {
	untouched,touched,ClosetState_MAX
};

class Closet: public Object
{
    public:
        void init();
        void draw();
        bool is_player_touching();
        bool closet_world(){return closet_world_loaded;}
        void interact();
        void reset_closet_world()  
        {
            closet_world_loaded=false;
        }
    private:
        //衣櫃的左上角(x,y)座標
        int x1,y1;
        //衣櫃的寬高
        int w,h;
        bool showing_prompt = false;
        bool closet_world_loaded= false;
        std::unique_ptr<Rectangle> trigger_shape;
        ClosetState state=ClosetState::untouched;
        std::map<ClosetState ,std::string> imgPath;
};

#endif