#include "T_Floor.h"
#include "T_Hero.h"
#include "../TableWorld/Boss.h"
#include "../scene/TableWorld.h"
#include "../data/DataCenter.h"
#include "../data/GIFCenter.h"
#include "../data/ImageCenter.h"
#include "../shapes/Point.h"
#include "../shapes/Rectangle.h"
#include "../Utils.h"
#include <allegro5/allegro_primitives.h>
using namespace std;

void TFloor::init()
{
    //ImageCenter *IC = ImageCenter::get_instance();
    DataCenter *DC = DataCenter::get_instance();

	//ALLEGRO_BITMAP *img = IC->get("./assets/image/floor.png");

	//設定地板方塊寬高
    // 因為暫時不讀取圖片，直接根據視窗大小和網格數量計算
	width = DC->window_width / 32;
	height = DC->window_height / 18;

    // Initialize map_data to 0 to avoid garbage values
    for(int i=0; i<18; i++) {
        for(int j=0; j<32; j++) {
            map_data[i][j] = 0;
        }
    }
}

void TFloor::load_map(int window)
{
    FILE *data = nullptr;

    // 保留您的選擇邏輯結構，以便未來針對不同關卡讀取不同地圖
    // 注意：TWorldType 的索引是 0, 1, 2，所以我將原本的 1, 2, 3 修正為 0, 1, 2
    if(window == 0) // Program
    {
        data = fopen("assets/map/gamescene_map.txt", "r");
    }
    else if(window == 1) // Calculus
    {
        data = fopen("assets/map/gamescene_map.txt", "r");
    }
    else if(window == 2) // DataStructure
    {
        data = fopen("assets/map/gamescene_map.txt", "r");
    }

    // 錯誤檢查：確保檔案成功開啟
    if (!data) {
        printf("Error: Failed to open map file for window %d\n", window);
        return;
    }

    for (int i = 0; i < 18; i++)
    {
        for (int j = 0; j < 32; j++)
        {
            // 使用 %1d 來讀取單個數字，因為地圖檔中的數字是緊挨著的 (例如 000111)
            if(fscanf(data, "%1d", &map_data[i][j]) != 1) {
                // 讀取錯誤處理
                map_data[i][j] = 0; 
            }
        }
    }
    fclose(data);
}
void TFloor::update()
{
}

