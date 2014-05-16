/*
 * MaintenenceController.hpp
 *
 *  Created on: May 16, 2014
 *      Author: emory.au
 */

#ifndef MAINTENANCECONTROLLER_HPP_
#define MAINTENANCECONTROLLER_HPP_

#include <glib.h>
#include <string>

#include "Controller.hpp"

class MaintenanceController: public Controller {
public:
	virtual ~MaintenanceController();
	MaintenanceController();

	virtual GOptionGroup* getOptionGroup();
	virtual void run( bool& allow_daemon );

private:
	static GOptionEntry options[];
	static gchar* scan_directory;
	static gboolean update_playlists;
	static const char* extension_blacklist[];

	void scanDirectory();
	void updatePlaylists();
	bool isInBlacklist( const std::string& subject );
};

#endif /* MAINTENANCECONTROLLER_HPP_ */
