//
//  SQLiteWrapper.cpp
//  Ventusky_iOS
//
//  Created by Martin Prantl on 22/07/2017.
//  Copyright © 2017 Martin Prantl. All rights reserved.
//


#include "SQLiteWrapper.h"

#include "SQLResult.h"
#include "SQLRow.h"

std::shared_ptr<SQLiteWrapper> SQLiteWrapper::Open(const std::string & path, int mode)
{
	return std::shared_ptr<SQLiteWrapper>(new SQLiteWrapper(path, mode));	
}

SQLiteWrapper::SQLiteWrapper(const std::string & path, int mode) : db(nullptr)
{
	sqlite3_shutdown();
    int threadSafe = sqlite3_threadsafe();
    
    if (threadSafe == 1)
    {
        SQLITE_CHECK(sqlite3_config(SQLITE_CONFIG_SERIALIZED));
    }
    
    int flag = 0;
    if (mode & SQLEnums::OpenMode::Create) flag |= SQLITE_OPEN_CREATE;
    if (mode & SQLEnums::OpenMode::FullMutex) flag |= SQLITE_OPEN_FULLMUTEX;
    if (mode & SQLEnums::OpenMode::Memory) flag |= SQLITE_OPEN_MEMORY;
    if (mode & SQLEnums::OpenMode::NoMutex) flag |= SQLITE_OPEN_NOMUTEX;
    if (mode & SQLEnums::OpenMode::PrivateCache) flag |= SQLITE_OPEN_PRIVATECACHE;
    if (mode & SQLEnums::OpenMode::Read) flag |= SQLITE_OPEN_READONLY;
    if (mode & SQLEnums::OpenMode::ReadWrite) flag |= SQLITE_OPEN_READWRITE;
    if (mode & SQLEnums::OpenMode::SharedCache) flag |= SQLITE_OPEN_SHAREDCACHE;
    if (mode & SQLEnums::OpenMode::Uri) flag |= SQLITE_OPEN_URI;

    
    SQLITE_CHECK(sqlite3_open_v2(path.c_str(), &db, flag, nullptr));
}

SQLiteWrapper::~SQLiteWrapper()
{
    sqlite3_close_v2( db );
	sqlite3_shutdown();
}

sqlite3 * SQLiteWrapper::GetRawConnection()
{
    return this->db;
}

std::string SQLiteWrapper::GetErrorMsg() const
{
    return sqlite3_errmsg(db);
}

long long SQLiteWrapper::GetLastInsertID() const
{
    return sqlite3_last_insert_rowid( db );
}


void SQLiteWrapper::DropAll()
{
    auto tables = this->GetAllTablesNames();
    for (auto t : tables)
    {
        this->DropTable(t);
    }
    
	auto tbl = this->OpenTable<SQLTable>("sqlite_sequence");
	if (tbl != nullptr)
	{
		tbl->Clear();
	}
}

void SQLiteWrapper::DropTable(const std::string & tableName) const
{
    this->Query("DROP TABLE IF EXISTS " + tableName).Execute();
}

bool SQLiteWrapper::CheckIntegrity()
{
	SQLResult res = this->Query("pragma integrity_check").Select();

	auto row = res.GetNextRow();
	if (row->ColumnCount() == 0)
	{
		return false;
	}
	std::string res1 = row->at(0).as_string();

	if (res1 == "ok")
	{
		return true;
	}

	return false;
}


std::shared_ptr<SQLTable> SQLiteWrapper::CreateTable(const std::string & tableName,
	const std::vector<SQLTable::TableEntry> & columns,
	const std::string & primaryKeyName,
	bool isPrimaryKeyWithAutoIncrement)
{	
	if (this->ExistTable(tableName))
	{
		printf("Table %s already exist\n", tableName.c_str());		
		return std::shared_ptr<SQLTable>(new SQLTable(tableName, shared_from_this()));
	}

	std::string q = "CREATE TABLE " + tableName;

	q += " (";
	

	for (auto & c : columns)
	{
		q += c.name;
		if (c.type == SQLEnums::ValueDataType::String) q += " TEXT";
		else if (c.type == SQLEnums::ValueDataType::Integer) q += " INTEGER";
		else if (c.type == SQLEnums::ValueDataType::Float) q += " REAL";
		else if (c.type == SQLEnums::ValueDataType::Blob) q += " BLOB";

		if (c.name == primaryKeyName)
		{
			q += " PRIMARY KEY ";
			if (isPrimaryKeyWithAutoIncrement)
			{
				q += " AUTOINCREMENT ";
			}
		}

		q += ",";
	}
	q.pop_back();
	q += ")";
	
	
	this->Query(q).Execute();

	if (this->ExistTable(tableName) == false)
	{
		return nullptr;
	}

	return std::shared_ptr<SQLTable>(new SQLTable(tableName, shared_from_this()));
}

std::vector<std::string> SQLiteWrapper::GetAllTablesNames() const
{
    SQLResult res = this->Query("SELECT name FROM sqlite_master WHERE type='table' AND name<>'sqlite_sequence'").Select();
    
    std::vector<std::string> tables;
    for (auto r : res)
    {
        tables.push_back(r[0].as_string());
    }
    
    return tables;
}

bool SQLiteWrapper::ExistTable(const std::string & table) const
{
    SQLResult res = this->Query("SELECT COUNT(*) FROM sqlite_master WHERE type='table' AND name=?").Select(table);
    
    const SQLRow * row = res.GetNextRow();
    
    if (row == nullptr)
    {
        return false;
    }
    
    return (*row)[0].as_int() != 0;
}


SQLQuery SQLiteWrapper::Query( const std::string & query ) const
{
    sqlite3_stmt *stmt = 0;
    SQLITE_CHECK(sqlite3_prepare_v2(db, query.c_str(), (int)query.length(), &stmt, 0));
    
    return SQLQuery( stmt );
}