void TFloor::interact()
{
    THero* thero = DataCenter::get_instance()->thero;
    if (!thero) return;
    Rectangle *rect = dynamic_cast<Rectangle*>(thero->shape.get());
    if (!rect) return;

    int tile_w = width;
    int tile_h = height;
    DataCenter *DC = DataCenter::get_instance();
    int WIDTH = DC->window_width;
    int HEIGHT = DC->window_height;

    int chara_w = thero->width();
    int chara_h = thero->height();
    double chara_x = rect->x1;
    double chara_y = rect->y1;

    // 1. Map Boundaries (Left/Right)
    if (chara_x < 0)
    {
        // 如果角色處於鎖定狀態（例如進場動畫），允許在左邊界外
        if (!thero->is_input_locked()) {
            rect->update_center_x(rect->center_x() - chara_x);
            chara_x = 0;
        }
    }
    else if (chara_x + chara_w > WIDTH)
    {
        rect->update_center_x(rect->center_x() - (chara_x + chara_w - WIDTH));
        chara_x = WIDTH - chara_w;
    }

    // 2. Map Boundaries (Top)
    if (chara_y < 0)
    {
        rect->update_center_y(rect->center_y() - chara_y);
        chara_y = 0;
        if(thero->gravity.vy < 0) thero->gravity.vy = 0;
    }

    // 3. Floor Collision & Bottom Boundary
    bool on_ground = false;

    // Check tile collision
    int foot_x = chara_x + chara_w / 2;
    int foot_y = chara_y + chara_h;
    int grid_x = foot_x / tile_w;
    int grid_y = foot_y / tile_h;

    if( grid_x >= 0 && grid_x < 32 && grid_y >= 0 && grid_y < 18)
    {
        if(map_data[grid_y][grid_x]) // Solid tile
        {
             // Check if we should ignore collision for this specific row (dropping down)
             bool ignore_collision = false;
             if(thero->drop_start_y != -1.0) {
                 int start_grid_y = (int)(thero->drop_start_y / tile_h);
                 if(grid_y == start_grid_y) {
                     ignore_collision = true;
                 } else {
                     // We have moved past the starting row, so reset the drop logic
                     // This allows landing on the next row
                     thero->drop_start_y = -1.0;
                 }
             }

             if(ignore_collision) {
                 // Do nothing, fall through
             }
             else if(thero->gravity.vy >= 0) // Falling or standing
             {
                 int tile_top = grid_y * tile_h;
                 // Snap to top of tile
                 rect->update_center_y(rect->center_y() - (foot_y - tile_top));
                 chara_y = rect->y1; // Update local var
                 thero->gravity.vy = 0;
                 on_ground = true;
             }
        }
    }

    // Check bottom boundary (if not already on ground)
    if (!on_ground)
    {
        if (chara_y + chara_h > HEIGHT)
        {
            rect->update_center_y(rect->center_y() - (chara_y + chara_h - HEIGHT));
            thero->gravity.vy = 0;
            on_ground = true;
        }
    }

    thero->gravity.on_ground = on_ground;

    // Boss Collision
    Boss* boss = DataCenter::get_instance()->boss;
    if (boss) {
        Rectangle *brect = dynamic_cast<Rectangle*>(boss->shape.get());
        if (brect) {
            int b_w = boss->width();
            int b_h = boss->height();
            double b_x = brect->x1;
            double b_y = brect->y1;

            // 1. Map Boundaries (Left/Right)
            if (b_x < 0) {
                brect->update_center_x(brect->center_x() - b_x);
                b_x = 0;
            } else if (b_x + b_w > WIDTH) {
                brect->update_center_x(brect->center_x() - (b_x + b_w - WIDTH));
                b_x = WIDTH - b_w;
            }

            // 2. Map Boundaries (Top)
            if (b_y < 0) {
                brect->update_center_y(brect->center_y() - b_y);
                b_y = 0;
                if(boss->gravity.vy < 0) boss->gravity.vy = 0;
            }

            // 3. Floor Collision & Bottom Boundary
            bool b_on_ground = false;

            int b_foot_x = b_x + b_w / 2;
            int b_foot_y = b_y + b_h;
            int b_grid_x = b_foot_x / tile_w;
            int b_grid_y = b_foot_y / tile_h;

            if( b_grid_x >= 0 && b_grid_x < 32 && b_grid_y >= 0 && b_grid_y < 18) {
                if(map_data[b_grid_y][b_grid_x]) {
                     if(boss->gravity.vy >= 0) {
                         int tile_top = b_grid_y * tile_h;
                         brect->update_center_y(brect->center_y() - (b_foot_y - tile_top));
                         b_y = brect->y1;
                         boss->gravity.vy = 0;
                         b_on_ground = true;
                     }
                }
            }

            if (!b_on_ground) {
                if (b_y + b_h > HEIGHT) {
                    brect->update_center_y(brect->center_y() - (b_y + b_h - HEIGHT));
                    boss->gravity.vy = 0;
                    b_on_ground = true;
                }
            }
            boss->gravity.on_ground = b_on_ground;
        }
    }
}
void TFloor::draw()
{
    ImageCenter *IC = ImageCenter::get_instance();
    //ALLEGRO_BITMAP *img = IC->get("./assets/image/floor.png");
    DataCenter *DC = DataCenter::get_instance();

    //繪製地板
    for(int i=0;i<18;i++)
    {
        for(int j=0;j<32;j++)
        {
            if(map_data[i][j]==1) //如果是可行走區域
            {
                //al_draw_bitmap(img,j*width,i*height,0);
                al_draw_filled_rectangle(j * width, i * height, (j + 1) * width, (i + 1) * height, al_map_rgb(100, 100, 100));
            }
        }
    }
}
