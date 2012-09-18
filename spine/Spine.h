/*
 * Spine.h
 *
 *  Created on: Sep 18, 2012
 *      Author: nazgee
 */

#ifndef SPINE_H_
#define SPINE_H_

#include <string>
#include <memory>
#include <mutex>
#include <osock-1.0/osock.h>

namespace ecce {
class Spine {
	std::mutex mutex;
	std::unique_ptr<osock::Parser> mParser;
	osock::StringMessage mMessage;
	osock::StringMessage mACK;
	osock::ChainedMessage mReplyWithACK;
public:
	Spine(std::string server);
	virtual ~Spine() {
	}

	int getInt(std::string cmd);
	osock::StringMessage setWithReply(std::string cmd);
	void set(std::string cmd);
};
} // namespace ecce
#endif /* SPINE_H_ */
