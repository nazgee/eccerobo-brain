/*
 * HandlerFile.cpp
 *
 *  Created on: Sep 3, 2012
 *      Author: nazgee
 */

#include "HandlerFile.h"
#include "../exceptions/SystemException.h"
#include "../misc/Logger.h"

//#include <stdio.h>
//#include <sys/stat.h>
//#include <sys/types.h>
#include <fcntl.h>


namespace ecce {
static Logger logger("SpineServer");

HandlerFile::HandlerFile(std::string file, bool write) :
	mWrite(write) {
	if (mWrite) {
		mFile = open(file.c_str(), O_WRONLY);
	} else {
		mFile = open(file.c_str(), O_RDONLY);
	}
	if (mFile < 0)
		throw SystemException("open failed!");
}

HandlerFile::~HandlerFile() {
	close(mFile);
}

std::shared_ptr<osock::Message> HandlerFile::handle(const std::string& current_token, tokenizer::iterator &tok) {
	lseek(mFile, SEEK_SET, 0);
	if (mWrite) {
		std::string token = getToken(tok);
		int written = write(token);
		if (written < 0)
			return std::shared_ptr<osock::Message>(	new osock::StringMessage(REPLY_ERR + " " + strerror(errno)));
		else
			return std::shared_ptr<osock::Message>(	new osock::StringMessage(REPLY_OK));
	} else {
		char buff[100];
		int rxed = read(buff, sizeof(buff));
		if (rxed < 0)
			return std::shared_ptr<osock::Message>(	new osock::StringMessage(REPLY_ERR + " " + strerror(errno)));
		else
			return std::shared_ptr<osock::Message>(	new osock::StringMessage(REPLY_OK + " " + std::string(buff, rxed)));
	}
}

int HandlerFile::write(const std::string& data2write) {
	int ret;

	const char *p = data2write.c_str();
	size_t want = data2write.size();
	errno = 0;
	while (want) {
		ret = ::write(mFile, p, want);
		if (ret <= 0) {
			if (errno != EINTR && errno != EAGAIN) {
				return -1;
			}
			errno = 0;
			continue;
		}
		want -= ret;
		p = p + ret;
	}
	return 0;
}

int HandlerFile::read(const void *p, size_t want) {
	int ret;
	int sofar = 0;

	errno = 0;
	while (want - sofar) {
		ret = ::read(mFile, (uint8_t*) p, want - sofar);
		if (ret == 0) {
			NFO << "EOF reached, sofar=" << sofar << std::endl;
			return sofar; /* EOF */
		}
		if (ret <= 0) {
			if (errno != EINTR && errno != EAGAIN) {
				return -1;
			}
			errno = 0;
			continue;
		}
		sofar += ret;
		p = (uint8_t*) p + ret;
	}
	return sofar;
}


} /* namespace ecce */
