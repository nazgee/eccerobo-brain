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

#include <cfg/Config.h>
#include <osock-1.0/osock.h>
#include <memory>
#include <signal.h>
#include <boost/lexical_cast.hpp>
#include <boost/circular_buffer.hpp>
#include <spine/Spine.h>

#include "misc/Logger.h"

using namespace ecce;
static Logger logger("brain");


Spine *spine;

int main(int argc, char **argv) {
	Config cfg(argc, argv);

	if (cfg.isVerbose()) {
		std::cout << "eccerobo brain is starting..." << std::endl;
		std::cout << cfg.toString() << std::endl;
	}

	spine = new Spine(cfg.getSpineServer());


	int16_t engine;
	int cm;
	int back_counter = 0;
	boost::circular_buffer<int> measurements(2);
	do {
		std::cout << "=============" << std::endl;

		int min = 0;
		for (unsigned int i = 0; i < 2; i++) {
			cm = spine->getInt("get range");
			usleep(100 * 1000);
			if (cm  == -EINVAL)
				cm = 300;
			min += cm;
			std::cout << " cm= " << cm;
		}
		min /= 2;
		std::cout << std::endl;

		measurements.push_front(min);

		int avg = 0;
		for (boost::circular_buffer<int>::const_iterator it = measurements.begin() ; it !=measurements.end(); ++it)
		{
			min = std::min(min, *it);
			avg += *it;
		}
		avg /= measurements.size();
		engine = spine->getInt("get speed");

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

			if (engine < 0) {
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
