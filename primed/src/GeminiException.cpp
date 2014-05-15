/*
 * GeminiException.cpp
 *
 *  Created on: May 14, 2014
 *      Author: emoryau
 */

#include <iostream>
#include <sstream>
#include "GeminiException.hpp"


GeminiException::GeminiException( const std::string& arg, const char* file, int line ) throw() : std::runtime_error(arg) {
	std::ostringstream o;
	o << file << ":" << line << ": " << arg;
	cause = o.str();
}

GeminiException::~GeminiException() throw() {
	// TODO Auto-generated destructor stub
}

const char* GeminiException::what() const throw() {
	return cause.c_str();
}
