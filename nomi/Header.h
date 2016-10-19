#pragma once//TODO:必要性

//TODO:ヘッダーも用途ごとに分ける?
//TODO:enemy継承
//TODO:Size strc使えない?

const int map_width = 240;
const int map_height = 66;
const int block_size = 20;

const int view_width = 60;
const int view_height = 20;

const int number_of_stage = 3;

enum LR {
	Left = false,
	Right = true
};

class enemy {
public:
	enum enemy_type {
		enemy_type_hae = 1,
		enemy_type_musi,
		enemy_type_star
	};
	enemy_type type;
	Point zahyo;
	LR walk_direction = Right;
	LR search_floor = Right;
	void walk();
	static void spawn();
	static void draw();
	enemy(Point p, enemy_type t) {
		zahyo = p;
		type = t;
	}


	struct star_st {
		Point zahyo;
		Vec2 v;
	};
private:

};










enum landform {
	landform_air,
	landform_land,
	landform_land2,
	landform_land3,
	landform_land4,
	landform_goal,
	landform_hae,
	landform_musi,
	landform_nomi
};

class mainchara {
	static Point zahyo;
	static LR LRdirection;
	static double vy;
};

struct hadoken {
	Point zahyo;
	LR LRdirection;
};

enum deathcause {
	collision,
	fall
};


extern Texture icontexture;
extern Texture background;
extern Texture haetexture;
extern Texture musitexture;
extern Texture nomitexture;
extern Texture startexture;
extern Texture block1texture;
extern Texture block2texture;
extern Texture block3texture;
extern Texture block4texture;

extern Point mainzahyo;
extern Point goalzahyo;
extern char map[map_height][map_width + 2];
extern LR main_muki;
extern std::vector<enemy>enemy_list;

extern Point window_corner_pos;
extern enemy::star_st stars[3];