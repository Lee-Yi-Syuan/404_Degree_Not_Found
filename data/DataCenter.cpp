#include "DataCenter.h"
#include <cstring>
#include "../Player.h"
#include "../object/bed.h"
#include "../object/floor.h"
#include "../object/candle.h"
#include "../object/closet.h"
#include "../object/door.h"
#include "../object/table.h"
#include "../TableWorld/T_Floor.h"
#include "../TableWorld/T_Hero.h"
#include "../TableWorld/Boss.h"
#include "../Character/Character.h"

// fixed settings
namespace DataSetting {
	constexpr double FPS = 60;
	constexpr int window_width = 1280;
	constexpr int window_height = 720;
}

//這邊是DataCenter的建構子
DataCenter::DataCenter() {
	this->FPS = DataSetting::FPS;
	this->window_width = DataSetting::window_width;
	this->window_height = DataSetting::window_height;
	memset(key_state, false, sizeof(key_state));
	memset(prev_key_state, false, sizeof(prev_key_state));
	mouse = Point(0, 0);
	memset(mouse_state, false, sizeof(mouse_state));
	memset(prev_mouse_state, false, sizeof(prev_mouse_state));


	player = new Player();
	character = new Character();
	bed = new Bed();
	floor = new Floor();
	candle = new Candle();
	closet = new Closet();
	door = new Door();
	table = new Table();
	tfloor = new TFloor();
	thero = new THero();
    boss = new Boss();
}

//這邊是DataCenter的解構子
DataCenter::~DataCenter() {
	delete player;
	delete character;
	delete bed;
	delete floor;
	delete candle;
	delete closet;
	delete door;
	delete table;
	delete tfloor;
	delete thero;
	/*for(Monster *&m : monsters) {
		delete m;
	}*/
	/*for(Tower *&t : towers) {
		delete t;
	}*/
	/*for(Bullet *&b : towerBullets) {
		delete b;
	}*/
}
