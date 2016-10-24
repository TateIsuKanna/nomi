#include"Header.h"

//HACK:グローバル汚染
Texture hadodantexture;
Texture musitexture;
Texture haetexture;

char map[map_height][map_width + 2];//+2は\nとnull文字(マップパーサが簡易だからね)
Point window_corner_pos;

struct CommonData {
	deathcause died_of;
	Font font{ 50 };
};

using MyApp = SceneManager<String, CommonData>;
class Title : public MyApp::Scene {
public:
	Texture nomilogo = Texture(L"title.png");
	void update() override {
		if (Input::KeyZ.clicked) {
			changeScene(L"Game");
		}
	}

	void draw() const override {
		Window::Resize(view_width*block_size, view_height*block_size);
		Window::SetTitle(L"蚤");

		Rect(0, 0, Window::Size()).draw(Palette::Black);

		String message = L"蚤\nゲームスタート…Zキー\n終了…Esc";
		nomilogo.draw();
	}
};

class Game : public MyApp::Scene {
public:
	Texture icontexture;
	Texture gameovertexture;
	Texture nomitexture;
	Texture background;
	Texture startexture;
	Texture block1texture;
	Texture block2texture;
	Texture block3texture;
	Texture block4texture;
	Font font{ 40 };//TODO:constにする?
	//srand((unsigned int)time(NULL));//TODO:
	int current_stage;
	Point goalzahyo;

	int score = 0;

	double main_vy = 0;
	bool jump = false;
	bool jump2 = false;
	LR main_muki = Left;
	Point mainzahyo;

	std::vector<hadoken> hado;

	std::vector<enemy*>enemy_list;
	enemy::star_st stars[3];

	//流星群到来までの時間☆彡☆彡☆彡
	Stopwatch star_timer;
	unsigned int shot_time = 0;

