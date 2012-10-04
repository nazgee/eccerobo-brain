//============================================================================
// Name        : motord.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C, Ansi-style
//============================================================================

#include <stdio.h>
#include <stdlib.h>
#include <limits>
#include <thread>

#include <cfg/Config.h>
#include <osock-1.0/osock.h>
#include <memory>
#include <signal.h>
#include <boost/lexical_cast.hpp>
#include <boost/circular_buffer.hpp>
#include <boost/thread.hpp>
#include <curses.h>

#include "misc/Logger.h"
#include "spine/Spine.h"
#include "spine/Eye.h"

using namespace ecce;
static Logger logger("brain");


Spine *spine;


int speed = 0;
int turn = 0;

void setSpeed(int s) {
	// sanitize speed
	s = std::min(s, 4);
	s = std::max(s, -4);
	speed = s;

	std::stringstream ss;
	ss << s;
	spine->set("set speed " + ss.str());
}

void setTurn(int t) {
	// sanitize turn
	t = std::min(t, 4);
	t = std::max(t, -4);
	turn = t;

	std::stringstream ss;
	ss << t;
	spine->set("set turn " + ss.str());
}

int main(int argc, char **argv) {
	Config cfg(argc, argv);

	Logger::ForceLoglevel(cfg.getLoglevel());

	NFO_FUNC << "eccerobo brain is starting..." << std::endl;
	NFO_FUNC << cfg.toString() << std::endl;

	Eye eye(cfg.getSpineServer());
	boost::thread thread( boost::bind(&Eye::run, &eye));

	spine = new Spine(cfg.getSpineServer());


	int no_press = 0;
	if (cfg.isManual()) {
		initscr();
		noecho();
		timeout(100);
		int key = 0;
		do {
			key = getch();
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
				case ERR:
					no_press++;
					break;
				default:
					break;
			}
			if (no_press > 50) {
				mvprintw(1, 30, "idle stop!");
				refresh();
				turn = 0;
				speed = 0;
			} else {
				mvprintw(1, 30, "               ");
				refresh();
			}
			int distance = eye.calculateAvgDistance();
			if (distance < 25) {
				mvprintw(0, 30, "emergency stop!");
				refresh();
				speed = 0;
			} else {
				mvprintw(0, 30, "               ");
				refresh();
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
		exit(0);
	}


	int16_t engine;
	int back_counter = 0;
	do {
		DBG_FUNC << "=============" << std::endl;

		engine = spine->getInt("get speed");
		int avg = eye.calculateAvgDistance();
		speed = 0;
		if (avg > 120) {
			speed = 4;
		} else if (avg > 75) {
			speed = 3;
		} else if (avg > 45) {
			speed = 2;
		} else if (avg > 30) {
			speed = 1;
		}

		DBG_FUNC << "speed= " << engine << std::endl;
		DBG_FUNC << "turn= " <<spine->getInt("get turn") << std::endl;
		DBG_FUNC << "range= " << avg << std::endl;
		DBG_FUNC << "safe_speed=" << speed << std::endl;

		if (avg > 30) {
			if (engine <= 0) {
				DBG_FUNC << "sleeping before FORWARD "<< std::endl;
				spine->set("set speed 0");
				spine->set("set turn 0");
				usleep(100 * 1000);
			}

			std::stringstream ss;
			ss << speed;
			spine->set("set speed " + ss.str());

			std::stringstream ssturn;
			if (avg < 40) {
				ssturn << -3;
			} else if (avg < 50) {
				ssturn << -2;
			} else if (avg < 60) {
				ssturn << -1;
			} else {
				ssturn << 0;
			}

			spine->set("set turn " + ssturn.str());
			usleep( 30 * 1000);
		} else {
			if (engine > 0) {
				back_counter = 0;
				DBG_FUNC << "sleeping before BACK "<< std::endl;
				spine->set("set speed 0");
				spine->set("set turn 0");
				usleep(100 * 1000);
			}

			if (back_counter > 40) {
				back_counter = 0;
				DBG_FUNC << "sleeping before blind BACK "<< std::endl;
				spine->set("set speed -2");
				spine->set("set turn -2");
				usleep(1000 * 1000);
			} else {
				back_counter++;
				spine->set("set turn -2");
				spine->set("set speed 0");
				usleep( 30 * 1000);
			}
		}
	} while (1);

	return EXIT_SUCCESS;
}
