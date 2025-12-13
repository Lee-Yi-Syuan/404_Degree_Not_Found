#ifndef UI_H_INCLUDED
#define UI_H_INCLUDED

#include <allegro5/bitmap.h>
#include <vector>
#include <tuple>
#include "./shapes/Point.h"

class UI
{
public:
	UI() {}
	void init();
	void update();
	void draw();

    // Result Dialog methods
    void show_result_dialog(std::string message);
    bool is_showing_result() const { return showing_result; }

private:
    // Result Dialog state
    bool showing_result = false;
    std::string result_msg;
    
    void draw_result_dialog();
    void interact_result_dialog();
};

#endif

