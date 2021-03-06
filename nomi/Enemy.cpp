﻿#include"Header.h"

Texture enemy::musi_texture;

//TODO:うまくやればまとめられそう
void enemy_hae::draw() {
	if (walk_direction) {
		haetexture.draw(zahyo - window_corner_pos);
	} else {
		haetexture.mirror().draw(zahyo - window_corner_pos);
	}
}
void enemy_musi::draw() {
	if (walk_direction) {
		musi_texture.draw(zahyo - window_corner_pos);
	} else {
		musi_texture.mirror().draw(zahyo - window_corner_pos);
	}
}
void enemy_hae::walk() {
	if (map[zahyo.y / block_size + 1][zahyo.x / block_size] != landform_air) {
		zahyo.y += Random(-3, 0);
	} else if (map[zahyo.y / block_size - 1][zahyo.x / block_size] != landform_air) {
		zahyo.y += Random(0, 3);
	} else {
		zahyo.y += Random(-3, 3);
	}

	if (map[zahyo.y / block_size][zahyo.x / block_size + 1] != landform_air) {
		zahyo.x += Random(-3, 0);
	} else if (map[zahyo.y / block_size][zahyo.x / block_size - 1] != landform_air) {
		zahyo.x += Random(0, 3);
	} else {
		zahyo.x += Random(-3, 3);
	}
}
void enemy_musi::walk() {
	if (walk_direction == Right) {
		zahyo.x += 2;
	} else {
		zahyo.x -= 2;
	}
	//落下回避
	if (map[zahyo.y / block_size + 1][zahyo.x / block_size + 1] == landform_air) {
		walk_direction = Left;
	}
	if (map[zahyo.y / block_size + 1][zahyo.x / block_size - 1] == landform_air) {
		walk_direction = Right;
	}
	//壁折り返し
	if (map[zahyo.y / block_size][zahyo.x / block_size + 1] != landform_air) {
		walk_direction = Left;
	}
	if (map[zahyo.y / block_size][zahyo.x / block_size - 1] != landform_air) {
		walk_direction = Right;
	}
}
void enemy::spawn(std::vector<enemy*> &enemy_list, enemy::star_st stars[]) {
	enemy_list.clear();
	//敵生成
	for (int y = 0; y < map_height; y++) {
		for (int x = 0; x < map_width; x++) {
			switch (map[y][x]) {
			case landform_hae: {
				enemy* tmp = new enemy_hae(Point(x*block_size, y*block_size));
				enemy_list.push_back(tmp);

				map[y][x] = landform_air;
				break;
			}
			case landform_musi: {
				enemy* tmp = new enemy_musi(Point(x*block_size, y*block_size));
				enemy_list.push_back(tmp);
				map[y][x] = landform_air;
				break;
			}
			default:
				break;
			}
		}
	}

	//敵生成星
	for (int n = 0; n < 5; ++n) {
		stars[n].zahyo = Point(rand() % (map_width*block_size), block_size);
		double kakudo = (rand() % 80+90)*(Pi / 180);
		stars[n].v = Vec2(Cos(kakudo), Sin(kakudo));
	}
}