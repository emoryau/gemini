/*
 * SettingsDAO.hpp
 *
 *  Created on: May 15, 2014
 *      Author: emory.au
 */

#ifndef SETTINGSDAO_HPP_
#define SETTINGSDAO_HPP_

#include <string>

class SettingsDAO {
public:
	virtual ~SettingsDAO() = 0;
	virtual void ensureDBSchema() = 0;

	virtual void get( const std::string& key, std::string& value ) = 0;
	virtual void get( const std::string& key, long& value ) = 0;
	virtual void get( const std::string& key, double& value ) = 0;

	virtual void set( const std::string& key, const std::string& value ) = 0;
	virtual void set( const std::string& key, const long& value ) = 0;
	virtual void set( const std::string& key, const double& value ) = 0;
};

inline SettingsDAO::~SettingsDAO() { };

#endif /* SETTINGSDAO_HPP_ */
