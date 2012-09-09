/*
 * SystemException.h
 *
 *  Created on: Aug 29, 2012
 *      Author: nazgee
 */

#ifndef SYSTEMEXCEPTION_H_
#define SYSTEMEXCEPTION_H_

#include <errno.h>
#include <stdexcept>
#include <string.h>

namespace ecce {
class SystemException : public std::runtime_error {
	int err;
public:
	SystemException(const std::string& arg) :
		std::runtime_error((arg + ": " ) + strerror(errno)) {
		err = errno;
	}

	int getErr() const {
		return err;
	}
};
}
#endif /* SYSTEMEXCEPTION_H_ */
