#ifndef CANDLE_H_INCLUDED
#define CANDLE_H_INCLUDED

#include "../shapes/Rectangle.h"
#include "../Object.h"
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <map>
#include <string>

enum class CandleState {
	untouched,touched,CandleState_MAX
};

class Candle: public Object
{
    public:
        void init();
        void draw();
        bool is_player_touching();
        bool candle_lighted(){return candle_light;}
        void interact(bool enabled = true);
        void reset_candle_light()  
        {
            candle_light=false;
        }
    private:
        //燭台的左上角(x,y)座標
        int x1,y1;
        //燭台的寬高
        int w,h;

        bool showing_prompt = false;
        bool candle_light= false;
        std::unique_ptr<Rectangle> trigger_shape;
        CandleState state=CandleState::untouched;
        std::map<CandleState ,std::string> imgPath;
};

#endif