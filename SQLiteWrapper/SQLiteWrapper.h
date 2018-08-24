//
//  SQLiteWrapper.h
//  Ventusky_iOS
//
//  Created by Martin Prantl on 22/07/2017.
//  Copyright © 2017 Martin Prantl. All rights reserved.
//

#ifndef SQLiteWrapper_h
#define SQLiteWrapper_h

#include <string>
#include <memory>
#include <vector>



#include "sqlite3.h"

#include "SQLEnums.h"
#include "SQLQuery.h"
#include "SQLTable.h"

#include "SQLLogger.h"

#if defined(_DEBUG) || defined(DEBUG)
#define SQLITE_CHECK(stmt) do { \
int r = stmt; \
if (r != SQLITE_OK && r != SQLITE_DONE){ \
    SQL_LOG("SQLite error: %i - %s\n", r, #stmt); \
} \
} while (0);
#else
#define SQLITE_CHECK(stmt) stmt
#endif


class SQLiteWrapper 
	: public std::enable_shared_from_this<SQLiteWrapper>
{
public:
        
	
	static std::shared_ptr<SQLiteWrapper> Open(const std::string & path, int mode);

	
    ~SQLiteWrapper();
    
    sqlite3 * GetRawConnection();
    
    std::string GetErrorMsg() const;
    long long GetLastInsertID() const;
    	
    void DropAll();
    void DropTable(const std::string & tableName) const;
    
	bool CheckIntegrity();

	template <typename T>
	std::shared_ptr<T> OpenTable(const std::string & tableName);
	std::shared_ptr<SQLTable> CreateTable(const std::string & tableName,
		const std::vector<SQLTable::TableEntry> & columns,
		const std::string & primaryKeyName,
		bool isPrimaryKeyWithAutoIncrement);
	
    std::vector<std::string> GetAllTablesNames() const;
    bool ExistTable(const std::string & table) const;
    
    SQLQuery Query( const std::string & query ) const;

	
	//friend class SQLTable;

protected:
    sqlite3 *db;
	SQLiteWrapper(const std::string & path, int mode);
	
};

template <typename T>
std::shared_ptr<T> SQLiteWrapper::OpenTable(const std::string & tableName)
{
	if (this->ExistTable(tableName))
	{
		return std::shared_ptr<T>(new T(tableName, shared_from_this()));
	}

	return nullptr;
}


#endif /* SQLiteWrapper_h */
