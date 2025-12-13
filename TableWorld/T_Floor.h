#ifndef TFLOOR_H_INCLUDED
#define TFLOOR_H_INCLUDED
#include "../Object.h"
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>

class TFloor: public Object
{
    public:
        void init();
        void draw();
        void update();
        void interact();
        void load_map(int window);
    private:
        //地板的右上角(x,y)座標
        int x, y;          // the position of image
        int width, height; // the width and height of image
        ALLEGRO_BITMAP *img;
        int map_data[18][32];
};

#endif