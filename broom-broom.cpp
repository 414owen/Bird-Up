#include <iostream>
#include <cmath>
#include <ncurses.h>
#include <unistd.h>
#include <vector>
#include <ctime>
#include <cstdlib>

using namespace std;

struct Player {
	int x;
	int y;
};

Player* p = new Player();
int _height, _width;

//void draw() {
//	mvwaddch(stdscr, p->y, p->x, 'p' | COLOR_RED);
//}

class object {
	public:
		char** form;
		int width;
		int height;
		double x;
		double y;
		double dx;
		double dy;
		double ddy;
		double ddx;

		object() {
			width = 1;
			height = 1;
			char* tmp = (char*) malloc(1);
			tmp[0] = '#';
			form = (char**) malloc(sizeof(char*));
			form[0] = tmp;
		}

		void update() {
			x += dx;
			y += dy;
			dy += ddy;
			dx += ddx;
		}

		bool render() {
			update();
			// Whether to remove.
			if (x < 0 || y < 0 || x >=_width || y >= _height) {
				return true;
			}

			for (int i = 0; i < height; i++) {
				char* line = form[i];
				for (int j = 0; j < width; j++) {
					mvwaddch(stdscr, y + i, x + j, line[j]);
				}
			}
			return false;
		}
};

class asteroid: public object {
	public:
		asteroid(double x, double y, double dx, double dy, double ddx, double ddy) {
			this->x = x;
			this->y = y;
			this->dx = dx;
			this->dy = dy;
			this->ddy = ddy;
			this->ddx = ddx;

		}
};

class player : public object {
	public:
		double change = 0.05;
		player() {
			this->height = 3;
			this->width = 6;
			this->form = (char**)malloc(sizeof(char*)*3);
			this->form[0] = (char*)malloc(4);
			this->form[0][0] = '-';
			this->form[0][1] = '-';
			this->form[0][2] = '-';
			this->form[0][3] = ' ';
			this->form[0][4] = ' ';
			this->form[0][5] = ' ';
			this->form[1] = (char*)malloc(4);
			this->form[1][0] = '-';
			this->form[1][1] = '-';
			this->form[1][2] = '-';
			this->form[1][3] = '-';
			this->form[1][4] = '-';
			this->form[1][5] = '>';
			this->form[2] = (char*)malloc(4);
			this->form[2][0] = '-';
			this->form[2][1] = '-';
			this->form[2][2] = '-';
			this->form[2][3] = ' ';
			this->form[2][4] = ' ';
			this->form[2][5] = ' ';
			this->x = 0;
			this->y = _height/2 - this->height/2;
			this->dx = 0;
			this->dy = 0;

		}
		void right() {
			this->dx += change;
		}
		void up() {
			this->dy -= change;
		}
		void left() {
			this->dx -= change;
		}
		void down() {
			this->dy += change;
		}
		//void render() {
		//	update();
		//	mvwaddch(stdscr, y, x, '#');
		//}
};

class Landscape {
	private:

		int dy;

		vector<pair<int, int>> points;
		bool isCeiling;

	public:
		Landscape(){
			points.push_back(make_pair(0,0));
			// points.push_back(make_pair(50,20));
			// points.push_back(make_pair(80,10));
			for (int i = 0; i < _width * 10; i += 20){
				Move(20);
			}
		}
		void Move(int offset){
			for (int i = 0; i < points.size(); i++){
				points[i].first -= offset;
			}

			if (points[points.size()-1].first <= _width + 500){

				int ranx = rand() % 50;
				int rany = rand() % (_height/3);
				ranx += _width;
				points.push_back(make_pair(ranx, rany));
			}

			while (points[0].first < -100){
				points.erase(points.begin());
			}
		}

		int move_interval = 0;
		void Render(bool top){
			move_interval = (move_interval + 1) % 10;
			if (move_interval == 0) {
				Move(1);
			}

			for(int i = 0; i < points.size()-1; i++){
				pair<int, int> p1 = points[i];
				pair<int, int> p2 = points[i+1];

				int len = p2.first - p1.first;
				double slope = (double)(p2.second - p1.second) / (double)len;
				for (int x = p1.first; x < min(p2.first, _width); x++) {

					int currY = (slope * (x-p1.first)) + p1.second;

					for (int y = 0; y < currY; y++) {
						//screen[x][y] = 'X';
						if (top) {
							mvwaddch(stdscr, y, x+70, '.');
						} else {
							mvwaddch(stdscr, _height - y, x+70, '.');
						}
					}
				}
			}
		}
};

int main() {
	initscr();
	// No buffering.
	cbreak();
	noecho();
	// Special chars.
	keypad(stdscr, true);
	// getch() can return ERR.
	nodelay(stdscr, TRUE);
	// No cursor.
	curs_set(0);
	srand(time(NULL));
	getmaxyx(stdscr, _height, _width);

	bool do_run = true;
	vector<object*> objs;
	player* p = new player();
	Landscape *land_top = new Landscape();
	Landscape *land_bot = new Landscape();

	int toSpawn = 1;
	int space = 20;
	while (do_run) {
		for (int i = 0; i < toSpawn; i++) {
			if (rand() % 5 == 0) {
				//printf("hi\n");
				objs.push_back(new asteroid(_width - 2, rand() % _height, -(rand() % 100)*0.001, (rand() % 100 - 50)*0.001, (rand() % 100 - 50)*0.00001, (rand() % 100 - 50)*0.00001));
			}
		}
		if (rand() % 100 == 0) {
			toSpawn++;
		}
		int c;
		while ((c = getch()) != ERR) {
			switch (c) {
				case ' ':
					space = 0;
					break;
				case 'q':
					do_run = false;
					break;
				case 'a':
					p->left();
					break;
				case 'd':
					p->right();
					break;
				case 'w':
					p->up();
					break;
				case 's':
					p->down();
					break;
				case KEY_UP:
					p->up();
					break;
				case KEY_DOWN:
					p->down();
					break;
				case KEY_LEFT:
					p->left();
					break;
				case KEY_RIGHT:
					p->right();
					break;
			}
		}
		erase();

		land_top->Render(true);
		land_bot->Render(false);

		p->render();

		for (int i = 0; i < objs.size(); i++) {
			object* o = objs[i];
			if (o->x < p->x + p->width && o->x >= p->x && o->y < p->y + p->height && o->y >= p->y) {
				return 0;
			}
			if (o->render()) {
				objs.erase(objs.begin() + i);
				i--;
			}
		}

		if (p->x < 0 || p->y < 0 || p->x + p->width > _width || p->y + p->height > _height) {
			return 0;
		}
		if (space++ < 20) {
			for (int i = 0; i < objs.size(); i++) {
				if ((int)objs[i]->y == (int)p->y+1) {
					objs.erase(objs.begin() + i);
					i--;
				}
			}
			for (int i = p->x + p->width + (7 * space);i < p->x + p->width + (7 * space)+ 30; i++) {
				mvwaddch(stdscr, p->y + 1, i, '-');
			}
		}

		//draw();
		wrefresh(stdscr);
		usleep(10000);
	}
}
