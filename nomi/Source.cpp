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
			//�������
			if(map[zahyo.y / block_size + 1][zahyo.x / block_size + 1] == air){ walk_direction = Left; }
			if(map[zahyo.y / block_size + 1][zahyo.x / block_size - 1] == air){ walk_direction = Right; }
			//�ǐ܂�Ԃ�
			if(map[zahyo.y / block_size][zahyo.x / block_size + 1] != air){ walk_direction = Left; }
			if(map[zahyo.y / block_size][zahyo.x / block_size - 1] != air){ walk_direction = Right; }

			break;
		default:
			break;
	}
}