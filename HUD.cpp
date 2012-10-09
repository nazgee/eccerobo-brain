/*
 * HUD.cpp
 *
 *  Created on: Oct 9, 2012
 *      Author: nazgee
 */

#include <limits>
#include <sstream>

#include "HUD.h"
#include <curses.h>

namespace ecce {

WINDOW *create_newwin(int height, int width, int starty, int startx)
{	WINDOW *local_win;

	local_win = newwin(height, width, starty, startx);
	box(local_win, 0 , 0);		/* 0, 0 gives default characters
					 * for the vertical and horizontal
					 * lines			*/
	wrefresh(local_win);		/* Show that box 		*/

	return local_win;
}

void destroy_win(WINDOW *local_win)
{
	/* box(local_win, ' ', ' '); : This won't produce the desired
	 * result of erasing the window. It will leave it's four corners
	 * and so an ugly remnant of window.
	 */
	wborder(local_win, ' ', ' ', ' ',' ',' ',' ',' ',' ');
	/* The parameters taken are
	 * 1. win: the window on which to operate
	 * 2. ls: character to be used for the left side of the window
	 * 3. rs: character to be used for the right side of the window
	 * 4. ts: character to be used for the top side of the window
	 * 5. bs: character to be used for the bottom side of the window
	 * 6. tl: character to be used for the top left corner of the window
	 * 7. tr: character to be used for the top right corner of the window
	 * 8. bl: character to be used for the bottom left corner of the window
	 * 9. br: character to be used for the bottom right corner of the window
	 */
	wrefresh(local_win);
	delwin(local_win);
}

HUD::HUD(Spine* pSpine, Eye* pEye) :
	spine(pSpine),
	eye(pEye),
	no_press(0),
	speed(0),
	turn(0), mTextWindow(0) {

}

HUD::~HUD() {
	destroy_win(mTextWindow);
}

void HUD::setSpeed(int s) {
	// sanitize speed
	s = std::min(s, 4);
	s = std::max(s, -4);
	speed = s;

	std::stringstream ss;
	ss << s;
	spine->set("set speed " + ss.str());
}

void HUD::setTurn(int t) {
	// sanitize turn
	t = std::min(t, 4);
	t = std::max(t, -4);
	turn = t;

	std::stringstream ss;
	ss << t;
	spine->set("set turn " + ss.str());
}

void HUD::go(void) {
	initscr();
	noecho();
	curs_set(0);
	timeout(100);
	int key = 0;
	do {
		key = getch();
		clear();
		switch (key) {
			case 'w':
				speed++;
				no_press = 0;
				break;
			case 's':
				speed--;
				no_press = 0;
				break;
			case 'a':
				turn--;
				no_press = 0;
				break;
			case 'd':
				turn++;
				no_press = 0;
				break;
			case ' ':
				turn = 0;
				speed = 0;
				no_press = 0;
				break;
			case 'o': {
				mTextWindow = create_newwin(10, 70, 3, 5);
				char str[512];
				curs_set(1);
				echo();
				wmove(mTextWindow, 1, 1);
				wgetstr(mTextWindow, str);
				std::string s = "echo ";
				s += str;
				s += " | festival --tts";
				system(s.c_str());
				wclear(mTextWindow);
				destroy_win(mTextWindow);
				curs_set(0);
				noecho();
			} break;
			case ERR:
				no_press++;
				break;
			default:
				break;
		}
		if (no_press > 50) {
			mvprintw(5, 30, "idle stop!");
			turn = 0;
			speed = 0;
		}

		int distance = eye->calculateAvgDistance();
		if (distance < 25) {
			mvprintw(6, 30, "emergency stop!");
			speed = 0;
		}

		mvprintw(1, 0, "                                                            ");
		for (int i = 0; i < distance/5; i++) {
			mvprintw(1, i, "#");
		}

		setTurn(turn);
		setSpeed(speed);
		mvprintw(0, 0, "speed=%2d turn=%2d distance=%d", speed, turn, distance);
		refresh();
	} while(key != 'p');
	endwin();
}

} /* namespace ecce */
