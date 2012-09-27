/*
 * Eye.h
 *
 *  Created on: Sep 27, 2012
 *      Author: nazgee
 */

#ifndef EYE_H_
#define EYE_H_

#include "Spine.h"
#include "IRunnable.h"


#include <boost/circular_buffer.hpp>
#include <mutex>

namespace ecce {

class Eye : public IRunnable {
	boost::circular_buffer<int> mDistances;
	std::mutex mutex;
	Spine mSpine;
protected:
	void measureDist();
public:
	Eye(std::string server);
	virtual ~Eye();
	int calculateMinDistance();
	int calculateAvgDistance();

	virtual void run();
};

} /* namespace ecce */
#endif /* EYE_H_ */
