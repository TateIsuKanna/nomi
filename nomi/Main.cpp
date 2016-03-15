#include"Header.h"
#include<HamFramework.hpp>
#include<windows.h>

Point mainzahyo;
Point goalzahyo;
char map[map_height][map_width + 2];//+2は\nとnull文字(マップパーサが簡易だからね)

bool DEBUG_grid;

star_st star[3];
std::vector<enemy>enemy_list;
LR main_muki = Left;

Texture icontexture;
Texture background;
Texture haetexture;
Texture musitexture;
Texture nomitexture;
Texture startexture;
Texture block1texture;
Texture block2texture;
Texture block3texture;
Texture block4texture;

void Title(){
	Window::Resize(1190, 500);
	Window::SetTitle(L"蚤");

	enemy_list.clear();
	Rect(0, 0, Window::Size()).draw(Palette::Black);

	String message = L"蚤\nゲームスタート…Zキー\n終了…Esc";
	Font font(50);
	while(System::Update()){
		font(message).draw();
		if(Input::KeyZ.clicked){
			Main2();
		}
	}
}
void Clear(){
	Rect(0, 0, Window::Size()).draw(Palette::Black);

	String message = L"ゴール!\nZでタイトルへ";
	Font font(50);
	while(System::Update()){
		font(message).draw();
		if(Input::KeyZ.clicked){
			Title();
		}
	}
}
void The_end(deathcause died_of){
	Rect(0, 0, Window::Size()).draw(Palette::Black);
	String message = L"＿人人人人人人＿\n＞　the end.　＜\n￣Y^Y^Y^Y^Y￣\n\n";
	switch(died_of){
		case deathcause::collision:
			message += L"死因:衝突";
			break;
		case deathcause::fall:
			message += L"死因:転落";
			break;
		default:
			message += L"不正な死に方をしました．";
			break;
	}
	//TODO:どっちが好みか微妙(ここ以外にもあり)
	//Font(40)(message).draw();
	Font(40).draw(message);
	System::Update();

	//HACK:廃止される?
	legacy::TimerMillisec restartTimer;
	restartTimer.start();
	//TODO:Zで抜けるように
	while(restartTimer.elapsed() < 2000){}
	Title();
}
void draw(){
	background.scale(3).draw();

	//map描画
	for(int y = 0; y < map_height; y++){
		for(int x = 0; x < map_width; x++){
			switch(map[y][x]){
				case land:
					block1texture.draw(x * block_size, y * block_size);
					break;
				case land2:
					block2texture.draw(x * block_size, y * block_size);
					break;
				case land3:
					block3texture.draw(x * block_size, y * block_size);
					break;
				case land4:
					block4texture.draw(x * block_size, y * block_size);
					break;
				case goal:
					Rect(x * block_size, y * block_size, block_size, block_size).draw(Palette::Red);
					break;
			}
		}
	}

	//蚤描画
	if(main_muki == Right){
		nomitexture.mirror().draw(mainzahyo);
	} else{
		nomitexture.draw(mainzahyo);
	}

	//蝿虫描画
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
		startexture.draw(star[n].zahyo);
	}

	//デバッグ用グリッド
	if(DEBUG_grid){
		Rect ma(mainzahyo.x / block_size*block_size, mainzahyo.y / block_size*block_size, block_size);
		ma.drawFrame();
	}
}

bool IsInterger_Position(int pos){
	return (double)pos / block_size - pos / block_size == 0;
}

