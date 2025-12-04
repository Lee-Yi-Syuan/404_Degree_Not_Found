#ifndef PLAYER_H_INCLUDED
#define PLAYER_H_INCLUDED


class Player
{
public:
    Player();
    void update();
    void reset();//新增的
   
    int now_point = 0;      // 總學分
    int now_love_point = 0; // 戀愛學分
    int now_study_point = 0;// 課業學分
    int now_club_point = 0; // 社團學分
    int now_fail = 0;       // 不及格科目


private:

};





/*class Player
{
public:
	Player();
	void update();
	int HP;
	int coin;
private:
	int coin_freq;
	int coin_increase;
	int coin_counter;
};
*/
#endif
