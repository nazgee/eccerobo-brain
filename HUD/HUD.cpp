/*
 * HUD.cpp
 *
 *  Created on: Oct 9, 2012
 *      Author: nazgee
 */

#include <limits>
#include <sstream>
#include <locale>
#include <clocale>
#include <iostream>
#include <cwchar>
#include <wchar.h>


#include "HUD.h"
#include "Window.h"
//#include "../misc/Logger.h"
#include <wchar.h>
#include <boost/algorithm/string/replace.hpp>
#ifdef __arm__
#include <curses.h>
#else
#include <ncursesw/cursesw.h>
#endif

namespace ecce {
//static Logger logger("HUD");

HUD::HUD(Spine* pSpine, Eye* pEye) :
	spine(pSpine),
	eye(pEye),
	no_press(0),
	speed(0),
	turn(0) {

}

HUD::~HUD() {

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
	std::setlocale(LC_ALL, "");
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
			case 'p':
			case 'e': {
				// FIXME wchars are fucked... clean it up sometime
#define MAXINPUT 512
				std::string language;
				switch (key) {
				case 'p':
					language = "polish";
					break;
				case 'e':
				default:
					language = "britishenglish";
					break;
				}

				Window win(70, 10, 5, 3);
				mvwprintw(win.get(), 0, win.getWidth()/2 - language.length()/2 , language.c_str());
				wint_t input_wint[MAXINPUT];
				wchar_t input_wchar[MAXINPUT];
				curs_set(1);
				echo();
				wmove(win.get(), 1, 1);
				wgetn_wstr(win.get(), input_wint, MAXINPUT);
				int i;
				for (i = 0; (input_wint[i]) != NULL; ++i) {
					input_wchar[i] = input_wint[i];
				}
				input_wchar[i] = 0;

				std::wstring input(input_wchar);
				if (language == "polish") {
					boost::algorithm::replace_all(input, L"ą", "o~");
					boost::algorithm::replace_all(input, L"ć", "c~");
					boost::algorithm::replace_all(input, L"ę", "e~");
					boost::algorithm::replace_all(input, L"ł", "l/");
					boost::algorithm::replace_all(input, L"ń", "n~");
					boost::algorithm::replace_all(input, L"ó", "u");
					boost::algorithm::replace_all(input, L"ś", "s~");
					boost::algorithm::replace_all(input, L"ź", "z~");
					boost::algorithm::replace_all(input, L"ż", "z*");
				}

				const std::locale locale("");

				typedef std::codecvt<wchar_t, char, std::mbstate_t> converter_type;
				const converter_type& converter = std::use_facet<converter_type>(locale);
				std::vector<char> to(input.length() * converter.max_length());
				std::mbstate_t state;
				const wchar_t* from_next;
				char* to_next;
				const converter_type::result result = converter.out(state, input.data(), input.data() + input.length(), from_next, &to[0], &to[0] + to.size(), to_next);
				if (result == converter_type::ok or result == converter_type::noconv) {
					const std::string converted(&to[0], to_next);
					//std::cout <<"std::string =     "<<converted<<std::endl;

					std::string s = "echo ";
					s += converted;
					s += " | festival --tts --language ";
					s += language;
					s += " > /dev/null";

					int res = system(s.c_str());
					if (res) {
						sleep(1);
					}
				}
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
	} while(key != 'q');
	endwin();
}

} /* namespace ecce */
