#include "OperationCenter.h"
#include "DataCenter.h"
#include "../object/bed.h"
#include "../object/candle.h"
#include "../object/closet.h"
#include "../object/door.h"
#include "../object/table.h"
#include "../Character/Character.h"
//#include "../monsters/Monster.h"
//#include "../towers/Tower.h"
//#include "../towers/Bullet.h"
#include "../Player.h"


// Helper function for distance squared
static double get_dist_sq(Shape* s1, Shape* s2) {
    if (!s1 || !s2) return 1e18;
    double dx = s1->center_x() - s2->center_x();
    double dy = s1->center_y() - s2->center_y();
    return dx*dx + dy*dy;
}

//這邊主要用來統一更新遊戲內物件互動的function
void OperationCenter::update() {
	DataCenter *DC = DataCenter::get_instance();
    
    bool bed_touch = DC->bed->is_player_touching();
    bool candle_touch = DC->candle->is_player_touching();
    bool closet_touch = DC->closet->is_player_touching();
    bool door_touch = DC->door->is_player_touching();
    bool table_touch = DC->table->is_player_touching();
    
    double min_dist = 1e18;
    int active_id = 0; // 0: None, 1: Bed, 2: Candle, 3: Closet, 4: Door, 5: Table
    
    Shape* char_shape = DC->character->shape.get();

    if (bed_touch) {
        double d = get_dist_sq(char_shape, DC->bed->shape.get());
        if (d < min_dist) { min_dist = d; active_id = 1; }
    }
    if (candle_touch) {
        double d = get_dist_sq(char_shape, DC->candle->shape.get());
        if (d < min_dist) { min_dist = d; active_id = 2; }
    }
    if (closet_touch) {
        double d = get_dist_sq(char_shape, DC->closet->shape.get());
        if (d < min_dist) { min_dist = d; active_id = 3; }
    }
    if (door_touch) {
        double d = get_dist_sq(char_shape, DC->door->shape.get());
        if (d < min_dist) { min_dist = d; active_id = 4; }
    }
    if (table_touch) {
        double d = get_dist_sq(char_shape, DC->table->shape.get());
        if (d < min_dist) { min_dist = d; active_id = 5; }
    }

	_update_character_bed(active_id == 1);
	_update_candle_character(active_id == 2);
	_update_closet_character(active_id == 3);
	_update_door_character(active_id == 4);
	_update_table_character(active_id == 5);
	// Update monsters.
	//_update_monster();
	// Update towers.
	//_update_tower();
	// Update tower bullets.
	//_update_towerBullet();
	// If any bullet overlaps with any monster, we delete the bullet, reduce the HP of the monster, and delete the monster if necessary.
	//_update_monster_towerBullet();
	// If any monster reaches the end, hurt the player and delete the monster.
	//_update_monster_player();
}
void OperationCenter::_update_character_bed(bool enabled) {
	DataCenter *DC = DataCenter::get_instance();
	Bed* bed=DC->bed;
	bed->interact(enabled);
}
void OperationCenter::_update_candle_character(bool enabled) {
	DataCenter *DC = DataCenter::get_instance();
	Candle* candle=DC->candle;
	candle->interact(enabled);
}
void OperationCenter::_update_closet_character(bool enabled) {
	DataCenter *DC = DataCenter::get_instance();
	Closet* closet=DC->closet;
	closet->interact(enabled);
}
void OperationCenter::_update_door_character(bool enabled) {
	DataCenter *DC = DataCenter::get_instance();
	Door* door=DC->door;
	door->interact(enabled);
}
void OperationCenter::_update_table_character(bool enabled) {
	DataCenter *DC = DataCenter::get_instance();
	Table* table=DC->table;
	table->interact(enabled);
}
/*void OperationCenter::_update_monster() {
	std::vector<Monster*> &monsters = DataCenter::get_instance()->monsters;
	for(Monster *monster : monsters)
		monster->update();
}*/

/*void OperationCenter::_update_tower() {
	std::vector<Tower*> &towers = DataCenter::get_instance()->towers;
	for(Tower *tower : towers)
		tower->update();
}*/

/*void OperationCenter::_update_towerBullet() {
	std::vector<Bullet*> &towerBullets = DataCenter::get_instance()->towerBullets;
	for(Bullet *towerBullet : towerBullets)
		towerBullet->update();
	// Detect if a bullet flies too far (exceeds its fly distance limit), which means the bullet lifecycle has ended.
	for(size_t i = 0; i < towerBullets.size(); ++i) {
		if(towerBullets[i]->get_fly_dist() <= 0) {
			delete towerBullets[i];
			towerBullets.erase(towerBullets.begin() + i);
			--i;
		}
	}
}*/

/*void OperationCenter::_update_monster_towerBullet() {
	DataCenter *DC = DataCenter::get_instance();
	std::vector<Monster*> &monsters = DC->monsters;
	std::vector<Bullet*> &towerBullets = DC->towerBullets;
	for(size_t i = 0; i < monsters.size(); ++i) {
		for(size_t j = 0; j < towerBullets.size(); ++j) {
			// Check if the bullet overlaps with the monster.
			if(monsters[i]->shape->overlap(*(towerBullets[j]->shape))) {
				// Reduce the HP of the monster. Delete the bullet.
				monsters[i]->HP -= towerBullets[j]->get_dmg();
				delete towerBullets[j];
				towerBullets.erase(towerBullets.begin() + j);
				--j;
			}
		}
	}
}*/

/*void OperationCenter::_update_monster_player() {
	DataCenter *DC = DataCenter::get_instance();
	std::vector<Monster*> &monsters = DC->monsters;
	Player *&player = DC->player;
	for(size_t i = 0; i < monsters.size(); ++i) {
		// Check if the monster is killed.
		if(monsters[i]->HP <= 0) {
			// Monster gets killed. Player receives money.
			player->coin += monsters[i]->get_money();
			delete monsters[i];
			monsters.erase(monsters.begin() + i);
			--i;
			// Since the current monsster is killed, we can directly proceed to next monster.
			break;
		}
		// Check if the monster reaches the end.
		if(monsters[i]->get_path().empty()) {
			delete monsters[i];
			monsters.erase(monsters.begin() + i);
			player->HP--;
			--i;
		}
	}
}*/

void OperationCenter::draw() {
	//_draw_monster();
	//_draw_tower();
	//_draw_towerBullet();
}

/*void OperationCenter::_draw_monster() {
	std::vector<Monster*> &monsters = DataCenter::get_instance()->monsters;
	for(Monster *monster : monsters)
		monster->draw();
}*/

/*void OperationCenter::_draw_tower() {
	std::vector<Tower*> &towers = DataCenter::get_instance()->towers;
	for(Tower *tower : towers)
		tower->draw();
}*/

/*void OperationCenter::_draw_towerBullet() {
	std::vector<Bullet*> &towerBullets = DataCenter::get_instance()->towerBullets;
	for(Bullet *towerBullet : towerBullets)
		towerBullet->draw();
}*/
