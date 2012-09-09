/*
 * config.h
 *
 *  Created on: Aug 25, 2012
 *      Author: nazgee
 */

#ifndef CONFIG_H_
#define CONFIG_H_

#include <string>

namespace ecce {
class Config {
	int mSilent;
	std::string mSpineServer;
public:
	Config(int argc, char **argv);
	bool isVerbose() { return !mSilent; }
	std::string toString();

	std::string getSpineServer() const {
		return mSpineServer;
	}
};
}

#endif /* CONFIG_H_ */
