/*
 * TestController.cpp
 *
 *  Created on: May 16, 2014
 *      Author: emory.au
 */

#include <glib.h>
#include "TestController.hpp"
#include "PlaylistService.hpp"

gboolean TestController::test_playlist_service = FALSE;
GOptionEntry TestController::options[] = {
		{ "test-playlist-service", 0, 0, G_OPTION_ARG_NONE, &test_playlist_service, "Test PlaylistService class", NULL },
		{ NULL }
};

TestController::TestController() : Controller() {
}

TestController::~TestController() {
	// TODO Auto-generated destructor stub
}

GOptionGroup* TestController::getOptionGroup() {
	GOptionGroup* group = g_option_group_new(
			"test",
			"Options for the Test Controller",
			"The Test Controller runs tests on various portions of the Gemini codebase",
			NULL,
			NULL
			);
	g_option_group_add_entries( group, options );
	return group;
}

void TestController::run() {
}
