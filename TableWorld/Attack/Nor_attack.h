#ifndef NOR_ATTCK_H_INCLUDED
#define NOR_ATTCK_H_INCLUDED
#include "../../Object.h"
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>

class NorAttack: public Object
{
    public:
        void init( int x_pos, int y_pos, bool facing_left );
        void draw();
        void update();
        void interact();
        bool is_alive() const { return alive; }
    private:
        //普通攻擊圖像右上角(x,y)座標
        int x, y;          // the position of image
        int width, height; // the width and height of image
        //ALLEGRO_BITMAP *img;

        int life_counter = 0;
        int max_life = 10;
        bool alive = true;
        bool facing_left = false;
};

#endif