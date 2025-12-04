#include "Game.h"
#include "Utils.h"
#include "data/DataCenter.h"
#include "data/OperationCenter.h"
#include "data/SoundCenter.h"
#include "data/ImageCenter.h"
#include "data/FontCenter.h"
#include "Player.h"
#include "character/Character.h"
#include "object/bed.h"
#include "object/candle.h"
#include "object/closet.h"
#include "object/door.h"
#include "object/floor.h"
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_acodec.h>
#include <algorithm> // 新增 有用到max
#include <vector>
#include <cstring>

// fixed settings
constexpr char game_icon_img_path[] = "./assets/image/game_icon.png";
constexpr char game_start_sound_path[] = "./assets/sound/growl.wav";
constexpr char background_img_path[] = "./assets/image/background.png";
constexpr char background_sound_path[] = "./assets/sound/BackgroundMusic.ogg";

/**
 * @brief Game entry.
 * @details The function processes all allegro events and update the event state to a generic data storage (i.e. DataCenter).
 * For timer event, the game_update and game_draw function will be called if and only if the current is timer.
 */
void
Game::execute() {
    DataCenter *DC = DataCenter::get_instance();
    // main game loop
    bool run = true;
    while(run) {
        // process all events here
        al_wait_for_event(event_queue, &event);
        switch(event.type) {
            case ALLEGRO_EVENT_TIMER: {
                run &= game_update();
                game_draw();
                break;
            } case ALLEGRO_EVENT_DISPLAY_CLOSE: { // stop game
                run = false;
                break;
            } case ALLEGRO_EVENT_KEY_DOWN: {
                DC->key_state[event.keyboard.keycode] = true;
                break;
            } case ALLEGRO_EVENT_KEY_UP: {
                DC->key_state[event.keyboard.keycode] = false;
                break;
            } case ALLEGRO_EVENT_MOUSE_AXES: {
                DC->mouse.x = event.mouse.x;
                DC->mouse.y = event.mouse.y;
                break;
            } case ALLEGRO_EVENT_MOUSE_BUTTON_DOWN: {
                DC->mouse_state[event.mouse.button] = true;
                break;
            } case ALLEGRO_EVENT_MOUSE_BUTTON_UP: {
                DC->mouse_state[event.mouse.button] = false;
                break;
            } default: break;
        }
    }
}

/**
 * @brief Initialize all allegro addons and the game body.
 * @details Only one timer is created since a game and all its data should be processed synchronously.
 */
Game::Game(bool testMode) {
    DataCenter *DC = DataCenter::get_instance();
    GAME_ASSERT(al_init(), "failed to initialize allegro.");

    // initialize allegro addons
    bool addon_init = true;
    addon_init &= al_init_primitives_addon();
    addon_init &= al_init_font_addon();
    addon_init &= al_init_ttf_addon();
    addon_init &= al_init_image_addon();
    addon_init &= al_init_acodec_addon();
    GAME_ASSERT(addon_init, "failed to initialize allegro addons.");

    if(testMode) {
        timer = nullptr;
        event_queue = nullptr;
        display = nullptr;
        debug_log("Game initialized in test mode.\n");
        return;
    }

    // initialize events
    bool event_init = true;
    event_init &= al_install_keyboard();
    event_init &= al_install_mouse();
    event_init &= al_install_audio();
    GAME_ASSERT(event_init, "failed to initialize allegro events.");

    // initialize game body
    GAME_ASSERT(
        timer = al_create_timer(1.0 / DC->FPS),
        "failed to create timer.");
    GAME_ASSERT(
        event_queue = al_create_event_queue(),
        "failed to create event queue.");
    GAME_ASSERT(
        display = al_create_display(DC->window_width, DC->window_height),
        "failed to create display.");

    debug_log("Game initialized.\n");
    game_init();
}

/**
 * @brief Initialize all auxiliary resources.
 */
