#include"Header.h"

void enemy::walk(){
	switch(type){
		case hae:
			if(map[zahyo.y / block_size + 1][zahyo.x / block_size] != '0'){
				zahyo.y += Random(-3, 0);
			} else if(map[zahyo.y / block_size - 1][zahyo.x / block_size] != '0'){
				zahyo.y += Random(0, 3);
			} else{
				zahyo.y += Random(-3, 3);
			}

			if(map[zahyo.y / block_size][zahyo.x / block_size + 1] != '0'){
				zahyo.x += Random(-3, 0);
			} else if(map[zahyo.y / block_size][zahyo.x / block_size - 1] != '0'){
				zahyo.x += Random(0, 3);
			} else{
				zahyo.x += Random(-3, 3);
			}

			break;
		case musi:
			if(walk_direction == Right){
				zahyo.x += 1;
			} else{
				zahyo.x -= 1;
			}
			//落下回避
			if(map[zahyo.y / block_size + 1][zahyo.x / block_size + 1] == air){ walk_direction = Left; }
			if(map[zahyo.y / block_size + 1][zahyo.x / block_size - 1] == air){ walk_direction = Right; }
			//壁折り返し
			if(map[zahyo.y / block_size][zahyo.x / block_size + 1] != air){ walk_direction = Left; }
			if(map[zahyo.y / block_size][zahyo.x / block_size - 1] != air){ walk_direction = Right; }

			break;
		default:
			break;
	}
	//流星群移動
	if(star_timer.elapsed() > shot_time){
		star_timer.reset();
	}
	if(star_timer.isPaused){
		Rect main_rect(mainzahyo, block_size, block_size);
		for(int i = 0; i < 3; ++i){
			//HACK:Pointに対してVec2を足したい
			stars[i].zahyo.x += stars[i].v.x;
			stars[i].zahyo.y += stars[i].v.y;
			Rect star_rect(stars[i].zahyo, block_size, block_size);
			if(main_rect.intersects(star_rect)){
				The_end(collision);
			}
		}
	}
}

//TODO:一固体でなく全部を描くのは不自然かもしれない．
void enemy::draw(){
	for(int n = 0; n < enemy_list.size(); ++n){
		switch(enemy_list[n].type){
			case enemy::enemy_type::hae:
				if(enemy_list[n].walk_direction){
					haetexture.draw(enemy_list[n].zahyo);
				} else{
					haetexture.mirror().draw(enemy_list[n].zahyo);
				}
				break;
			case enemy::enemy_type::musi:
				if(enemy_list[n].walk_direction){
					musitexture.draw(enemy_list[n].zahyo);
				} else{
					musitexture.mirror().draw(enemy_list[n].zahyo);
				}
				break;
			default:
				break;
		}
	}
	//星描画
	for(int n = 0; n < 3; ++n){
		startexture.draw(stars[n].zahyo);
	}
}
void enemy::spawn(){
	//敵生成蝿
	for(int y = 0; y < map_height; y++){
		for(int x = 0; x < map_width; x++){
			if(map[y][x] == hae){
				enemy_list.push_back(enemy(Point(x*block_size, y*block_size), enemy::enemy_type::hae));
				map[y][x] = air;
			}
		}
	}

	//敵生成虫
	for(int y = 0; y < map_height; y++){
		for(int x = 0; x < map_width; x++){
			if(map[y][x] == musi){
				enemy_list.push_back(enemy(Point(x*block_size, y*block_size), enemy::enemy_type::musi));
				map[y][x] = air;
			}
		}
	}

	//敵生成星
	for(int n = 0; n < 3; ++n){
		stars[n].zahyo = Point(rand() % (map_width*block_size), 0);
		double kakudo = (rand() % 120 + 30)*(Pi / 180);
		stars[n].v = Vec2(Cos(kakudo) * 6, Sin(kakudo) * 6);
	}
}

