#include "FontCenter.h"
#include <allegro5/allegro_ttf.h>
#include <iostream> 

namespace FontSetting {
    const char caviar_dreams_font_path[] = "./assets/font/Caviar_Dreams_Bold.ttf";
    // 有改字體喔喔
    const char arcade_classic_font_path[] = "./assets/font/Star Crush.ttf"; 
}

void
FontCenter::init() {
    for(const int &fs : FontSize::list) {
        caviar_dreams[fs] = al_load_ttf_font(FontSetting::caviar_dreams_font_path, fs, 0);
        
     
        courier_new[fs] = al_load_ttf_font(FontSetting::arcade_classic_font_path, fs, 0);

        if(!courier_new[fs]) {
            std::cerr << "Failed to load Font: " << FontSetting::arcade_classic_font_path << " at size " << fs << std::endl;
        }
    }
}

FontCenter::~FontCenter() {
    for(auto &[size, font] : caviar_dreams)
        al_destroy_font(font);
    for(auto &[size, font] : courier_new)
        al_destroy_font(font);
}