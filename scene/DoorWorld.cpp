#include "DoorWorld.h"
#include "../data/DataCenter.h"
#include "../data/FontCenter.h"
#include "../Player.h"
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <cstdlib>

// --- DItem Implementation ---
void DItem::draw() {
    if(active) {
        al_draw_filled_circle(x + w/2, y + h/2, w/2, al_map_rgb(255, 255, 0)); // Yellow Item
    }
}

// --- DWorld Implementation ---
DWorld::DWorld()
{
    state = DWorldState::PLAYING;
}

DWorld::~DWorld()
{
    for(auto e : enemies) {
        delete e;
    }
    enemies.clear();
}

void DWorld::init()
{
    DataCenter *DC = DataCenter::get_instance();
    dhero.init();
    
    score = 0;
    lives = 3;
    state = DWorldState::PLAYING;
    return_to_main = false;
    
    // Init Enemies
    for(auto e : enemies) {
        delete e;
    }
    enemies.clear();
    for(int i=0; i<5; i++) {
        DEnemy *e = new DEnemy();
        double ex = rand() % (DC->window_width - 40);
        double ey = rand() % (DC->window_height - 40);
        double edx = (rand() % 5 + 2) * (rand()%2 ? 1 : -1);
        double edy = (rand() % 5 + 2) * (rand()%2 ? 1 : -1);
        e->init(ex, ey, edx, edy);
        enemies.push_back(e);
    }
    
    // Init Items
    items.clear();
    for(int i=0; i<3; i++) {
        DItem item;
        item.w = 30; item.h = 30;
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

void DWorld::update()
{
    DataCenter *DC = DataCenter::get_instance();
    if(state != DWorldState::PLAYING) {
        if(DC->key_state[ALLEGRO_KEY_SPACE] && !DC->prev_key_state[ALLEGRO_KEY_SPACE]) {
            // Restart or Exit? For now, restart
            // reset();
            // Change to return to main
            return_to_main = true;
        }
        return;
    }

    dhero.update();
    Rectangle *heroRect = dynamic_cast<Rectangle*>(dhero.shape.get());
    
    // Update Enemies & Check Collision
    for(auto e : enemies) {
        e->update();
        // Need to get shape from DEnemy object now
        Rectangle *enemyRect = dynamic_cast<Rectangle*>(e->shape.get());
        if(enemyRect && check_collision(*heroRect, *enemyRect)) {
            lives--;
            // Respawn enemy to avoid continuous hit
            double new_x = rand() % (DC->window_width - e->get_w());
            double new_y = rand() % (DC->window_height - e->get_h());
            e->set_position(new_x, new_y);
            
            if(lives <= 0) {
                state = DWorldState::LOSE;
                DC->player->now_fail++;
            }
        }
    }
    
    // Update Items & Check Collision
    for(auto &item : items) {
        if(item.active) {
            // Update item shape position just in case (static items don't move though)
            item.shape = Rectangle(item.x, item.y, item.x+item.w, item.y+item.h);
            
            if(check_collision(*heroRect, item.shape)) {
                score += 10;
                // Respawn item
                item.x = rand() % (DC->window_width - item.w);
                item.y = rand() % (DC->window_height - item.h);
                
                if(score >= win_score) {
                    state = DWorldState::WIN;
                }
            }
        }
    }
}

void DWorld::draw()
{
    DataCenter *DC = DataCenter::get_instance();
    FontCenter *FC = FontCenter::get_instance();
    
    // Draw Background (Simple color for now)
    al_clear_to_color(al_map_rgb(50, 50, 50));
    
    dhero.draw();
    
    for(auto e : enemies) e->draw();
    for(auto &item : items) item.draw();
    
    // UI
    al_draw_textf(FC->caviar_dreams[FontSize::MEDIUM], al_map_rgb(255, 255, 255), 
                  10, 10, 0, "Score: %d / %d", score, win_score);
    al_draw_textf(FC->caviar_dreams[FontSize::MEDIUM], al_map_rgb(255, 255, 255), 
                  10, 40, 0, "Lives: %d", lives);
                  
    if(state == DWorldState::WIN) {
        al_draw_text(FC->caviar_dreams[FontSize::LARGE], al_map_rgb(0, 255, 0), 
                     DC->window_width/2, DC->window_height/2, ALLEGRO_ALIGN_CENTER, "YOU WIN! Press Space");
    } else if(state == DWorldState::LOSE) {
        al_draw_text(FC->caviar_dreams[FontSize::LARGE], al_map_rgb(255, 0, 0), 
                     DC->window_width/2, DC->window_height/2, ALLEGRO_ALIGN_CENTER, "GAME OVER! Press Space");
    }
}