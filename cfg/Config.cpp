/*
 * config.cpp
 *
 *  Created on: Aug 25, 2012
 *      Author: nazgee
 */
#include <iostream>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <stdexcept>
#include <errno.h>
#include <stdlib.h>
#include "Config.h"

namespace ecce {

Config::Config(int argc, char **argv) :
	mSilent(0),
	mPort("1111") {
	int c;

	while (1) {
		static struct option long_options[] = {
				/* These options set a flag. */
				{ "verbose", no_argument, &mSilent, 1 },
				/* These options don't set a flag.
				 We distinguish them by their indices. */
				{ "device", required_argument, NULL, 'd' },
				{ "baud", required_argument, NULL, 'b' },
				{ "parity", required_argument, NULL, 'p' },
				{ "port", required_argument, NULL, 't' },
				{ 0, 0, 0, 0 }
			};

		/* getopt_long stores the option index here. */
		int option_index = 0;

		c = getopt_long(argc, argv, "d:b:p:t:", long_options, &option_index);

		/* Detect the end of the options. */
		if (c == -1)
			break;

		switch (c) {
		case 0:
			/* If this option set a flag, do nothing else now. */
			if (long_options[option_index].flag != 0)
				break;
			std::cout << "option " << long_options[option_index].name;
			if (optarg)
				std::cout << " with arg " << optarg;
			std::cout << std::endl;
			break;

		case 'd': {
			serial.device = optarg;
		} break;
		case 'p': {
			serial.parity.set(optarg);
		} break;
		case 'b': {
			errno = 0;
			char* str = optarg;
			serial.baud_rate = strtol(optarg, &str, 10);

			if (errno || (optarg == str)) {
				throw std::invalid_argument("Invalid baud rate given");
			}
		} break;
		case 't': {
			mPort = optarg;
		} break;
		case '?':
			/* getopt_long already printed an error message. */
			throw std::invalid_argument("Bad params given!");
			break;

		default:
			abort();
			break;
		}
	}

	/* Print any remaining command line arguments (not options). */
	if (optind < argc) {
		std::cerr << "non-option ARGV-elements: ";
		while (optind < argc)
			std::cerr << argv[optind++];
		std::cerr << std::endl;
	}
}

std::string Config::toString() {
	return serial.toString();
}

}
