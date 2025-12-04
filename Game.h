#ifndef GAME_H_INCLUDED
#define GAME_H_INCLUDED

#include <allegro5/allegro.h>
#include "UI.h"
#include "scene/Start.h"
#include "scene/End.h" // 新加的

/**
 * @brief Main class that runs the whole game.
 * @details All game procedures must be processed through this class.
 */
class Game
{
public:
    void execute();
public:
    Game(bool testMode = false);
    ~Game();
    void game_init();
    bool game_update();
    void game_draw();
private:
    /**
     * @brief States of the game process in game_update.
     * @see Game::game_update()
     */
    enum class STATE {
        START, // -> Main
        Main,  // -> START, PAUSE, END
        PAUSE, // -> Main
        END    // -> START
    };
    STATE state;
    ALLEGRO_EVENT event;
    ALLEGRO_BITMAP *game_icon;
    ALLEGRO_BITMAP *background;
private:
    ALLEGRO_DISPLAY *display;
    ALLEGRO_TIMER *timer;
    ALLEGRO_EVENT_QUEUE *event_queue;
    UI *ui;
    StartScreen* start_screen;
    EndScreen* end_screen; //結局
};

#endif

