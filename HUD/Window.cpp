/*
 * Window.cpp
 *
 *  Created on: Oct 9, 2012
 *      Author: nazgee
 */

#include "Window.h"

namespace ecce {

Window::Window(int width, int height, int startx, int starty) :
	mWidth(width), mHeight(height), mX(startx), mY(starty) {

	mWindow = newwin(height, width, starty, startx);
	box(mWindow, 0 , 0);		/* 0, 0 gives default characters
					 * for the vertical and horizontal
					 * lines			*/
	wrefresh(mWindow);		/* Show that box 		*/
}

Window::~Window() {
	wclear(mWindow);
	wborder(mWindow, ' ', ' ', ' ',' ',' ',' ',' ',' ');
	wrefresh(mWindow);
	delwin(mWindow);
}

} /* namespace ecce */