void
Game::game_init() {
    DataCenter *DC = DataCenter::get_instance();
    SoundCenter *SC = SoundCenter::get_instance();
    ImageCenter *IC = ImageCenter::get_instance();
    FontCenter *FC = FontCenter::get_instance();
    // set window icon
    game_icon = IC->get(game_icon_img_path);
    al_set_display_icon(display, game_icon);

    // register events to event_queue
    al_register_event_source(event_queue, al_get_display_event_source(display));
    al_register_event_source(event_queue, al_get_keyboard_event_source());
    al_register_event_source(event_queue, al_get_mouse_event_source());
    al_register_event_source(event_queue, al_get_timer_event_source(timer));

    // init sound setting
    SC->init();

    // init font setting
    FC->init();

    ui = new UI();
    ui->init();

    // init start screen
    start_screen = new StartScreen();
    start_screen->init();

    // 初始化 end screen
    end_screen = new EndScreen();
    end_screen->init();

    //object init
    DC->character->init();
    DC->floor->init(190, 10,DC->window_width - 150, DC->window_height - 10);
    DC->bed->init();
    DC->candle->init();
    DC->closet->init();
    DC->door->init();
    // game start
    background = IC->get(background_img_path);
    debug_log("Game state: change to START\n");
    state = STATE::START;
    al_start_timer(timer);
}

/**
 * @brief The function processes all data update.
 * @details The behavior of the whole game body is determined by its state.
 * @return Whether the game should keep running (true) or reaches the termination criteria (false).
 * @see Game::STATE
 */

bool
Game::game_update() {
    DataCenter *DC = DataCenter::get_instance();
    OperationCenter *OC = OperationCenter::get_instance();
    SoundCenter *SC = SoundCenter::get_instance();
    static ALLEGRO_SAMPLE_INSTANCE *background = nullptr;

    switch(state) {
        case STATE::START: {
            if(start_screen)
                start_screen->update();
            if(start_screen->isStarted()) {
                debug_log("<Game> state: change to Main\n");
                state = STATE::Main;
                DC->bed->reset_bed_world();
                DC->closet->reset_closet_world();
            }
            break;
        } case STATE::Main: {
            static bool BGM_played = false;
            if(!BGM_played) {
                background = SC->play(background_sound_path, ALLEGRO_PLAYMODE_LOOP);
                BGM_played = true;
            }

            if(DC->key_state[ALLEGRO_KEY_P] && !DC->prev_key_state[ALLEGRO_KEY_P]) {
                SC->toggle_playing(background);
                debug_log("<Game> state: change to PAUSE\n");
                state = STATE::PAUSE;
            }

            
            // 結局判斷
            bool game_over = false;
            EndScreen::EndType final_end_type = EndScreen::EndType::FAIL_END; // 預設失敗

            // Fail End
            if (DC->player->now_fail >= 3) {
                game_over = true;
                final_end_type = EndScreen::EndType::FAIL_END;
            }
            
             // 總學分>128畢業
            if (DC->player->now_point >= 128 && !game_over) {
                game_over = true;
                
                const int love = DC->player->now_love_point;
                const int study = DC->player->now_study_point;
                const int club = DC->player->now_club_point;

                // (Perfect End): 三種學分都大於 30
                if (love >= 30 && study >= 30 && club >= 30) {
                    final_end_type = EndScreen::EndType::PREFECT_END;
                }
                // 其他結局
                else {
                    int max_credit = std::max({love, study, club});

                    if (max_credit == love) {
                        final_end_type = EndScreen::EndType::LOVE_END;
                    } else if (max_credit == club) {
                        final_end_type = EndScreen::EndType::CLUB_END;
                    } else { 
                        final_end_type = EndScreen::EndType::STUDY_END;
                    }
                }
            }
            
            // 執行結局切換
            if (game_over) {
                if (end_screen) {
                    end_screen->set_end_type(final_end_type);
                }
                debug_log("<Game> state: change to END\n");
                state = STATE::END;
            }

                        if(DC->bed->bed_world())
            {
                // 隨機選擇要增加的學分
                int credit_type = rand() % 3; // 0, 1, or 2

                switch (credit_type) {
                    case 0:
                        DC->player->now_love_point += 20;
                        debug_log("<Game> Bed effect: Love Credits +20. New total: %d\n", DC->player->now_love_point);
                        break;
                    case 1:
                        DC->player->now_study_point += 20;
                        debug_log("<Game> Bed effect: Study Credits +20. New total: %d\n", DC->player->now_study_point);
                        break;
                    case 2:
                        DC->player->now_club_point += 20;
                        debug_log("<Game> Bed effect: Club Credits +20. New total: %d\n", DC->player->now_club_point);
                        break;
                }
                
                // 隨後回到主畫面 (不切換 Game State)
                DC->bed->reset_bed_world();

                // 總學分一起更新
                
                DC->player->now_point = DC->player->now_love_point + 
                                        DC->player->now_study_point + 
                                        DC->player->now_club_point;

                
            }

            if(DC->closet->closet_world())
            {
                // 回到初始畫面
                debug_log("<Game> state: change to START\n");
                state = STATE::START;
                start_screen->reset();
            }
            if(DC->door->door_world())
            {
                // 點擊門口時，如果遊戲未結束，則不執行任何操作。
            }
            break;
        } case STATE::PAUSE: {
            if(DC->key_state[ALLEGRO_KEY_P] && !DC->prev_key_state[ALLEGRO_KEY_P]) {
                SC->toggle_playing(background);
                debug_log("<Game> state: change to Main\n");
                state = STATE::Main;
            }
            break;
        } case STATE::END: {
            // 【修改】在 END 狀態下，檢查是否要求重啟
            if (end_screen)
                end_screen->update();
            
            //重設置學分    
            DC->player->reset();
            // 如果 EndScreen 要求重啟 (按下空白鍵)
            if (end_screen && end_screen->is_restart_requested()) {
                debug_log("<Game> state: change to START\n");
                state = STATE::START;
                start_screen->reset();
            }
            // 遊戲循環必須繼續運行，等待空白鍵按下
            return true;
        }
    }
    // If the game is not paused, we should progress update.
    if(state != STATE::PAUSE) {
        DC->player->update();
        SC->update();
        ui->update();
        if(state != STATE::START) {
            DC->character->update();
            OC->update();
        }
    }
    // game_update is finished. The states of current frame will be previous states of the next frame.
    memcpy(DC->prev_key_state, DC->key_state, sizeof(DC->key_state));
    memcpy(DC->prev_mouse_state, DC->mouse_state, sizeof(DC->mouse_state));
    return true;
}




