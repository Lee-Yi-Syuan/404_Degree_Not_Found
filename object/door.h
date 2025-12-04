#ifndef DOOR_H_INCLUDED
#define DOOR_H_INCLUDED

#include "../shapes/Rectangle.h"
#include "../Object.h"
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <map>
#include <string>

enum class DoorState {
	untouched,touched,DoorState_MAX
};

class Door: public Object
{
    public:
        void init();
        void draw();
        bool is_player_touching();
        bool door_world(){return door_world_loaded;}
        void interact();
        void reset_door_world()  
        {
            door_world_loaded=false;
        }
    private:
        //門的左上角(x,y)座標
        int x1,y1;
        //門的寬高
        int w,h;
        bool showing_prompt = false;
        bool door_world_loaded = false;
        std::unique_ptr<Rectangle> trigger_shape;
        DoorState state=DoorState::untouched;
        std::map<DoorState,std::string> imgPath;
};

#endif