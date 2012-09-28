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

#include "misc/Logger.h"
#include "spine/Spine.h"
#include "spine/Eye.h"

using namespace ecce;
static Logger logger("brain");


Spine *spine;

int main(int argc, char **argv) {
	Config cfg(argc, argv);

	if (cfg.isVerbose()) {
		std::cout << "eccerobo brain is starting..." << std::endl;
		std::cout << cfg.toString() << std::endl;
	}

	Eye eye(cfg.getSpineServer());
	std::thread t(&Eye::run, &eye);


	spine = new Spine(cfg.getSpineServer());


	int16_t engine;
	int back_counter = 0;
	do {
		std::cout << "=============" << std::endl;

		engine = spine->getInt("get speed");

		int avg = eye.calculateAvgDistance();
		int speed = 0;
		if (avg > 120) {
			speed = 4;
		} else if (avg > 75) {
			speed = 3;
		} else if (avg > 40) {
			speed = 2;
		} else if (avg > 20) {
			speed = 1;
		}

		std::cout << "speed= " << engine << std::endl;
		std::cout << "turn= " <<spine->getInt("get turn") << std::endl;
		std::cout << "range= " << avg << std::endl;
		std::cout << "safe_speed=" << speed << std::endl;

		if (avg > 20) {
			spine->set("set turn 0");

			if (engine <= 0) {
				std::cout << "sleeping before FORWARD "<< std::endl;
				spine->set("set speed 0");
				usleep(1000 * 1000);
			}

			if (engine <= 0 && speed < 2) {
				spine->set("set speed 4");
				usleep(100 * 1000);
			}

			std::stringstream ss;
			ss << speed;
			spine->set("set speed " + ss.str());

//			std::stringstream ssturn;
//			int dist = std::min(avg, 100);
//			int max = 55;
//			if (dist < max) {
//				ssturn << ((max - dist) * 4) / (max - 20 + 1) + 1;
//			} else {
//				ssturn << 0;
//			}
//
//			spine->set("set turn " + ssturn.str());
		} else {
			if (engine > 0) {
				back_counter = 0;
				std::cout << "sleeping before BACK "<< std::endl;
				spine->set("set speed 0");
				spine->set("set turn 0");
				usleep(1000 * 1000);
				spine->set("set speed -2");
				spine->set("set turn -2");
				usleep(500 * 1000);
				spine->set("set speed 0");
				spine->set("set turn 0");
				usleep(1000 * 1000);
			}

			back_counter++;
			if (back_counter > 200) {
				back_counter = 0;
				sleep(4);
				std::cout << "sleeping before blind BACK "<< std::endl;
				spine->set("set speed 0");
				spine->set("set turn 0");
				usleep(1000 * 1000);
				spine->set("set speed -2");
				spine->set("set turn -2");
				usleep(500 * 1000);
				spine->set("set speed 0");
				spine->set("set turn 0");
				usleep(1000 * 1000);
			}
			spine->set("set turn -2");
			spine->set("set speed 0");
			usleep( 20 * 1000);
		}
	} while (1);

	return EXIT_SUCCESS;
}
