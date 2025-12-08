#include "DoorWorld.h"
#include "../data/DataCenter.h"
#include "../data/FontCenter.h"
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <cstdlib>

// Item 是在 .h 裡定義的小 class，實作放在這裡沒問題
void DItem::draw() {
    if (active) {
        al_draw_filled_circle(x + w/2, y + h/2, w/2, al_map_rgb(255, 215, 0)); 
    }
}

DWorld::DWorld() {
    state = DWorldState::PLAYING;
    init(); // 記得要 init，避免當機
}

DWorld::~DWorld() {
    for(auto e : enemies) delete e;
    enemies.clear();
}

void DWorld::init() {
    DataCenter *DC = DataCenter::get_instance();
    
    dhero.init(); 
    
    score = 0; 
    win_score = 300;
    state = DWorldState::PLAYING;
    return_to_main = false;
    
    // 初始化敵人
    for(auto e : enemies) delete e;
    enemies.clear();
    for(int i=0; i<8; i++) {
        DEnemy *e = new DEnemy();
        double ex = rand() % (DC->window_width - 40);
        double ey = rand() % (DC->window_height - 40);
        double edx = (rand() % 5 + 1) * (rand() % 2 ? 1 : -1);
        double edy = (rand() % 5 + 1) * (rand() % 2 ? 1 : -1);
        e->init(ex, ey, edx, edy);
        enemies.push_back(e);
    }
    
    // 初始化道具
    items.clear();
    for(int i=0; i<10; i++) {
        DItem item;
        item.w = 20; item.h = 20;
        item.x = rand() % (DC->window_width - item.w);
        item.y = rand() % (DC->window_height - item.h);
        item.active = true;
        item.shape = Rectangle(item.x, item.y, item.x+item.w, item.y+item.h);
        items.push_back(item);
    }
}

void DWorld::start_level() {
    init();
}

void DWorld::reset() {
    init();
}

bool DWorld::check_collision(const Rectangle& r1, const Rectangle& r2) {
    return r1.overlap(r2);
}

void DWorld::update() {
    DataCenter *DC = DataCenter::get_instance();
    
    if (state != DWorldState::PLAYING) {
        if (DC->key_state[ALLEGRO_KEY_SPACE] && !DC->prev_key_state[ALLEGRO_KEY_SPACE]) {
            return_to_main = true; 
        }
        if (DC->key_state[ALLEGRO_KEY_ENTER] && !DC->prev_key_state[ALLEGRO_KEY_ENTER]) {
            reset(); 
        }
        return;
    }

    dhero.update();
    
    // 防呆：如果 Hero shape 沒初始化好，就先不檢測碰撞
    if (!dhero.shape) return;
    Rectangle *heroRect = dhero.shape.get();

    for (auto e : enemies) {
        e->update();
        if (!e->shape) continue; // 防呆
        Rectangle *enemyRect = e->shape.get();
        
        if (check_collision(*heroRect, *enemyRect)) {
            if (dhero.shield.active) {
                // 無敵
            } else {
                dhero.hearts--;
            }
            
            double new_x = rand() % (DC->window_width - (int)e->get_w());
            double new_y = rand() % (DC->window_height - (int)e->get_h());
            e->set_position(new_x, new_y);
            
            if (dhero.hearts <= 0) {
                state = DWorldState::LOSE;
            }
        }
    }

    for (auto &item : items) {
        item.shape = Rectangle(item.x, item.y, item.x + item.w, item.y + item.h);
        
        if (check_collision(*heroRect, item.shape)) {
            score += 10;
            item.x = rand() % (DC->window_width - item.w);
            item.y = rand() % (DC->window_height - item.h);
            
            if (score >= win_score) {
                state = DWorldState::WIN;
            }
        }
    }
}

void DWorld::draw_ui() {
    DataCenter *DC = DataCenter::get_instance();
    FontCenter *FC = FontCenter::get_instance();
    ALLEGRO_FONT *font = FC->caviar_dreams[FontSize::MEDIUM];
    
    al_draw_textf(font, al_map_rgb(0, 0, 0), 10, 10, 0, "Happiness: %d  /300", score);

    float base_x = 20;
    float base_y = DC->window_height - 60;
    for(int i=0; i<dhero.hearts; i++) {
        al_draw_filled_rectangle(base_x + i*40, base_y, base_x + i*40 + 30, base_y + 30, al_map_rgb(255, 0, 0));
    }

    float icon_x = DC->window_width - 80;
    float icon_y = 10;
    float gap = 50;
    
    auto draw_skill_status = [&](const char* label, Skill& skill, int index) {
        float y = icon_y + index * gap;
        al_draw_rectangle(icon_x, y, icon_x + 100, y + 40, al_map_rgb(0, 0, 0), 2);
        
        char status_text[20];
        if (skill.on_cooldown) {
            sprintf(status_text, "%.1fs", skill.cooldown);
        } else {
            sprintf(status_text, "Ready");
        }
        
        al_draw_text(font, al_map_rgb(0, 0, 0), icon_x - 30, y + 10, 0, label);
        al_draw_text(font, al_map_rgb(0, 0, 0), icon_x + 10, y + 10, 0, status_text);
        
        if(skill.active) {
            al_draw_rectangle(icon_x-2, y-2, icon_x + 102, y + 42, al_map_rgb(0, 255, 0), 3);
        }
    };

    draw_skill_status("J (Heal)         ", dhero.heal, 0);
    draw_skill_status("K (Shield)        ", dhero.shield, 1);
    draw_skill_status("L (Speed)        ", dhero.speedup, 2);
}

void DWorld::draw() {
    DataCenter *DC = DataCenter::get_instance();
    FontCenter *FC = FontCenter::get_instance();

    al_clear_to_color(al_map_rgb(249, 244, 236));

    dhero.draw();
    for (auto e : enemies) e->draw();
    for (auto &item : items) item.draw();

    draw_ui();

    if (state == DWorldState::WIN) {
        al_draw_filled_rectangle(0, 0, DC->window_width, DC->window_height, al_map_rgba(0, 0, 0, 150));
        al_draw_text(FC->caviar_dreams[FontSize::LARGE], al_map_rgb(255, 255, 255), 
                     DC->window_width/2, DC->window_height/2 - 50, ALLEGRO_ALIGN_CENTER, "Congratulations on a smooth day!");
        al_draw_text(FC->caviar_dreams[FontSize::MEDIUM], al_map_rgb(255, 255, 255), 
                     DC->window_width/2, DC->window_height/2 + 20, ALLEGRO_ALIGN_CENTER, "Press SPACE to Exit");
    } else if (state == DWorldState::LOSE) {
        al_draw_filled_rectangle(0, 0, DC->window_width, DC->window_height, al_map_rgba(50, 0, 0, 150));
        al_draw_text(FC->caviar_dreams[FontSize::LARGE], al_map_rgb(255, 100, 100), 
                     DC->window_width/2, DC->window_height/2 - 50, ALLEGRO_ALIGN_CENTER, "I should have stayed home...");
        al_draw_text(FC->caviar_dreams[FontSize::MEDIUM], al_map_rgb(255, 255, 255), 
                     DC->window_width/2, DC->window_height/2 + 20, ALLEGRO_ALIGN_CENTER, "Press SPACE to Exit");
    }
}