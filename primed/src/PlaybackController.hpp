/*
 * PlaybackController.hpp
 *
 *  Created on: May 16, 2014
 *      Author: emory.au
 */

#ifndef PLAYBACKCONTROLLER_HPP_
#define PLAYBACKCONTROLLER_HPP_

#include "Controller.hpp"

class PlaybackController: public Controller {
public:
	PlaybackController();
	virtual ~PlaybackController();

	virtual GOptionGroup* getOptionGroup();
	virtual void run(bool& allow_daemon);

private:
	static GOptionEntry options[];
	static gboolean foreground;

	void daemon();
};

#endif /* PLAYBACKCONTROLLER_HPP_ */
