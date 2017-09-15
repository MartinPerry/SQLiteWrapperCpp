//
//  SQLResult.cpp
//  Ventusky_iOS
//
//  Created by Martin Prantl on 22/07/2017.
//  Copyright © 2017 Martin Prantl. All rights reserved.
//

#include "SQLResult.h"

SQLResult::SQLResult(std::shared_ptr<sqlite3_stmt> stmt) : stmt(stmt), isValid(true), row(this, stmt)
{
}

SQLResult::SQLResult( const SQLResult & res) :
    stmt(res.stmt), isValid(res.isValid), row(this, res.stmt)
{
}

const SQLResult::const_iterator SQLResult::begin()
{
    this->isValid = true;
    this->GetNextRow();
    return SQLResult::const_iterator( this );
}

const SQLResult::const_iterator SQLResult::end()
{
    return SQLResult::const_iterator( this );
}

const SQLRow * SQLResult::GetNextRow()
{
    if (isValid == false)
    {
        return nullptr;
    }
    
    if ( sqlite3_step( stmt.get() ) != SQLITE_ROW )
    {
        isValid = false;
        return nullptr;
    }
    return &row;
}


void SQLResult::Reset()
{
    sqlite3_reset(stmt.get());
    isValid = true;
}

int SQLResult::ColumnCount() const
{
    return sqlite3_column_count(stmt.get());
}



void SQLResult::CreateNameIndexMapping()
{
    int count = this->ColumnCount();
    for (int i = 0; i < count; i++)
    {
        std::string key = std::string(sqlite3_column_name(stmt.get(), i));
        assocKeyMapping[key] = i;
    }
}
