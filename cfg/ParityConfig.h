/*
 * SerialConfig.h
 *
 *  Created on: Aug 25, 2012
 *      Author: nazgee
 */

#ifndef PARITYCONFIG_H_
#define PARITYCONFIG_H_

#include <string>
#include <stdexcept>
#include <boost/algorithm/string.hpp>

class ParityConfig {


public:
	enum eParity {
		PARITY_NONE = 'N',
		PARITY_EVEN = 'E',
		PARITY_ODD  = 'O',
	};
private:
	enum eParity mParity;

public:
	ParityConfig (std::string parity) {
		set(parity);
	}

	enum eParity set(std::string parity) {
		if (boost::iequals(parity, "even")) {
			mParity = PARITY_EVEN;
		} else if (boost::iequals(parity, "odd")) {
			mParity = PARITY_ODD;
		} else if (boost::iequals(parity, "none")) {
			mParity = PARITY_NONE;
		} else {
			throw std::invalid_argument("Unknown parity (should be none/odd/even)");
		}
		return mParity;
	}

	enum eParity get() {
		return mParity;
	}

//	std::string getString() {
//		std::stringstream ss;//create a stringstream
//		ss << (char)mParity;
//		return ss.str();
//	}
};

#endif
