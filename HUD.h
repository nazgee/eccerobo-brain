/*
 * HUD.h
 *
 *  Created on: Oct 9, 2012
 *      Author: nazgee
 */

#ifndef HUD_H_
#define HUD_H_

#include "spine/Spine.h"
#include "spine/Eye.h"
#include <ncurses.h>

namespace ecce {
class HUD {
	Spine* spine;
	Eye* eye;
	int no_press;
	int speed;
	int turn;
	WINDOW *mTextWindow;

	void setSpeed(int s);
	void setTurn(int t);
public:
	HUD(Spine* pSpine, Eye* pEye);
	virtual ~HUD();
	void go();
};

} /* namespace ecce */
#endif /* HUD_H_ */
