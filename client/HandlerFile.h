/*
 * HandlerFile.h
 *
 *  Created on: Sep 3, 2012
 *      Author: nazgee
 */

#ifndef HANDLERFILE_H_
#define HANDLERFILE_H_

#include "Handler.h"

namespace ecce {

class HandlerFile: public ecce::Handler {
	int mFile;
	bool mWrite;
public:
	HandlerFile(std::string file, bool write = false);
	virtual ~HandlerFile();
	std::shared_ptr<osock::Message> handle(const std::string& current_token, tokenizer::iterator &tok);
protected:
	int write(const std::string& data2write);
	int read(const void *p, size_t want);
};

} /* namespace ecce */
#endif /* HANDLERFILE_H_ */