/**
 * @brief Draw the whole game and objects.
 */
void
Game::game_draw() {
    DataCenter *DC = DataCenter::get_instance();
    OperationCenter *OC = OperationCenter::get_instance();
    FontCenter *FC = FontCenter::get_instance();

    // Flush the screen first.
    al_clear_to_color(al_map_rgb(100, 100, 100));
    if(state != STATE::END) {
        // background
        al_draw_bitmap(background, 0, 0, 0);
        /*if(DC->game_field_length < DC->window_width)
            al_draw_filled_rectangle(
                DC->game_field_length, 0,
                DC->window_width, DC->window_height,
                al_map_rgb(100, 100, 100));
        if(DC->game_field_length < DC->window_height)
            al_draw_filled_rectangle(
                0, DC->game_field_length,
                DC->window_width, DC->window_height,
                al_map_rgb(100, 100, 100));*/
        // user interface
        if(state != STATE::START) {
            DC->character->draw();
            DC->bed->draw();
            DC->candle->draw();
            DC->closet->draw();
            DC->door->draw();
            ui->draw();
            OC->draw();
        }
    }
    switch(state) {
        case STATE::START: {
            if(start_screen)
                start_screen->draw();
            break;
        } case STATE::Main: {
            if(!DC->candle->candle_lighted())
                al_draw_filled_rectangle(0, 0, DC->window_width, DC->window_height, al_map_rgba(0, 0, 0, 180));
            break;
        } case STATE::PAUSE: {
            // game layout cover
            al_draw_filled_rectangle(0, 0, DC->window_width, DC->window_height, al_map_rgba(50, 50, 50, 64));
            al_draw_text(
                FC->caviar_dreams[FontSize::LARGE], al_map_rgb(255, 255, 255),
                DC->window_width/2., DC->window_height/2.,
                ALLEGRO_ALIGN_CENTRE, "GAME PAUSED");
            break;
        } case STATE::END: {
            // 【修正】繪製結局畫面和重啟提示
            if (end_screen)
                end_screen->draw();
                
            // 繪製提示文字
            al_draw_text(
                FC->caviar_dreams[FontSize::MEDIUM], al_map_rgb(255, 255, 255),
                DC->window_width / 2., DC->window_height - 50,
                ALLEGRO_ALIGN_CENTRE, "Press SPACE to return to Start Screen");
        }
    }
    al_flip_display();
}




Game::~Game() {
    if(display) al_destroy_display(display);
    if(timer) al_destroy_timer(timer);
    if(event_queue) al_destroy_event_queue(event_queue);
}

