#include "DoorWorld.h"
#include "../data/DataCenter.h"
#include "../data/FontCenter.h"
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_image.h>
#include <cstdlib>
#include <iostream>

// 保持原本 DItem::draw 的定義（作為圖片讀取失敗時的備援）
void DItem::draw() {
    if (active) {
        // 原本 30 的圓心位置也要隨著放大調整 (1.5倍)
        al_draw_filled_circle(x + w/2, y + h/2, w/2, al_map_rgb(255, 215, 0)); 
    }
}

DWorld::DWorld() {
    // 初始指標設為空，避免解構時錯誤
    background = nullptr;
    player_img1 = nullptr;
    player_img2 = nullptr;
    enemy_img = nullptr;
    award_img = nullptr;
    
    face_right = false; 
    animation_tick = 0;
    state = DWorldState::PLAYING;
    init(); 
}

DWorld::~DWorld() {
    for(auto e : enemies) delete e;
    enemies.clear();
    // 徹底釋放圖片資源，防止記憶體洩漏
    if(background) al_destroy_bitmap(background);
    if(player_img1) al_destroy_bitmap(player_img1);
    if(player_img2) al_destroy_bitmap(player_img2);
    if(enemy_img) al_destroy_bitmap(enemy_img);
    if(award_img) al_destroy_bitmap(award_img);
}

void DWorld::init() {
    DataCenter *DC = DataCenter::get_instance();
    
    // 載入資源 (請確保資料夾名稱 Door 為首字母大寫)
    if(!background) background = al_load_bitmap("assets/image/Door/doorback.png");
    if(!player_img1) player_img1 = al_load_bitmap("assets/image/Door/player1.png");
    if(!player_img2) player_img2 = al_load_bitmap("assets/image/Door/player2.png");
    if(!enemy_img)  enemy_img  = al_load_bitmap("assets/image/Door/enemy.png");
    if(!award_img)  award_img  = al_load_bitmap("assets/image/Door/award.png");

    dhero.init(); 
    score = 0; 
    win_score = 300;
    state = DWorldState::PLAYING;
    return_to_main = false;
    animation_tick = 0;
    
    // 初始化敵人
    for(auto e : enemies) delete e;
    enemies.clear();
    for(int i=0; i<8; i++) {
        DEnemy *e = new DEnemy();
        // 隨機初始位置，減去 60 是因為敵人也被放大了 (40 * 1.5)
        double ex = rand() % (DC->window_width - 60);
        double ey = rand() % (DC->window_height - 60);
        double edx = (rand() % 5 + 1) * (rand() % 2 ? 1 : -1);
        double edy = (rand() % 5 + 1) * (rand() % 2 ? 1 : -1);
        e->init(ex, ey, edx, edy);
        enemies.push_back(e);
    }
    
    // 初始化道具 (實體尺寸直接設為原本 30 的 1.5 倍 = 45)
    items.clear();
    for(int i=0; i<10; i++) {
        DItem item;
        item.w = 45; // 30 * 1.5
        item.h = 45; 
        item.x = rand() % (DC->window_width - (int)item.w);
        item.y = rand() % (DC->window_height - (int)item.h);
        item.active = true;
        item.shape = Rectangle(item.x, item.y, item.x + item.w, item.y + item.h);
        items.push_back(item);
    }
}

void DWorld::start_level() { init(); }
void DWorld::reset() { init(); }

bool DWorld::check_collision(const Rectangle& r1, const Rectangle& r2) {
    return r1.overlap(r2);
}

//做碰撞檢查跟勝負判定
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
    animation_tick = (animation_tick + 1) % 40; 

    // 更新水平翻轉狀態
    if (DC->key_state[ALLEGRO_KEY_RIGHT]) face_right = true;
    else if (DC->key_state[ALLEGRO_KEY_LEFT]) face_right = false;

    if (!dhero.shape) return;
    Rectangle *heroRect = dhero.shape.get();

    for (auto e : enemies) {
        e->update();
        if (!e->shape) continue;
        Rectangle *enemyRect = e->shape.get();
        
        if (check_collision(*heroRect, *enemyRect)) {
            if (!dhero.shield.active) dhero.hearts--;
            // 撞到後重置位置與速度
            e->init(rand() % (DC->window_width - 60), rand() % (DC->window_height - 60), 
                    (rand() % 5 + 1) * (rand() % 2 ? 1 : -1), (rand() % 5 + 1) * (rand() % 2 ? 1 : -1));
            
            if (dhero.hearts <= 0) state = DWorldState::LOSE;
        }
    }

    for (auto &item : items) {
        if (item.active && check_collision(*heroRect, item.shape)) {
            score += 10;
            // 重新產生位置並更新實體碰撞框 (45x45)
            item.x = rand() % (DC->window_width - (int)item.w);
            item.y = rand() % (DC->window_height - (int)item.h);
            item.shape = Rectangle(item.x, item.y, item.x + item.w, item.y + item.h);
            if (score >= win_score) state = DWorldState::WIN;
        }
    }
}


