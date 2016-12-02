#include <iostream>
#include <clocale>
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

void exit_game() {
	endwin();
	exit(0);
}

class object {
	public:
		vector<string> form;
		int width;
		int height;
		double x;
		double y;
		double dx;
		double dy;

		// object is removed if it touches any of these chars.
		string collide_chars;

		object() {
			collide_chars = "^v.";
			width = 1;
			height = 1;
		}

		void update() {
			x += dx;
			y += dy;
		}

		bool render() {
			update();
			// Whether to remove.
			if (x < 0 || y < 0 || x + width > _width || y + height > _height) {
				return true;
			}

			int i = 0, j = 0;
			for (string row : form){
				for (char c : row){
					if (c != ' '){
						char del_c = mvinch(y + j, x + i);

						if (collide_chars.find(del_c) != string::npos){
							return true;
						}

						mvaddch(y + j, x + i, c);
					}
					i++;
				}
				i = 0; j++;
			}
			return false;
		}
};

class asteroid: public object {
	public:
		asteroid(double x, double y, double dx, double dy) {
			this->x = x;
			this->y = y;
			this->dx = dx;
			this->dy = dy;

			this->form.push_back("#");
			collide_chars += "-";
		}
};

class player : public object {
	public:
		const double change = 0.05;
		player() {
			this->height = 3;
			this->width = 6;

			this->form.push_back("||\\\\");
			this->form.push_back("----->");
			this->form.push_back("||//");

			this->x = 0;
			this->y = _height/2 - this->height/2;
			this->dx = 0;
			this->dy = 0;

			collide_chars = ".^v#";
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
};

class Landscape {
	private:
		vector< pair<int, int> > _points;
		bool _isCeiling;

	public:
		Landscape(bool isCeiling){
			_isCeiling = isCeiling;

			_points.push_back(make_pair(0,0));
			for (int i = 0; i < _width * 10; i += 20){
				Move(20);
			}
		}
		void Move(int offset){
			for (int i = 0; i < _points.size(); i++){
				_points[i].first -= offset;
			}

			if (_points.back().first <= _width + 500){

				int ranx = rand() % 50;
				int rany = rand() % (_height/3);
				ranx += _width;
				_points.push_back(make_pair(ranx, rany));
			}

			while (_points[0].first < -10000){
				_points.erase(_points.begin());
			}
		}

		int move_interval = 0;
		void render(){

			move_interval = (move_interval + 1) % 10;
			if (move_interval == 0) {
				Move(1);
			}

			for(int i = 0; i < _points.size()-1; i++){
				pair<int, int> p1 = _points[i];
				pair<int, int> p2 = _points[i+1];

				int len = p2.first - p1.first;
				double slope = (double)(p2.second - p1.second) / (double)len;
				for (int x = p1.first; x < min(p2.first, _width); x++) {

					int currY = (slope * (x-p1.first)) + p1.second;

					for (int y = 0; y <= currY; y++) {
						char c = '.';
						if (y == currY){
							c = _isCeiling ? 'v' : '^';
						}
						if (_isCeiling) {
							mvwaddch(stdscr, y, x+70, c);
						} else {
							mvwaddch(stdscr, _height - y, x+70, c);
						}
					}
				}
			}
		}
};

int main() {
	setlocale(LC_ALL, "");
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
	Landscape *land_top = new Landscape(true);
	Landscape *land_bot = new Landscape(false);

	int toSpawn = 1;
	int space = 20;
	while (do_run) {
		for (int i = 0; i < toSpawn; i++) {
			if (rand() % 5 == 0) {
				objs.push_back(new asteroid(_width - 2, rand() % _height, -(rand() % 100)*0.0017, (rand() % 100 - 50)*0.001));
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

		land_top->render();
		land_bot->render();

		if (space++ < 20) {
			//			for (int i = 0; i < objs.size(); i++) {
			//				object* o = objs[i];
			//				if ((int)o->y == (int)p->y+1 && o->x > p->x + p->width) {
			//					objs.erase(objs.begin() + i);
			//					i--;
			//				}
			//			}
			for (int i = p->x + p->width + (7 * space);i < p->x + p->width + (7 * space)+ 30; i++) {
				mvwaddch(stdscr, p->y + 1, i, '-');
			}
		}

		for (int i = 0; i < objs.size(); i++) {
			object* o = objs[i];
			if (o->render()) {
				objs.erase(objs.begin() + i);
				i--;
			}
		}

		if (p->render()){
			exit_game();
		}


		wrefresh(stdscr);
		usleep(10000);
	}
}

