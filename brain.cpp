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
	boost::circular_buffer<int> measurements(3);
	do {
		std::cout << "=============" << std::endl;

		int min = 0;
		for (unsigned int i = 0; i < 3; i++) {
			usleep(10 * 1000);
			cm = spine->getInt("get range");
			if (cm  == -EINVAL)
				cm = 300;
			min += cm;
		}
		min /= 3;

		measurements.push_front(min);

		int avg = 0;
		for (boost::circular_buffer<int>::const_iterator it = measurements.begin() ; it !=measurements.end(); ++it)
		{
			min = std::min(min, *it);
			avg += *it;
		}
		avg /= measurements.size();
		engine = spine->getInt("get engine");

		int speed = 0;
		if (min > 150) {
			speed = 32000;
		} else if (min > 125) {
			speed = 26000;
		} else if (min > 100) {
			speed = 25000;
		} else if (min > 75) {
			speed = 24000;
		} else if (min > 50) {
			speed = 22000;
		} else if (min > 30) {
			speed = 20000;
		}
		std::cout << "speed= " << speed << std::endl;

		if (min > 30) {
			if (engine < 0) {
				std::cout << "sleeping before FORWARD "<< std::endl;
				spine->set("set engine 0");
				usleep(500 * 1000);
			}

			if (engine <= 0 && speed < 32000) {
				spine->set("set engine 32000");
				usleep(500 * 1000);
			}

			std::stringstream ss;
			ss << speed;
			spine->set("set engine " + ss.str());

			std::stringstream ssturn;
			ssturn << (100 + 50 - std::min(avg, 100)) * 500 / 100;
			spine->set("set turn " + ssturn.str());
		} else {
			if (engine > 0) {
				std::cout << "sleeping before BACK "<< std::endl;
				spine->set("set engine 0");
				usleep(500 * 1000);
			}

			spine->set("set turn -500");
			spine->set("set engine -25000");
			usleep( 50 * 1000);
		}
	} while (1);

	return EXIT_SUCCESS;
}
