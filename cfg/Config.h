/*
 * config.h
 *
 *  Created on: Aug 25, 2012
 *      Author: nazgee
 */

#ifndef CONFIG_H_
#define CONFIG_H_

#include <string>
#include "SerialConfig.h"

namespace ecce {
class Config {
	int mSilent;
	std::string mPort;
public:
	Config(int argc, char **argv);
	SerialConfig serial;
	bool isVerbose() { return !mSilent; }
	std::string toString();

	std::string getPort() const {
		return mPort;
	}
};
}

#endif /* CONFIG_H_ */
