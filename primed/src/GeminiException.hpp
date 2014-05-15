/*
 * GeminiException.hpp
 *
 *  Created on: May 14, 2014
 *      Author: emoryau
 */

#ifndef GEMINIEXCEPTION_HPP_
#define GEMINIEXCEPTION_HPP_

#include <stdexcept>
#include <string>


class GeminiException: public std::runtime_error {
public:
	GeminiException( const std::string &arg, const char* func, const char *file, int line ) throw();
	virtual ~GeminiException() throw();
	virtual const char* what() const throw();

private:
	std::string cause;
};

#define THROW_GEMINI_EXCEPTION(s) throw new GeminiException( s, __PRETTY_FUNCTION__, __FILE__, __LINE__ )


#endif /* GEMINIEXCEPTION_HPP_ */