void Main2(){
	srand((unsigned int)time(NULL));
	int score = 0;
	double main_vy = 0;
	const Font font(40);
	bool jump = false;
	std::vector<hadoken> hado;

	icontexture = Texture(L"thumbnail.png");
	haetexture = Texture(L"hae.png");
	musitexture = Texture(L"musi.png");
	nomitexture = Texture(L"nomi.png");
	background = Texture(L"Windmill.png");
	startexture = Texture(L"star.png");
	block1texture = Texture(L"block1.png");
	block2texture = Texture(L"block2.png");
	block3texture = Texture(L"block3.png");
	block4texture = Texture(L"block4.png");


	//マップファイル読み込み
	//TODO:ifstream使ってみよう
	FILE *fp;
	fopen_s(&fp, "map.txt", "r");
	int linep = 0;
	while(fgets(&map[linep][0], map_width + 2, fp))linep += 1;
	for(int y = 0; y < map_height; ++y){
		for(int x = 0; x < map_width; ++x){
			map[y][x] -= '0';
		}
	}

	//ゴールと蚤の位置をmapから読み取る
	for(int y = 0; y < map_height; y++){
		for(int x = 0; x < map_width; x++){
			if(map[y][x] == goal){
				goalzahyo = Point(x * block_size, y * block_size);
			}
			if(map[y][x] == nomi){
				mainzahyo = Point(x * block_size, y * block_size);
			}
		}
	}

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
		star[n].zahyo = Point(rand() % (map_width*block_size), 0);
		double kakudo = (rand() % 120 + 30)*(Pi / 180);
		star[n].v = Vec2(Cos(kakudo) * 6, Sin(kakudo) * 6);
	}

	//流星群到来までのタイマー☆彡☆彡☆彡
	unsigned int shot_time = 0;
	//HACK:廃止される?
	legacy::TimerMillisec star_timer;
	star_timer.start();

	while(System::Update()){
		Rect main_rect(mainzahyo, block_size, block_size);
		for(int n = 0; n < enemy_list.size(); ++n){
			enemy_list[n].walk();
			//蚤と敵との衝突判定
			Rect enemy_rect(enemy_list[n].zahyo, block_size, block_size);
			if(main_rect.intersects(enemy_rect)){
				The_end(collision);
			}
		}

		draw();

		//ゴール
		//HACK:ゴールが壁扱いなので当たれるように1pixel拡大
		Rect goal_rect(goalzahyo - Point(1, 1), block_size + 2, block_size + 2);
		if(main_rect.intersects(goal_rect)){
			Clear();
		}

		//デバッグのための
		if(Input::KeyControl.pressed){
			mainzahyo = Mouse::Pos();
			main_vy = 0;
		}
		if(Input::KeyA.pressed){
			hado.push_back({mainzahyo, main_muki});
		}
		if(Input::KeyC.clicked && Input::KeyAlt.pressed){
			enemy_list.clear();
		}
		if(Input::KeyG.clicked && Input::KeyAlt.pressed){
			DEBUG_grid = !DEBUG_grid;
		}


		//UNDONE:一見壁抜けしないようになったように見えるが完全ではないと思われる
		if(Input::KeyLeft.pressed){
			main_muki = Left;
			if(IsInterger_Position(mainzahyo.x)){
				if(map[mainzahyo.y / block_size][mainzahyo.x / block_size - 1] == air){
					mainzahyo.x -= 2;
					if(Input::KeyShift.pressed)mainzahyo.x -= 2;
				} else{
					mainzahyo.x = mainzahyo.x / block_size * block_size;
				}
			} else{
				mainzahyo.x -= 2;
				if(Input::KeyShift.pressed)mainzahyo.x -= 2;
			}
		}
		if(Input::KeyRight.pressed){
			main_muki = Right;
			if(map[mainzahyo.y / block_size][mainzahyo.x / block_size + 1] == air){
				mainzahyo.x += 2;
				if(Input::KeyShift.pressed)mainzahyo.x += 2;
			} else{
				mainzahyo.x = mainzahyo.x / block_size * block_size;
			}
		}


		if(Input::KeySpace.pressed && !jump){
			jump = true;
			//ジャンプ初速
			if(Input::KeyShift.pressed){
				main_vy = -4;
			} else{
				main_vy = -3;
			}
		}
		if(jump){
			main_vy += 0.1;//重力加速度積分
			mainzahyo.y += main_vy;//速度積分
			if(main_vy < 0){
				//天井衝突反転
				if(map[mainzahyo.y / block_size][mainzahyo.x / block_size] != air || map[mainzahyo.y / block_size][mainzahyo.x / block_size + 1] != air){
					OutputDebugString(L"天井衝突反転");
					main_vy *= -1;
					mainzahyo.y = (mainzahyo.y / block_size + 1) * block_size;
				}
			} else{

				//床が来たら止まる
				//UNDONE:もう少し確認
				if(IsInterger_Position(mainzahyo.x)){
					OutputDebugString(L"t");
					if(map[mainzahyo.y / block_size + 1][mainzahyo.x / block_size] != air){
						main_vy = 0;
						jump = false;
						mainzahyo.y = mainzahyo.y / block_size * block_size;
					}
				} else{
					OutputDebugString(L"f");
					if(map[mainzahyo.y / block_size + 1][mainzahyo.x / block_size] != air || map[mainzahyo.y / block_size + 1][mainzahyo.x / block_size + 1] != air){
						main_vy = 0;
						jump = false;
						mainzahyo.y = mainzahyo.y / block_size * block_size;
					}
				}
			}
		}

		if(Input::KeyZ.clicked){
			if(hado.size() < 100){
				hado.push_back({mainzahyo, main_muki});
			}
		}

		//下が床以外ならジャンプ
		if(!jump && main_vy == 0 && map[mainzahyo.y / block_size + 1][mainzahyo.x / block_size] == air && map[mainzahyo.y / block_size + 1][mainzahyo.x / block_size + 1] == air){
			main_vy = -2;
			jump = true;
		}

		//転落死
		if(mainzahyo.y >= (map_height - 1) * block_size){
			The_end(fall);
		}

		//波動拳壁衝突
		for(int n = 0; n < hado.size(); ++n){
			std::vector<hadoken>::iterator hado_iterator = hado.begin();
			if(hado[n].LRdirection){
				if(IsInterger_Position(mainzahyo.y)){
					if(map[hado[n].zahyo.y / block_size][hado[n].zahyo.x / block_size + 1] != air){
						hado_iterator = hado.erase(hado_iterator + n);
					}
				} else{
					if(map[hado[n].zahyo.y / block_size][hado[n].zahyo.x / block_size + 1] != air || map[hado[n].zahyo.y / block_size + 1][hado[n].zahyo.x / block_size + 1] != air){
						hado_iterator = hado.erase(hado_iterator + n);
					}
				}
			} else{
				if(IsInterger_Position(mainzahyo.y)){
					if(map[hado[n].zahyo.y / block_size][hado[n].zahyo.x / block_size - 1] != air){
						hado_iterator = hado.erase(hado_iterator + n);
					}
				} else{
					if(map[hado[n].zahyo.y / block_size][hado[n].zahyo.x / block_size - 1] != air || map[hado[n].zahyo.y / block_size + 1][hado[n].zahyo.x / block_size - 1] != air){
						hado_iterator = hado.erase(hado_iterator + n);
					}
				}
			}
		}
		//波動拳当たり判定
		for(int n = 0; n < hado.size(); ++n){
			Rect hado_rect(hado[n].zahyo, block_size, block_size);
			for(int i = 0; i < enemy_list.size(); ++i){
				Rect enemy_rect(enemy_list[i].zahyo, block_size, block_size);
				if(hado_rect.intersects(enemy_rect)){
					enemy_list.erase(enemy_list.begin() + i);
					hado.erase(hado.begin() + n);
					score += 1;
				}
			}
		}
		//波動拳移動
		for(int n = 0; n < hado.size(); ++n){
			Rect(hado[n].zahyo, block_size, block_size).draw(Palette::Aliceblue);
			if(hado[n].LRdirection){
				hado[n].zahyo.x += 5;
			} else{
				hado[n].zahyo.x -= 5;
			}
		}

		//TODO:移動 to draw()
		font(Format(score, L"/", enemy_list.size())).draw();
		font(hado.size()).draw(300, 0);
		//デバッグ表示
		font(mainzahyo).draw(0, 300);

		//流星群移動
		if(star_timer.elapsed() > shot_time){
			star_timer.pause();
		}
		if(star_timer.isPaused){
			for(int i = 0; i < 3; ++i){
				//HACK:Pointに対してVec2を足したい
				star[i].zahyo.x += star[i].v.x;
				star[i].zahyo.y += star[i].v.y;
				Rect star_rect(star[i].zahyo, block_size, block_size);
				if(main_rect.intersects(star_rect)){
					The_end(collision);
				}
			}
		}
	}
}

//HACK:Main&Main2
void Main(){
	Title();
}