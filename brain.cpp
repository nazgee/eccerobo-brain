//============================================================================
// Name        : motord.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C, Ansi-style
//============================================================================

#include <stdio.h>
#include <stdlib.h>

#include <cfg/Config.h>
#include <osock-1.0/osock.h>
#include <memory>
#include <signal.h>
#include <boost/lexical_cast.hpp>


#include "misc/Logger.h"

using namespace ecce;
static Logger logger("brain");


osock::StringMessage message ("set engine 1");
osock::ChainedMessage chain;
osock::Parser* spine;

int readValue(std::string cmd) {
	message.setString(cmd);
	spine->Send(message);
	spine->Receive(chain);
	osock::StringMessage& strmsg = (osock::StringMessage&)chain.getLink(1);
	std::string val = strmsg.getString();

	int cm = atoi(val.c_str());
	std::cout << "value=" << cm << " - "<< val <<std::endl;
	return cm;
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

	osock::BIOSocket_p c = osock::BIOSocket::PopulateClientBIO(cfg.getSpineServer());
	spine = new osock::Parser(c);

	int engine;
	int cm;
	int loop = 0;
	do {
		loop++;
		cm = readValue("get range");
		std::cout << "cm= " << cm << std::endl;
		if (cm  == -EINVAL)
			cm = 9999999;
		engine = readValue("get engine");

		if ((loop % 10) == 0) {
			std::cout << "sleeping @loop "<< std::endl;
			message.setString("set engine 0");
			spine->Send(message);
			spine->Receive(message);
			sleep(1);
		}

		if (cm > 25) {
			if (engine == 2) {
				std::cout << "sleeping @2 "<< std::endl;
				message.setString("set engine 0");
				spine->Send(message);
				spine->Receive(message);
				sleep(2);
			}

			message.setString("set engine 1");
			spine->Send(message);
			spine->Receive(message);
			if (cm < 80) {
				if (cm < 50) {
					message.setString("set turn 500");
					spine->Send(message);
					spine->Receive(message);
				} else {
					message.setString("set turn 300");
					spine->Send(message);
					spine->Receive(message);
				}
			} else {
				message.setString("set turn 0");
				spine->Send(message);
				spine->Receive(message);
			}
		} else {
			if (engine == 1) {
				std::cout << "sleeping @1 "<< std::endl;
				message.setString("set engine 0");
				spine->Send(message);
				spine->Receive(message);
				sleep(2);
			}

			message.setString("set engine 2");
			spine->Send(message);
			spine->Receive(message);

			message.setString("set turn -500");
			spine->Send(message);
			spine->Receive(message);

			sleep(1);

			message.setString("set engine 0");
			spine->Send(message);
			spine->Receive(message);

			sleep(1);
		}
	} while (1);

	return EXIT_SUCCESS;
}
