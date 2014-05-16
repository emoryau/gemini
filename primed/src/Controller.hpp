/*
 * Controller.hpp
 *
 *  Created on: May 16, 2014
 *      Author: emory.au
 */

#ifndef CONTROLLER_HPP_
#define CONTROLLER_HPP_

#include <glib.h>

class Controller {
public:
	virtual ~Controller() = 0;

	// Command line argument stuff
	virtual GOptionEntry* getOptions() = 0;
	virtual void run() = 0;
};

#endif /* CONTROLLER_HPP_ */
