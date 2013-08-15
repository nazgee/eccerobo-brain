//============================================================================
// Name        : motord.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C, Ansi-style
//============================================================================

#include <limits>

#include <osock-1.0/osock.h>
#include <boost/thread.hpp>

#include "spine/Spine.h"
#include "spine/Eye.h"
#include "misc/Logger.h"
#include "cfg/Config.h"
#include "HUD/HUD.h"

using namespace ecce;
static Logger logger("brain");


Spine *spine;

int main(int argc, char **argv) {
	Config cfg(argc, argv);

	Logger::ForceLoglevel(cfg.getLoglevel());

	NFO_FUNC << "eccerobo brain is starting..." << std::endl;
	NFO_FUNC << cfg.toString() << std::endl;

//	Eye eye(cfg.getSpineServer());
//	boost::thread thread( boost::bind(&Eye::run, &eye));

	spine = new Spine(cfg.getSpineServer());


	if (cfg.isManual()) {
//		HUD hud(spine, &eye);
		HUD hud(spine, 0);
		hud.go();
		exit(0);
	}

	Eye eye(cfg.getSpineServer());
	boost::thread thread( boost::bind(&Eye::run, &eye));


	int16_t engine;
	int speed;
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
