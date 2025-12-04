#include "floor.h"

void Floor::init(int xl,int yup,int xr,int ydown)
{
    x1=xl;
    y1=yup;
    x2=xr;
    y2=ydown;
}

void Floor::draw()
{
    // Implement drawing logic if needed
}

bool Floor::is_on_floor(int x,int y)
{
    return (x>=x1&&x<=x2&&y>=y1&&y<=y2);
}