#include "Player.h"


Player::Player() {

}

void
Player::update() {
   
}

void
Player::reset() {
    // 初始化
    this->now_point = 0;
    this->now_love_point = 0;
    this->now_study_point = 0;
    this->now_club_point = 0;
    this->now_fail = 0;
}


/*
namespace PlayerSetting {
	constexpr int init_HP = 3;
	constexpr int init_coin = 100;
	constexpr int coin_freq = 60;
	constexpr int coin_increase = 5;
};

Player::Player() : HP(PlayerSetting::init_HP), coin(PlayerSetting::init_coin) {
	this->coin_freq = PlayerSetting::coin_freq;
	this->coin_increase = PlayerSetting::coin_increase;
	coin_counter = PlayerSetting::coin_freq;
}

void
Player::update() {
	if(coin_counter) --coin_counter;
	else {
		coin += coin_increase;
		coin_counter = coin_freq;
	}
}
*/