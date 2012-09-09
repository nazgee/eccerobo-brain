/*
 * SpineHandler.cpp
 *
 *  Created on: Sep 2, 2012
 *      Author: nazgee
 */

#include "Handler.h"
#include "TokenizingException.h"

namespace ecce {

const std::string Handler::REPLY_OK = "OK";
const std::string Handler::REPLY_ERR = "ERR";

Handler::Handler() {
}

Handler::~Handler() {
}

void Handler::install(std::string chunk, Handler_p handler) {
	mHandlers[chunk] = handler;
}

std::shared_ptr<osock::Message> Handler::handle(const std::string& current_token, tokenizer::iterator &tok) {
	try {
		std::string token = getToken(tok);
		Handler_p handler = getHandler(token);
		tok++;
		return handler->handle(token, tok);
	} catch (TokenizingException e) {
		return std::shared_ptr<osock::Message>(new osock::StringMessage(REPLY_ERR + " " +e.what()));
	}
}

std::string Handler::getToken(tokenizer::iterator &tok) {
	if (tok.at_end()) {
		throw TokenizingException("no token!");
	}

	return *tok;
}

Handler_p Handler::getHandler(std::string token) {
	Handler_p handler = mHandlers[token];
	if (!handler) {
		throw TokenizingException("unknown token (" + token + ")");
	}

	return handler;
}

} /* namespace ecce */
