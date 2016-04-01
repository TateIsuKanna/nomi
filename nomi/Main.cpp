//https://github.com/Reputeless/Siv3D-Reference/blob/master/Examples/SceneManager.md
//シーン管理を使わなければならない!!!!!

#include"Header.h"
Point corner_pos(0, 0);

Point mainzahyo;
Point goalzahyo;
char map[map_height][map_width + 2];//+2は\nとnull文字(マップパーサが簡易だからね)

bool DEBUG_no_gameover;

int current_stage;

legacy::TimerMillisec enemy::star_timer;
unsigned int enemy::shot_time = 0;
enemy::star_st enemy::stars[3];

bool DEBUG_grid;

std::vector<enemy>enemy_list;
LR main_muki = Left;

Texture nomilogo;
Texture icontexture;
Texture gameovertexture;
Texture background;
Texture haetexture;
Texture hadodantexture;
Texture musitexture;
Texture nomitexture;
Texture startexture;
Texture block1texture;
Texture block2texture;
Texture block3texture;
Texture block4texture;


/////////////////////////////
SOCKET sock;
char buf[5];
/////////////////////////////

void Title(){
	Window::Resize(view_width*block_size, view_height*block_size);
	Window::SetTitle(L"蚤");

	enemy_list.clear();
	Rect(0, 0, Window::Size()).draw(Palette::Black);

	String message = L"蚤\nゲームスタート…Zキー\n終了…Esc";
	Font font(50);

	nomilogo = Texture(L"title.png");

	while(System::Update()){

		////////////////////////
		recv(sock, buf, sizeof(buf), 0);
		////////////////////////
		nomilogo.draw();
		if(Input::KeyZ.clicked || buf[4] == 1){
			//HACK:何度も呼び返す事になる(再帰みたいに)
			game_main();
		}

	}
}
void Clear(){
	++current_stage;
	if(current_stage > number_of_stage - 1){
		current_stage = 0;
	}
	Rect(0, 0, Window::Size()).draw(Palette::Black);

	String message = L"ゴール!\nZで次のステージへ";
	Font font(50);
	while(System::Update()){
		font(message).draw();
		if(Input::KeyZ.clicked){
			game_main();
		}
	}
}
void The_end(deathcause died_of){
	if(DEBUG_no_gameover){
		return ;
	}
	
	current_stage = 0;
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
	
	gameovertexture.draw();

	System::Update();


	//HACK:廃止される?
	legacy::TimerMillisec restartTimer;
	restartTimer.start();
	//TODO:Zで抜けるように
	while(restartTimer.elapsed() < 1000){


	}
	Title();
}
void draw(){
	background.scale(3).draw(-mainzahyo.x / 50, 0);

	//map描画
	for(int y = corner_pos.y / block_size; y < corner_pos.y / block_size + view_height + 1; y++){
		for(int x = corner_pos.x / block_size; x < corner_pos.x / block_size + view_width + 1; x++){
			switch(map[y][x]){
				case landform_land:
					block1texture.draw(x * block_size - corner_pos.x, y * block_size - corner_pos.y);
					break;
				case landform_land2:
					block2texture.draw(x * block_size - corner_pos.x, y * block_size - corner_pos.y);
					break;
				case landform_land3:
					block3texture.draw(x * block_size - corner_pos.x, y * block_size - corner_pos.y);
					break;
				case landform_land4:
					block4texture.draw(x * block_size - corner_pos.x, y * block_size - corner_pos.y);
					break;
				case landform_goal:
					Rect(x * block_size - corner_pos.x, y * block_size - corner_pos.y, block_size, block_size).draw(Palette::Red);
					break;
				case landform_air:
					break;
				default:
					Rect(x * block_size - corner_pos.x, y * block_size - corner_pos.y, block_size, block_size).draw(Palette::Blue);
					break;
			}
		}
	}

	enemy::draw();

	//蚤描画
	if(main_muki == Right){
		nomitexture.mirror().draw(mainzahyo - corner_pos);
	} else{
		nomitexture.draw(mainzahyo - corner_pos);
	}

	//デバッグ用グリッド
	if(DEBUG_grid){
		Rect ma((mainzahyo.x - corner_pos.x) / block_size*block_size, (mainzahyo.y - corner_pos.y) / block_size*block_size, block_size);
		ma.drawFrame();
	}
}

