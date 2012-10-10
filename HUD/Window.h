/*
 * Window.h
 *
 *  Created on: Oct 9, 2012
 *      Author: nazgee
 */

#ifndef WINDOW_H_
#define WINDOW_H_

#ifdef __arm__
#include <curses.h>
#else
#include <ncursesw/cursesw.h>
#endif

namespace ecce {

class Window {
	int mWidth;
	int mHeight;
	int mX;
	int mY;
	WINDOW *mWindow;
public:
	Window(int width, int height, int startx, int starty);
	virtual ~Window();

	int getHeight() const {
		return mHeight;
	}

	int getWidth() const {
		return mWidth;
	}

	WINDOW* get() const {
		return mWindow;
	}

	int getX() const {
		return mX;
	}

	int getY() const {
		return mY;
	}
};

} /* namespace ecce */
#endif /* WINDOW_H_ */