//字的部分
void DWorld::draw_ui() {
    DataCenter *DC = DataCenter::get_instance();
    FontCenter *FC = FontCenter::get_instance();
    ALLEGRO_FONT *font = FC->caviar_dreams[FontSize::MEDIUM];

    al_draw_textf(font, al_map_rgb(0, 0, 0), 10, 10, 0, "Happiness: %d / 300", score);

    float base_x = 20;
    float base_y = DC->window_height - 60;
    for(int i = 0; i < dhero.hearts; i++) {
        al_draw_filled_rectangle(base_x + i*40, base_y, base_x + i*40 + 30, base_y + 30, al_map_rgb(255, 0, 0));
    }

    float icon_x = DC->window_width - 150; 
    float icon_y = 10;
    float gap = 60;
    const char* labels[] = {"A (Heal)", "S (Shield)", "D (Speed)"};
    Skill* skills[] = {&dhero.heal, &dhero.shield, &dhero.speedup};

    for(int i = 0; i < 3; i++) {
        float y = icon_y + i * gap;
        al_draw_rectangle(icon_x, y, icon_x + 120, y + 50, al_map_rgb(0, 0, 0), 2);
        al_draw_text(font, al_map_rgb(0, 0, 0), icon_x + 5, y + 5, 0, labels[i]);
        char status_text[20];
        if(skills[i]->on_cooldown) sprintf(status_text, "%.1fs", skills[i]->cooldown);
        else sprintf(status_text, "Ready");
        al_draw_text(font, al_map_rgb(0, 0, 0), icon_x + 5, y + 25, 0, status_text);
        if(skills[i]->active) al_draw_rectangle(icon_x-2, y-2, icon_x+122, y+52, al_map_rgb(0,255,0), 3);
    }
}

void DWorld::draw() {
    DataCenter *DC = DataCenter::get_instance();
    FontCenter *FC = FontCenter::get_instance();

    // 背景
    if (background) {
        al_draw_scaled_bitmap(background, 0, 0, al_get_bitmap_width(background), al_get_bitmap_height(background),
                              0, 0, DC->window_width, DC->window_height, 0);
    } else {
        al_clear_to_color(al_map_rgb(249, 244, 236));
    }

    // 道具
    for (auto &item : items) {
        if (item.active) {
            if (award_img) {
                al_draw_scaled_bitmap(award_img, 0, 0, al_get_bitmap_width(award_img), al_get_bitmap_height(award_img),
                                      item.x, item.y, item.w, item.h, 0);
            } else {
                item.draw(); 
            }
        }
    }

    //敵人
    for (auto e : enemies) {
        if (enemy_img && e->shape) {
            Rectangle* er = e->shape.get();
            // 視覺放大 1.5
            float dw = (er->x2 - er->x1) * 1.5f;
            float dh = (er->y2 - er->y1) * 1.5f;
            al_draw_scaled_bitmap(enemy_img, 0, 0, al_get_bitmap_width(enemy_img), al_get_bitmap_height(enemy_img),
                                  er->x1, er->y1, dw, dh, 0);
        } else {
            e->draw();
        }
    }

    // 英雄
    if (dhero.shape) {
        Rectangle* r = dhero.shape.get();
        ALLEGRO_BITMAP* current_frame = (animation_tick < 20) ? player_img1 : player_img2;
        if (!current_frame) current_frame = player_img1;

        if (current_frame) {
            int flags = face_right ? ALLEGRO_FLIP_HORIZONTAL : 0;
            float dw = (r->x2 - r->x1) * 1.5f;
            float dh = (r->y2 - r->y1) * 1.5f;
            al_draw_scaled_bitmap(current_frame, 0, 0, al_get_bitmap_width(current_frame), al_get_bitmap_height(current_frame),
                                  r->x1, r->y1, dw, dh, flags);
        } else {
            dhero.draw();
        }
    }

    draw_ui();

    if (state == DWorldState::WIN || state == DWorldState::LOSE) {
        al_draw_filled_rectangle(0, 0, DC->window_width, DC->window_height, al_map_rgba(0, 0, 0, 150));
        const char* msg = (state == DWorldState::WIN) ? "You lived a wonderful day:)" : "I should have stayed home...";
        al_draw_text(FC->caviar_dreams[FontSize::LARGE], al_map_rgb(255, 255, 255), 
                     DC->window_width/2, DC->window_height/2 - 50, ALLEGRO_ALIGN_CENTER, msg);
        al_draw_text(FC->caviar_dreams[FontSize::LARGE], al_map_rgb(255, 255, 255), 
                     DC->window_width/2, DC->window_height/2 + 50, ALLEGRO_ALIGN_CENTER, "Press Space to Return");
    }
}