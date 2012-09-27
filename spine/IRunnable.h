/*
 * Eye.h
 *
 *  Created on: Sep 27, 2012
 *      Author: nazgee
 */

#ifndef RUNNABLE_H_
#define RUNNABLE_H_

namespace ecce {

class IRunnable {
public:
	virtual ~IRunnable() {};
	virtual void run() = 0;
};

} /* namespace ecce */
#endif /* RUNNABLE_H_ */
