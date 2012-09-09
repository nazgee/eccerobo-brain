/*
 * SerialConfig.h
 *
 *  Created on: Aug 25, 2012
 *      Author: nazgee
 */

#ifndef SERIALCONFIG_H_
#define SERIALCONFIG_H_

#include <string>
#include <iostream>
#include <sstream>

#include "ParityConfig.h"

class SerialConfig {
public:
	SerialConfig() :
		device("/dev/ttyS0"),
		baud_rate(9600),
		data_bits(8),
		stop_bits(1),
		parity("none") {
	}

	SerialConfig(std::string pDevice, int pBaudRate, int pDataBits, std::string pParity, int pStopBits) :
		device(pDevice),
		baud_rate(pBaudRate),
		data_bits(pDataBits),
		stop_bits(pStopBits),
		parity(pParity) {
	}

	std::string device;
	int baud_rate;
	int data_bits;
	int stop_bits;
	ParityConfig parity;

	std::string toString() {
		std::stringstream ss;//create a stringstream
		ss << device << " " << baud_rate << " " << data_bits << (char)parity.get() << stop_bits;
		return ss.str();
	}

};


#endif /* SERIALCONFIG_H_ */
