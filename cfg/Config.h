/*
 * config.h
 *
 *  Created on: Aug 25, 2012
 *      Author: nazgee
 */

#ifndef CONFIG_H_
#define CONFIG_H_

#include <string>

#include "../misc/Logger.h"

namespace ecce {
class Config {
	Logger::logLevel mLoglevel;
	std::string mSpineServer;
public:
	Config(int argc, char **argv);
	std::string toString();

	Logger::logLevel getLoglevel() const {
		return mLoglevel;
	}

	std::string getSpineServer() const {
		return mSpineServer;
	}
};
}

#endif /* CONFIG_H_ */
