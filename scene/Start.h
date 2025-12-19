#ifndef Start_H_INCLUDED
#define Start_H_INCLUDED
#include "../shapes/Rectangle.h"
#include "../Object.h"
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <string>
#include <map>

enum class StartState {
	Normal,Special,StartState_MAX
};

class StartScreen
{
    public:
        StartScreen();
        void init();
        void update();
        void draw();
        void reset();

        bool isStarted() const  {return start_pressed;}
    private:
        bool start_pressed;
         StartState state=StartState::Normal;
        std::map<StartState,std::string> imgPath;
        
        int sub_state;           // 用來記錄目前的劇情階段
        std::string storyPath[2];
};









#endif