bool IsInterger_Position(int pos){
	return (double)pos / block_size - pos / block_size == 0;
}

void game_main(){
	srand((unsigned int)time(NULL));
	srand((unsigned char)'y' + (unsigned char)'y' + (unsigned char)'s' + (unsigned char)'k');
	int score = 0;
	double main_vy = 0;
	const Font font(40);
	bool jump = false;
	bool jump2 = false;

	std::vector<hadoken> hado;

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
	//TODO:ifstream使ってみよう
	FILE *fp;
	fopen_s(&fp, (std::string("stages/map") + std::to_string(current_stage) + std::string(".txt")).c_str(), "r");
	int linep = 0;
	while(fgets(&map[linep][0], map_width + 2, fp))linep += 1;


	for(int y = 0; y < map_height; ++y){
		for(int x = 0; x < map_width; ++x){
			if(map[y][x] == ' '){
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
	for(int y = 0; y < map_height; y++){
		for(int x = 0; x < map_width; x++){
			if(map[y][x] == landform_goal){
				goalzahyo = Point(x*block_size, y*block_size);
			}
			if(map[y][x] == landform_nomi){
				mainzahyo = Point(x * block_size, y * block_size);
				map[y][x] = landform_air;
			}
		}
	}


	enemy::spawn();

	enemy::star_timer.start();

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

		

		//HACK:ゴールが壁扱いなので当たれるように3pixel拡大
		Rect goal_rect(goalzahyo - Point(5, 5), block_size + 10, block_size + 10);
		if(main_rect.intersects(goal_rect)){
			Clear();
		}

		//画面スクロール
		if(mainzahyo.x < view_width / 2 * block_size){
			corner_pos.x = 0;
		} else if(mainzahyo.x >(map_width - view_width / 2)*block_size){
			corner_pos.x = (map_width - view_width)* block_size;
		} else{
			corner_pos.x = mainzahyo.x - (view_width / 2 * block_size);
		}
		if(mainzahyo.y < view_height / 2 * block_size){
			corner_pos.y = 0;
		} else if(mainzahyo.y >(map_height - view_height / 2)*block_size){
			corner_pos.y = (map_height - view_height)* block_size;
		} else{
			corner_pos.y = mainzahyo.y - view_height / 2 * block_size;
		}

		//デバッグのための
		if(Input::KeyControl.clicked){
			mainzahyo = Mouse::Pos() + corner_pos;
			main_vy = 0;
		}
		if(Input::KeyC.clicked && Input::KeyAlt.pressed){
			enemy_list.clear();
		}
		if(Input::KeyG.clicked && Input::KeyAlt.pressed){
			DEBUG_grid = !DEBUG_grid;
		}
		if(Input::KeyE.clicked && Input::KeyAlt.pressed){
			DEBUG_no_gameover = !DEBUG_no_gameover;
		}

		//UNDONE:一見壁抜けしないようになったように見えるが完全ではないと思われる


		////////////////////////
		recv(sock, buf, sizeof(buf), 0);
		////////////////////////

		if(Input::KeyLeft.pressed || buf[0] == 1){
			main_muki = Left;
			if(IsInterger_Position(mainzahyo.x)){
				if(map[mainzahyo.y / block_size][mainzahyo.x / block_size - 1] == landform_air){
					mainzahyo.x -= 2;
					if(Input::KeyShift.pressed)mainzahyo.x -= 2;
				} else{
					mainzahyo.x = mainzahyo.x / block_size * block_size;
				}
			} else{
				mainzahyo.x -= 2;
				if(Input::KeyShift.pressed || buf[3] == 1)mainzahyo.x -= 2;
			}
		}
		if(Input::KeyRight.pressed || buf[1] == 1){
			main_muki = Right;
			if(map[mainzahyo.y / block_size][mainzahyo.x / block_size + 1] == landform_air){
				mainzahyo.x += 2;
				if(Input::KeyShift.pressed || buf[3] == 1)mainzahyo.x += 2;
			} else{
				mainzahyo.x = mainzahyo.x / block_size * block_size;
			}
		}

		font(jump).draw(0, 0);
		font(jump2).draw(0, 100);
		if((Input::KeySpace.clicked || buf[2] == 1) && !jump2){
			if(jump){
				jump2 = true;
			} else{
				jump = true;
			}
			//ジャンプ初速
			if(Input::KeyShift.pressed){
				main_vy = -8;
			} else{
				main_vy = -7;
			}
		}
		if(jump || jump2){
			main_vy += 0.2;//重力加速度積分
			mainzahyo.y += main_vy;//速度積分
			if(main_vy < 0){
				//天井衝突反転
				if(map[mainzahyo.y / block_size][mainzahyo.x / block_size] != landform_air || map[mainzahyo.y / block_size][mainzahyo.x / block_size + 1] != landform_air){
					main_vy *= -1;
					mainzahyo.y = (mainzahyo.y / block_size + 1) * block_size;
				}
			} else{

				//床が来たら止まる
				//UNDONE:もう少し確認
				if(IsInterger_Position(mainzahyo.x)){
					if(map[mainzahyo.y / block_size + 1][mainzahyo.x / block_size] != landform_air){
						main_vy = 0;
						jump = false;
						jump2 = false;
						mainzahyo.y = mainzahyo.y / block_size * block_size;
					}
				} else{
					if(map[mainzahyo.y / block_size + 1][mainzahyo.x / block_size] != landform_air || map[mainzahyo.y / block_size + 1][mainzahyo.x / block_size + 1] != landform_air){
						main_vy = 0;
						jump = false;
						jump2 = false;
						mainzahyo.y = mainzahyo.y / block_size * block_size;
					}
				}
			}
		}

		if(Input::KeyZ.clicked || buf[4] == 1){
			////////////////////
			buf[4] = 0;
			////////////////////
			if(hado.size() < 100){
				hado.push_back({mainzahyo, main_muki});
			}
		}

		//下が床以外ならジャンプ
		if(!jump && main_vy == 0 && map[mainzahyo.y / block_size + 1][mainzahyo.x / block_size] == landform_air && map[mainzahyo.y / block_size + 1][mainzahyo.x / block_size + 1] == landform_air){
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
					if(map[hado[n].zahyo.y / block_size][hado[n].zahyo.x / block_size + 1] != landform_air){
						hado_iterator = hado.erase(hado_iterator + n);
					}
				} else{
					if(map[hado[n].zahyo.y / block_size][hado[n].zahyo.x / block_size + 1] != landform_air || map[hado[n].zahyo.y / block_size + 1][hado[n].zahyo.x / block_size + 1] != landform_air){
						hado_iterator = hado.erase(hado_iterator + n);
					}
				}
			} else{
				if(IsInterger_Position(mainzahyo.y)){
					if(map[hado[n].zahyo.y / block_size][hado[n].zahyo.x / block_size - 1] != landform_air){
						hado_iterator = hado.erase(hado_iterator + n);
					}
				} else{
					if(map[hado[n].zahyo.y / block_size][hado[n].zahyo.x / block_size - 1] != landform_air || map[hado[n].zahyo.y / block_size + 1][hado[n].zahyo.x / block_size - 1] != landform_air){
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
		//波動拳描画移動
		for(int n = 0; n < hado.size(); ++n){
			if(hado[n].LRdirection){
				hadodantexture.draw(hado[n].zahyo - corner_pos);
				hado[n].zahyo.x += 10;
			} else{
				hadodantexture.mirror().draw(hado[n].zahyo - corner_pos);
				hado[n].zahyo.x -= 10;
			}
		}

		//TODO:移動 to draw()
		font(Format(score, L"/", enemy_list.size())).draw();
		font(hado.size()).draw(300, 0);
		//デバッグ表示
		font(mainzahyo).draw(0, 300);
	}
}

int Main(){
	/////////////////////////////
	WSAData wsaData;
	WSAStartup(MAKEWORD(2, 0), &wsaData);

	sock = socket(AF_INET, SOCK_DGRAM, 0);

	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(2002);
	addr.sin_addr.S_un.S_addr = INADDR_ANY;

	bind(sock, (struct sockaddr *)&addr, sizeof(addr));

	u_long val = 1;
	ioctlsocket(sock, FIONBIO, &val);
	/////////////////////////////

	Title();

	return 0;
}