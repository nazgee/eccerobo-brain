/*
 * Eye.cpp
 *
 *  Created on: Sep 27, 2012
 *      Author: nazgee
 */

#include "Eye.h"
#include "../misc/Logger.h"
#include <limits>
namespace ecce {

static Logger logger("brain");

Eye::Eye(std::string server) :
	mDistances(5),
	mSpine(server) {

	measureDist();
}

Eye::~Eye() {
}

int Eye::calculateMinDistance() {
	std::lock_guard<std::mutex> autolock(mutex);

	if (mDistances.size() == 0) {
		return 0;
	}

	int min = std::numeric_limits<int>::max();
	for (boost::circular_buffer<int>::const_iterator it = mDistances.begin() ; it !=mDistances.end(); ++it)
	{
		min = std::min(min, *it);
	}

	return min;
}

int Eye::calculateAvgDistance() {
	std::lock_guard<std::mutex> autolock(mutex);

	if (mDistances.size() == 0) {
		return 0;
	}

	int avg = 0;
	for (boost::circular_buffer<int>::const_iterator it = mDistances.begin() ; it !=mDistances.end(); ++it)
	{
		avg += *it;
	}
	avg /= mDistances.size();

	return avg;
}

void Eye::measureDist() {
	std::lock_guard<std::mutex> autolock(mutex);
	int meas = mSpine.getInt("get range");
	mDistances.push_front(meas);
}

void Eye::run() {
	while(1) {
		measureDist();
		usleep(10000);
		//DBG << "foo " << mDistances.front() << std::endl;
	}
}

} /* namespace ecce */
