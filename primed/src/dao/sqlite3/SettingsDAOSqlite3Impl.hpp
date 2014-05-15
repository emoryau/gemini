/*
 * SettingsDAOSqlite3Impl.hpp
 *
 *  Created on: May 15, 2014
 *      Author: emory.au
 */

#ifndef SETTINGSDAOSQLITE3IMPL_HPP_
#define SETTINGSDAOSQLITE3IMPL_HPP_

#include "dao/SettingsDAO.hpp"
#include "BaseSqlite3Impl.hpp"

class SettingsDAOSqlite3Impl: public SettingsDAO, public BaseSqlite3Impl {
public:
	SettingsDAOSqlite3Impl( sqlite3* db ): BaseSqlite3Impl( db ) { };
	virtual ~SettingsDAOSqlite3Impl();
	virtual void ensureDBSchema();

	virtual void get( const std::string& key, std::string& value );
	virtual void get( const std::string& key, long& value );
	virtual void get( const std::string& key, double& value );

	virtual void set( const std::string& key, const std::string& value );
	virtual void set( const std::string& key, const long& value );
	virtual void set( const std::string& key, const double& value );
};

#endif /* SETTINGSDAOSQLITE3IMPL_HPP_ */
