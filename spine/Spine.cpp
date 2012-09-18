/*
 * Spine.cpp
 *
 *  Created on: Sep 18, 2012
 *      Author: nazgee
 */

#include "Spine.h"
#include "SpineException.h"

#include <boost/lexical_cast.hpp>

namespace ecce {
Spine::Spine(std::string server) :
	mParser(new osock::Parser(osock::BIOSocket::PopulateClientBIO(server))),
	mMessage(""),
	mReply() {

	// replies are sent in a form: <STATUS> <VALUE><EOL>
	// we need to setup a message chain covering it
	mReply.AddLink(new osock::StringMessage("", " ")); // "<STATUS> "
	mReply.AddLink(new osock::StringMessage("")); //"<VALUE><EOL>"
	mReply.LinksClose();
}

int Spine::getInt(std::string cmd) {
	return boost::lexical_cast<int>(set(cmd));
}

osock::StringMessage Spine::set(std::string cmd) {
	std::lock_guard<std::mutex> autolock(mutex);

	// prepare message to send
	mMessage.setString(cmd);

	// message-reply
	mParser->Send(mMessage);
	mParser->Receive(mReply);

	// retrieve status
	const osock::StringMessage& status = dynamic_cast<const osock::StringMessage&>(mReply.getLink(0));
	// retrieve value
	const osock::StringMessage& value = dynamic_cast<const osock::StringMessage&>(mReply.getLink(1));

	if (status != "OK") {
		throw SpineException("Spine reply error - " + status.getString());
	}
	return value;
}
} // namespace ecce



