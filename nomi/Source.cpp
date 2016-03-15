#include"Header.h"

void enemy::walk(){
	switch(type){
		case enemy::type::hae:
			if(map[zahyo.y / block_size + 1][zahyo.x / block_size] != '0'){
				zahyo.y += Random(-3, 0);
			} else if(map[zahyo.y / block_size- 1][zahyo.x / block_size] != '0'){
				zahyo.y += Random(0, 3);
			} else{
				zahyo.y += Random(-3, 3);
			}

			if(map[zahyo.y / block_size][zahyo.x / block_size + 1] != '0'){
				zahyo.x += Random(-3, 0);
			} else if(map[zahyo.y / block_size][zahyo.x / block_size- 1] != '0'){
				zahyo.x += Random(0, 3);
			} else{
				zahyo.x += Random(-3, 3);
			}

			break;
		case enemy::type::musi:
			if(walk_direction){
				zahyo.x += 1;
			} else{
				zahyo.x -= 1;
			}
			//óéâ∫âÒî
			if(map[zahyo.y / block_size + 1][zahyo.x / block_size + 1] == '0'){ walk_direction = false; }
			if(map[zahyo.y / block_size + 1][zahyo.x / block_size - 1] == '0'){ walk_direction = true; }
			//ï«ê‹ÇËï‘Çµ
			if(map[zahyo.y / block_size][zahyo.x / block_size + 1] != '0'){ walk_direction = false; }
			if(map[zahyo.y / block_size][zahyo.x / block_size - 1] != '0'){ walk_direction = true; }
		default:
			break;
	}
}