	void init() override {
		icontexture = Texture(L"thumbnail.png");
		hadodantexture = Texture(L"hadoudan.png");
		haetexture = Texture(L"hae.png");
		gameovertexture = Texture(L"gameover.png");
		musitexture = Texture(L"musi.png");
		nomitexture = Texture(L"nomi.png");
		background = Texture(L"Windmill.png");
		startexture = Texture(L"star.png");
		block1texture = Texture(L"block1.png");
		block2texture = Texture(L"block2.png");
		block3texture = Texture(L"block3.png");
		block4texture = Texture(L"block4.png");
		//マップファイル読み込み
		//TODO:ifstream使う
		FILE *fp;
		fopen_s(&fp, (std::string("stages/map") + std::to_string(current_stage) + std::string(".txt")).c_str(), "r");

		int linep = 0;
		while (fgets(&map[linep][0], map_width + 2, fp))linep += 1;

		for (int y = 0; y < map_height; ++y) {
			for (int x = 0; x < map_width; ++x) {
				if (map[y][x] == ' ') {
					map[y][x] = '0';
				}
				map[y][x] -= '0';
			}
		}
		fclose(fp);


		/*std::ifstream ifs(std::string("stages/map")+ std::to_string(current_stage) + std::string(".txt"));
		while(ifs.getline(map,sizeof(map[][]))){
			std::cout << "[" << str << "]" << std::endl;
		}*/

		//ゴールと蚤の位置をmapから読み取る
		for (int y = 0; y < map_height; y++) {
			for (int x = 0; x < map_width; x++) {
				if (map[y][x] == landform_goal) {
					goalzahyo = Point(x*block_size, y*block_size);
				}
				if (map[y][x] == landform_nomi) {
					mainzahyo = Point(x * block_size, y * block_size);
					map[y][x] = landform_air;
				}
			}
		}

		star_timer.start();
		enemy::spawn(enemy_list, stars);
	}
	bool IsInterger_Position(int pos) {
		return (double)pos / block_size - pos / block_size == 0;
	}
	void update() override {


		Rect main_rect(mainzahyo, block_size, block_size);
		for (int n = 0; n < enemy_list.size(); ++n) {
			enemy_list[n]->walk();
			//蚤と敵との衝突判定
			Rect enemy_rect(enemy_list[n]->zahyo, block_size, block_size);
			if (main_rect.intersects(enemy_rect)) {
				changeScene(L"Result");
			}
		}


		//HACK:ゴールが壁扱いなので当たれるように3pixel拡大
		Rect goal_rect(goalzahyo - Point(5, 5), block_size + 10, block_size + 10);
		if (main_rect.intersects(goal_rect)) {
			changeScene(L"Result");
		}

		//画面スクロール
		if (mainzahyo.x < view_width / 2 * block_size) {
			window_corner_pos.x = 0;
		} else if (mainzahyo.x > (map_width - view_width / 2)*block_size) {
			window_corner_pos.x = (map_width - view_width)* block_size;
		} else {
			window_corner_pos.x = mainzahyo.x - (view_width / 2 * block_size);
		}
		if (mainzahyo.y < view_height / 2 * block_size) {
			window_corner_pos.y = 0;
		} else if (mainzahyo.y > (map_height - view_height / 2)*block_size) {
			window_corner_pos.y = (map_height - view_height)* block_size;
		} else {
			window_corner_pos.y = mainzahyo.y - view_height / 2 * block_size;
		}

		//デバッグのための
		if (Input::KeyControl.clicked) {
			mainzahyo = Mouse::Pos() + window_corner_pos;
			main_vy = 0;
		}
		if (Input::KeyC.clicked && Input::KeyAlt.pressed) {
			enemy_list.clear();
		}

		//UNDONE:一見壁抜けしないようになったように見えるが完全ではないと思われる
		if (Input::KeyLeft.pressed) {// || buf[0] == 1){
			main_muki = Left;
			if (IsInterger_Position(mainzahyo.x)) {
				if (map[mainzahyo.y / block_size][mainzahyo.x / block_size - 1] == landform_air) {
					mainzahyo.x -= 2;
					if (Input::KeyShift.pressed)mainzahyo.x -= 2;
				} else {
					mainzahyo.x = mainzahyo.x / block_size * block_size;
				}
			} else {
				mainzahyo.x -= 2;
				if (Input::KeyShift.pressed) {
					mainzahyo.x -= 2;
				}
			}
		}

		if (Input::KeyRight.pressed) {
			main_muki = Right;
			if (map[mainzahyo.y / block_size][mainzahyo.x / block_size + 1] == landform_air) {
				mainzahyo.x += 2;
				if (Input::KeyShift.pressed) {
					mainzahyo.x += 2;
				}
			} else {
				mainzahyo.x = mainzahyo.x / block_size * block_size;
			}
		}

		font(jump).draw(0, 0);
		font(jump2).draw(0, 100);
		if ((Input::KeySpace.clicked) && !jump2) {
			if (jump) {
				jump2 = true;
			} else {
				jump = true;
			}
			//ジャンプ初速
			if (Input::KeyShift.pressed) {
				main_vy = -8;
			} else {
				main_vy = -7;
			}
		}
		if (jump || jump2) {
			main_vy += 0.2;//重力加速度積分
			mainzahyo.y += main_vy;//速度積分
			if (main_vy < 0) {
				//天井衝突反転
				if (map[mainzahyo.y / block_size][mainzahyo.x / block_size] != landform_air || map[mainzahyo.y / block_size][mainzahyo.x / block_size + 1] != landform_air) {
					main_vy *= -1;
					mainzahyo.y = (mainzahyo.y / block_size + 1) * block_size;
				}
			} else {

				//床が来たら止まる
				//UNDONE:もう少し確認
				if (IsInterger_Position(mainzahyo.x)) {
					if (map[mainzahyo.y / block_size + 1][mainzahyo.x / block_size] != landform_air) {
						main_vy = 0;
						jump = false;
						jump2 = false;
						mainzahyo.y = mainzahyo.y / block_size * block_size;
					}
				} else {
					if (map[mainzahyo.y / block_size + 1][mainzahyo.x / block_size] != landform_air || map[mainzahyo.y / block_size + 1][mainzahyo.x / block_size + 1] != landform_air) {
						main_vy = 0;
						jump = false;
						jump2 = false;
						mainzahyo.y = mainzahyo.y / block_size * block_size;
					}
				}
			}
		}

		if (Input::KeyZ.clicked) {
			if (hado.size() < 100) {
				hado.push_back({ mainzahyo, main_muki });
			}
		}

		//下が床以外ならジャンプ
		if (!jump && main_vy == 0 && map[mainzahyo.y / block_size + 1][mainzahyo.x / block_size] == landform_air && map[mainzahyo.y / block_size + 1][mainzahyo.x / block_size + 1] == landform_air) {
			main_vy = -2;
			jump = true;
		}

		//転落死
		if (mainzahyo.y >= (map_height - 1) * block_size) {
			changeScene(L"Result");
		}

		//波動拳壁衝突
		for (int n = 0; n < hado.size(); ++n) {
			std::vector<hadoken>::iterator hado_iterator = hado.begin();
			if (hado[n].LRdirection) {
				if (IsInterger_Position(mainzahyo.y)) {
					if (map[hado[n].zahyo.y / block_size][hado[n].zahyo.x / block_size + 1] != landform_air) {
						hado_iterator = hado.erase(hado_iterator + n);
					}
				} else {
					if (map[hado[n].zahyo.y / block_size][hado[n].zahyo.x / block_size + 1] != landform_air || map[hado[n].zahyo.y / block_size + 1][hado[n].zahyo.x / block_size + 1] != landform_air) {
						hado_iterator = hado.erase(hado_iterator + n);
					}
				}
			} else {
				if (IsInterger_Position(mainzahyo.y)) {
					if (map[hado[n].zahyo.y / block_size][hado[n].zahyo.x / block_size - 1] != landform_air) {
						hado_iterator = hado.erase(hado_iterator + n);
					}
				} else {
					if (map[hado[n].zahyo.y / block_size][hado[n].zahyo.x / block_size - 1] != landform_air || map[hado[n].zahyo.y / block_size + 1][hado[n].zahyo.x / block_size - 1] != landform_air) {
						hado_iterator = hado.erase(hado_iterator + n);
					}
				}
			}
		}
		//波動拳当たり判定
		//FIXME:vectorの削除は色々とやっかい?
		for (int n = 0; n < hado.size(); ++n) {
			Rect hado_rect(hado[n].zahyo, block_size, block_size);
			for (int i = 0; i < enemy_list.size(); ++i) {
				Rect enemy_rect(enemy_list[i]->zahyo, block_size, block_size);
				if (hado_rect.intersects(enemy_rect)) {
					enemy_list.erase(enemy_list.begin() + i);
					hado.erase(hado.begin() + n);
					score += 1;

				}
			}
		}
		//波動拳移動
		for (int n = 0; n < hado.size(); ++n) {
			if (hado[n].LRdirection) {
				hado[n].zahyo.x += 10;
			} else {
				hado[n].zahyo.x -= 10;
			}
		}
		//流星群移動
		//if (star_timer.ms() > shot_time) {
		//	star_timer.reset();
		//}
		//if (star_timer.isPaused()) {
		//	Rect main_rect(mainzahyo, block_size, block_size);
		//	for (int i = 0; i < 3; ++i) {
		//		//HACK:Pointに対してVec2を足したい
		//		stars[i].zahyo.x += stars[i].v.x;
		//		stars[i].zahyo.y += stars[i].v.y;
		//		Rect star_rect(stars[i].zahyo, block_size, block_size);
		//		if (main_rect.intersects(star_rect)) {
		//			//UNDONE:抜けなければならない
		//		}
		//	}
		//}
	}

