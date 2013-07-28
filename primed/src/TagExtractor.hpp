/*
 * TagExtractor.hpp
 *
 *  Created on: Jul 28, 2013
 *      Author: emoryau
 */


#include <gst/gst.h>

class TagExtractor
{
public:
	TagExtractor();

	static void printTags(const char* filename);

private:
	static void onNewPad (GstElement * dec, GstPad * pad, GstElement * fakesink);
	static void printOneTag (const GstTagList * list, const gchar * tag, gpointer user_data);
	
};
