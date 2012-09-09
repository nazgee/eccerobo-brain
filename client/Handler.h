/*
 * SpineHandler.h
 *
 *  Created on: Sep 2, 2012
 *      Author: nazgee
 */

#ifndef SPINEHANDLER_H_
#define SPINEHANDLER_H_

#include <memory>
#include <list>
#include <map>
#include <string>
#include <boost/tokenizer.hpp>
#include <osock-1.0/osock.h>

namespace ecce {
class Handler;
typedef std::shared_ptr<Handler> Handler_p;
typedef boost::tokenizer< boost::char_separator<char> > tokenizer;

class Handler {
public:
	static const std::string REPLY_OK;
	static const std::string REPLY_ERR;

	Handler();
	virtual ~Handler();
	void install(std::string chunk, Handler_p handler);
	virtual std::shared_ptr<osock::Message> handle(const std::string& current_token, tokenizer::iterator &tok);
protected:
	std::string getToken(tokenizer::iterator &tok);
	Handler_p getHandler(std::string token);
private:
	std::map<std::string, Handler_p> mHandlers;
};

} /* namespace ecce */
#endif /* SPINEHANDLER_H_ */
