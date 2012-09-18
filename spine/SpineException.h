/*
 * SpineException.h
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
class SpineException : public std::runtime_error {
	int err;
public:
	SpineException(const std::string& arg) :
		std::runtime_error("SpineException" + arg) {
		err = errno;
	}

	int getErr() const {
		return err;
	}
};
}
#endif /* SYSTEMEXCEPTION_H_ */
