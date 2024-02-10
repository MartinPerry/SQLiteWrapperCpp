//
//  SQLQuery.cpp
//  Ventusky_iOS
//
//  Created by Martin Prantl on 22/07/2017.
//  Copyright © 2017 Martin Prantl. All rights reserved.
//
#include "SQLQuery.h"

#include <string.h>

#include "SQLiteWrapper.h"

SQLQuery::SQLQuery()
	: SQLQuery(nullptr)
{
}

SQLQuery::SQLQuery(sqlite3_stmt * stmt)
{
#if defined(_DEBUG) || defined(DEBUG)
    this->stmt = std::shared_ptr<sqlite3_stmt>(stmt, [=](sqlite3_stmt* stmt)
                                               {
                                                   //MY_LOG_INFO("Releasing SQL statement for query: %s", sqlite3_sql(stmt));
                                                   sqlite3_finalize(stmt);
                                               });
#else
    this->stmt = std::shared_ptr<sqlite3_stmt>(stmt, sqlite3_finalize);
#endif
    this->autoBind = true;
}

void SQLQuery::ClearBindings()
{
    this->autoBind = true;
    if (stmt.get() != nullptr)
    {
        SQLITE_CHECK(sqlite3_clear_bindings( stmt.get() ));
    }
}

void SQLQuery::Reset()
{
    SQLITE_CHECK(sqlite3_reset( stmt.get() ));
}


SQLResult SQLQuery::Select()
{
    this->Reset();
    if (this->autoBind)
    {
        this->ClearBindings();
    }
    return SQLResult( stmt );
}

void SQLQuery::Execute()
{
    this->Reset();
    if (this->autoBind)
    {
        this->ClearBindings();
    }
    this->ExecuteStep();
}

void SQLQuery::ExecuteStep()
{
    SQLITE_CHECK(sqlite3_step( stmt.get() ));
}

std::vector<std::string> SQLQuery::GetColumnNames() const
{
    int count = sqlite3_column_count( stmt.get() );
    std::vector<std::string> names;
    for (int i = 0; i < count; i++)
    {
        names.emplace_back( sqlite3_column_name( stmt.get(), i ) );
    }
    return names;
}


void SQLQuery::set(sqlite3_stmt *stmt, int index, int value) 
{
    SQLITE_CHECK(sqlite3_bind_int( stmt, index, value ));
}

void SQLQuery::set(sqlite3_stmt *stmt, int index, double value) 
{
    SQLITE_CHECK(sqlite3_bind_double( stmt, index, value ));
}

void SQLQuery::set(sqlite3_stmt *stmt, int index, float value) 
{
    SQLITE_CHECK(sqlite3_bind_double( stmt, index, (double) value ));
}

void SQLQuery::set(sqlite3_stmt *stmt, int index, std::string value) 
{
    SQLITE_CHECK(sqlite3_bind_text( stmt, index, value.c_str(), (int) value.length(), SQLITE_TRANSIENT ));
}

void SQLQuery::set(sqlite3_stmt *stmt, int index, const char * value) 
{
    SQLITE_CHECK(sqlite3_bind_text( stmt, index, value, (int) strlen( value ), SQLITE_TRANSIENT ));
}

void SQLQuery::set(sqlite3_stmt *stmt, int index, char * value) 
{
    SQLITE_CHECK(sqlite3_bind_text( stmt, index, value, (int) strlen( value ), SQLITE_TRANSIENT ));
}


