#include<Siv3D.hpp>
#include<stdio.h>
#include<time.h>
#include<math.h>

//TODO:�n�`��enum�쐬
//TODO:main�̍\���̂���(class�̕�����������)

//Size strc�g���Ȃ�?
const int map_width = 120;
const int map_height = 33;
const int block_size = 10;

extern Point mainzahyo;
extern Point goalzahyo;
extern char map[map_height][map_width + 2];//+2��\n��null����

void  Main2();

class enemy{
public:
	int type;
	Point zahyo;
	bool walk_direction = true;
	bool search_floor = true;
	int search = 0;
	void walk();
	enemy(Point p, int t){
		zahyo = p;
		type = t;
	}
	enum type{
		hae = 1,
		musi,
		star
	};
private:

};

class mainchara{
	static Point mainzahyo;
};

struct hadoken{
	Point zahyo;
	bool muki;
};
struct star_st{
	Point zahyo;
	Point v;
};
enum deathcause{
	collision,
	fall
};