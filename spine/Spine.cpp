/*
 * Spine.cpp
 *
 *  Created on: Sep 18, 2012
 *      Author: nazgee
 */

#include "Spine.h"
#include "SpineException.h"
#include "../misc/Logger.h"

#include <boost/lexical_cast.hpp>

namespace ecce {

static Logger logger("Spine");

Spine::Spine(std::string server) :
	mParser(new osock::Parser(osock::BIOSocket::PopulateClientBIO(server))),
	mMessage(""),
	mACK(""),
	mReplyWithACK() {

	// replies are sent in a form: <STATUS> <VALUE><EOL>
	// we need to setup a message chain covering it
	mReplyWithACK.AddLink(new osock::StringMessage("", " ")); // "<STATUS> "
	mReplyWithACK.AddLink(new osock::StringMessage("")); //"<VALUE><EOL>"
	mReplyWithACK.LinksClose();
}

int Spine::getInt(std::string cmd) {
	int val;
	osock::StringMessage msg = setWithReply(cmd);
	try {
		val = boost::lexical_cast<int>(msg.getString().c_str());
	} catch (boost::bad_lexical_cast& e) {
		ERR << "could not convert \'" << msg.getString() << "'" << std::endl;
		throw e;
	}
	return val;
}

void Spine::set(std::string cmd) {
	std::lock_guard<std::mutex> autolock(mutex);

	// prepare message to send
	mMessage.setString(cmd);

	// message-reply
	mParser->Send(mMessage);
	mParser->Receive(mACK);

	if (mACK.getString() != "OK") {
		throw SpineException("Spine reply error - " + mACK.getString());
	}
}

osock::StringMessage Spine::setWithReply(std::string cmd) {
	std::lock_guard<std::mutex> autolock(mutex);

	// prepare message to send
	mMessage.setString(cmd);

	// message-reply
	mParser->Send(mMessage);
	mParser->Receive(mReplyWithACK);

	// retrieve status
	const osock::StringMessage& status = dynamic_cast<const osock::StringMessage&>(mReplyWithACK.getLink(0));
	// retrieve value
	const osock::StringMessage& value = dynamic_cast<const osock::StringMessage&>(mReplyWithACK.getLink(1));

	if (status.getString() != "OK") {
		throw SpineException("Spine reply error - " + status.getString());
	}
	return value;
}
} // namespace ecce



