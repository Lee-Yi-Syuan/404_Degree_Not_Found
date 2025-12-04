#ifndef FLOOR_H_INCLUDED
#define FLOOR_H_INCLUDED
#include "../shapes/Rectangle.h"
#include "../Object.h"

class Floor: public Object
{
    public:
        void init(int xl,int yup,int xr,int ydown);
        void draw();
        bool is_on_floor(int x,int y);
        int get_x1(){return x1;}
        int get_y1(){return y1;}
        int get_x2(){return x2;}
        int get_y2(){return y2;}
    private:
        //地板的範圍(x1,y1,x2,y2)座標
        int x1,y1,x2,y2;
};
#endif