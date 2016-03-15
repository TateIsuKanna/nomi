#include<Siv3D.hpp>
#include<stdio.h>
#include<time.h>
#include<math.h>
#include<fstream>
#include<iostream>
#include<string>

//TODO:Size strc使えない?
const int map_width = 120;
const int map_height = 33;
const int block_size = 10;

extern Point mainzahyo;
extern Point goalzahyo;
extern char map[map_height][map_width + 2];//+2は\nとnull文字

void Main2();

enum LR{
	Left = false,
	Right = true
};


class enemy{
public:
	enum enemy_type{
		hae = 1,
		musi,
		star
	};
	enemy_type type;
	Point zahyo;
	LR walk_direction = Right;
	LR search_floor = Right;
	int search = 0;
	void walk();
	enemy(Point p, enemy_type t){
		zahyo = p;
		type = t;
	}

private:

};

enum landform{
	air,
	land,
	land2,
	land3,
	land4,
	goal,
	hae,
	musi,
	nomi
};

class mainchara{
	static Point zahyo;
	static LR LRdirection ;
	static double vy;
};

struct hadoken{
	Point zahyo;
	LR LRdirection;
};

//TODO:start enemyにまとめる
struct star_st{
	Point zahyo;
	Vec2 v;
};
enum deathcause{
	collision,
	fall
};