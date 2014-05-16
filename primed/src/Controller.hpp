/*
 * Controller.hpp
 *
 *  Created on: May 16, 2014
 *      Author: emory.au
 */

#ifndef CONTROLLER_HPP_
#define CONTROLLER_HPP_

#include <glib.h>
#include "dao/DAOFactory.hpp"


class Controller {
public:
	Controller();
	virtual ~Controller() = 0;

	void setDAOFactory( DAOFactory* dao_factory ) { this->dao_factory = dao_factory; };

	virtual GOptionGroup* getOptionGroup() = 0;
	virtual void run(bool& allow_daemon) = 0;

protected:
	DAOFactory* dao_factory;
};

#endif /* CONTROLLER_HPP_ */
