#ifndef TABLE_H_INCLUDED
#define TABLE_H_INCLUDED

#include "../shapes/Rectangle.h"
#include "../Object.h"
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <map>
#include <string>

enum class TableState {
	untouched,touched,TableState_MAX
};

class Table: public Object
{
    public:
        void init();
        void draw();
        bool is_player_touching();
        bool table_world(){return table_world_loaded;}
        void interact(bool enabled = true);
        void reset_table_world()  
        {
            table_world_loaded=false;
        }
    private:
        //桌子的右上角(x,y)座標
        int x1,y1;
        //桌子的寬高
        int w,h;
        bool showing_prompt = false;
        bool table_world_loaded = false;
        std::unique_ptr<Rectangle> trigger_shape;
        TableState state=TableState::untouched;
        std::map<TableState,std::string> imgPath;
};

#endif