/*
 * TestController.hpp
 *
 *  Created on: May 16, 2014
 *      Author: emory.au
 */

#ifndef TESTCONTROLLER_HPP_
#define TESTCONTROLLER_HPP_

#include "Controller.hpp"
#include "dao/DAOFactory.hpp"

class TestController: public Controller {
public:
	TestController();
	virtual ~TestController();

	virtual GOptionGroup* getOptionGroup();
	virtual void run();

private:
	static GOptionEntry options[];
	static gboolean test_playlist_service;
	static gboolean test_database;

	void testDatabase();
	void testPlaylistService();
};

#endif /* TESTCONTROLLER_HPP_ */