	void draw() const override {
		background.scale(3).draw(-mainzahyo.x / 50, 0);

		//map描画
		for (int y = window_corner_pos.y / block_size; y < window_corner_pos.y / block_size + view_height + 1; y++) {
			for (int x = window_corner_pos.x / block_size; x < window_corner_pos.x / block_size + view_width + 1; x++) {
				switch (map[y][x]) {
				case landform_land:
					block1texture.draw(x * block_size - window_corner_pos.x, y * block_size - window_corner_pos.y);
					break;
				case landform_land2:
					block2texture.draw(x * block_size - window_corner_pos.x, y * block_size - window_corner_pos.y);
					break;
				case landform_land3:
					block3texture.draw(x * block_size - window_corner_pos.x, y * block_size - window_corner_pos.y);
					break;
				case landform_land4:
					block4texture.draw(x * block_size - window_corner_pos.x, y * block_size - window_corner_pos.y);
					break;
				case landform_goal:
					Rect(x * block_size - window_corner_pos.x, y * block_size - window_corner_pos.y, block_size, block_size).draw(Palette::Red);
					break;
				case landform_air:
					break;
				default:
					Rect(x * block_size - window_corner_pos.x, y * block_size - window_corner_pos.y, block_size, block_size).draw(Palette::Blue);
					break;
				}
			}
		}


		//蚤描画
		if (main_muki == Right) {
			nomitexture.mirror().draw(mainzahyo - window_corner_pos);
		} else {
			nomitexture.draw(mainzahyo - window_corner_pos);
		}

		//波動拳描画
		for (int i = 0; i < hado.size(); ++i) {
			if (hado[i].LRdirection) {
				hadodantexture.draw(hado[i].zahyo - window_corner_pos);
			} else {
				hadodantexture.mirror().draw(hado[i].zahyo - window_corner_pos);
			}
		}

		//星描画
		for (int i = 0; i < 3; ++i) {
			startexture.draw(stars[i].zahyo);
		}

		//敵描画
		for (int i = 0; i < enemy_list.size(); ++i) {
			enemy_list[i]->draw();
		}

		font(Format(score, L"/", enemy_list.size())).draw();
		//デバッグ表示
		font(mainzahyo).draw(0, 300);
	}
};

class Result : public MyApp::Scene {
public:
	Texture gameovertexture = Texture(L"gameover.png");

	void update() override {
		if (Input::KeyZ.clicked) {
			changeScene(L"Title");
		}
	}

	void draw() const override {
		Rect(0, 0, Window::Size()).draw(Palette::Black);
		String message = L"＿人人人人人人＿\n＞　the end.　＜\n￣Y^Y^Y^Y^Y￣\n\n";
		switch (m_data->died_of) {
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

		gameovertexture.draw();
	}
};

void Clear() {
	//++current_stage;
	//if (current_stage > number_of_stage - 1) {
	//	current_stage = 0;
	//}
	//Rect(0, 0, Window::Size()).draw(Palette::Black);

	//String message = L"ゴール!\nZで次のステージへ";
	//Font font(50);
	//while (System::Update()) {
	//	font(message).draw();
	//	if (Input::KeyZ.clicked) {
	//		game_main();
	//	}
	//}
}

void Main() {
	enemy::musi_texture = Texture(L"musi.png");

	MyApp manager;
	manager.add<Title>(L"Title");
	manager.add<Game>(L"Game");
	manager.add<Result>(L"Result");

	while (System::Update()) {
		manager.updateAndDraw();
	}
}