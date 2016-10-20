#pragma once//TODO:必要性

//TODO:ヘッダーも用途ごとに分ける

//TODO:Size strc使えない?
const int map_width = 240;
const int map_height = 66;
const int block_size = 20;

const int view_width = 60;
const int view_height = 20;

const int number_of_stage = 3;

enum LR{
	Left = false,
	Right = true
};

class enemy{
public:
	static Texture musi_texture;

	enemy(Point p){
		zahyo = p;
	}

	Point zahyo;
	LR walk_direction = Right;

	enum enemy_type{
		enemy_type_hae = 1,
		enemy_type_musi,
		enemy_type_star
	};
	struct star_st{
		Point zahyo;
		Vec2 v;
	};

	virtual void walk()const{};//HACK:{}これでいいのかあと謎const
	virtual	void draw()const{};//HACK:{}これでいいのかあと謎const
	static void spawn(std::vector<enemy*> &, star_st[],std::vector<enemy> &);
private:

};

class enemy_hae :public enemy{
public:
	void walk();
	void draw();

	enemy_hae(Point p) :enemy(p){}//HACK:これでいいのか
private:

};

class enemy_musi :public enemy{
public:
	LR search_floor = Right;
	void walk();
	void draw();

	enemy_musi(Point p) :enemy(p){}//HACK:これでいいのか
private:

};



enum landform{
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

class mainchara{
	static Point zahyo;
	static LR LRdirection;
	static double vy;
};

struct hadoken{
	Point zahyo;
	LR LRdirection;
};

enum deathcause{
	collision,
	fall
};

extern Texture haetexture;
extern Texture musitexture;

extern char map[map_height][map_width + 2];
extern Point window_corner_pos;