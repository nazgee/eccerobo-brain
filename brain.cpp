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


#include "misc/Logger.h"

using namespace ecce;
static Logger logger("brain");


osock::StringMessage message ("set engine 1");
osock::ChainedMessage chain;
osock::Parser* spine;
osock::BIOSocket_p clientBIO;

int readValue(std::string cmd) {
	message.setString(cmd);
	spine->Send(message);
	spine->Receive(chain);
	osock::StringMessage& strmsg = (osock::StringMessage&)chain.getLink(1);
	std::string val = strmsg.getString();

//	clientBIO.get()->setReadTimeout(100);
//	try {
//		osock::StringMessage dummy("");
////		std::cout << "trying weird shit..." << std::endl;
//		spine->ReceiveWithoutRetry(dummy);
//	} catch (osock::BIO::RecoverableException& e) {
////		std::cout << "...well, there was no data" << std::endl;
//	}
//	clientBIO.get()->setReadTimeout(0);

	int value = atoi(val.c_str());
	std::cout << "value=" << value << " from "<< val <<std::endl;
	return value;
}

int main(int argc, char **argv) {
	Config cfg(argc, argv);

	if (cfg.isVerbose()) {
		std::cout << "eccerobo brain is starting..." << std::endl;
		std::cout << cfg.toString() << std::endl;
	}

	chain.AddLink(new osock::StringMessage("", " "));
	chain.AddLink(new osock::StringMessage(""));
	chain.LinksClose();

	clientBIO = osock::BIOSocket::PopulateClientBIO(cfg.getSpineServer());
	spine = new osock::Parser(clientBIO);

	int16_t engine;
	int cm;
	int loop = 0;
#define MAX_MEASUREMENT 300
	boost::circular_buffer<int> measurements(3);

	do {
		loop++;

		std::cout << "=============" << std::endl;

//		int min = MAX_MEASUREMENT;
		int min = 0;
		for (unsigned int i = 0; i < 3; i++) {
			usleep(10 * 1000);
			cm = readValue("get range");
			if (cm  == -EINVAL)
				cm = MAX_MEASUREMENT;
//			min = std::min(min, cm);
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
		std::cout << "cm= " << min << " | " << (osock::StringMessage&)chain.getLink(1) << std::endl;

		engine = readValue("get engine");
		std::cout << "engine= " << engine << " | " << (osock::StringMessage&)chain.getLink(1) << std::endl;

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
				std::cout << "sleeping before FWD "<< std::endl;
				message.setString("set engine 0");
				spine->Send(message);
				spine->Receive(message);
				usleep(500 * 1000);
			}

			if (engine <= 0 && speed < 32000) {
				message.setString("set engine 32000");
				spine->Send(message);
				spine->Receive(message);
				usleep(500 * 1000);
			}

			std::stringstream ss;
			ss << speed;
			message.setString("set engine " + ss.str());
			std::cout << message.getString() << std::endl;
			spine->Send(message);
			spine->Receive(message);

			std::stringstream ssturn;
			ssturn << (100 + 50 - std::min(avg, 100)) * 500 / 100;
			std::cout << "turn= " << ssturn.str() << std::endl;
			message.setString(std::string("set turn ") + ssturn.str());
			std::cout << message.getString() << std::endl;
			spine->Send(message);
			spine->Receive(message);
		} else {
			if (engine > 0) {
				std::cout << "sleeping before BWD "<< std::endl;
				message.setString("set engine 0");
				spine->Send(message);
				spine->Receive(message);
				usleep(500 * 1000);
			}

			message.setString("set engine -25000");
			spine->Send(message);
			spine->Receive(message);

			message.setString("set turn -500");
			spine->Send(message);
			spine->Receive(message);

			usleep( 50 * 1000);
		}
	} while (1);

	return EXIT_SUCCESS;
}
