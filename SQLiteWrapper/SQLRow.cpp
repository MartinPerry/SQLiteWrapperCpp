//
//  SQLRow.cpp
//  Ventusky_iOS
//
//  Created by Martin Prantl on 23/07/2017.
//  Copyright © 2017 Martin Prantl. All rights reserved.
//

#include "SQLRow.h"

#include "SQLResult.h"

SQLRow::SQLRow( SQLResult * res, std::shared_ptr<sqlite3_stmt> stmt) : res(res), stmt(stmt)
{
}

SQLRow::RowValue SQLRow::operator []( const int index ) const
{
    return SQLRow::RowValue( stmt, index );
}

SQLRow::RowValue SQLRow::operator []( const std::string & key ) const
{
    if (res->assocKeyMapping.size() == 0)
    {
        res->CreateNameIndexMapping();
    }
    return this->operator[](res->assocKeyMapping[key]);
}

SQLRow::RowValue SQLRow::at( const int index ) const
{
    return this->operator[](index);
}

SQLRow::RowValue SQLRow::at( const std::string & key ) const
{
    return this->operator[](key);
}

int SQLRow::ColumnCount() const
{
    return res->ColumnCount();
}



std::string SQLRow::RowValue::as_string() const
{
    return std::string( (char*)sqlite3_column_text(stmt.get(), column),
                       sqlite3_column_bytes(stmt.get(), column) );
}

int SQLRow::RowValue::as_int() const
{
    return sqlite3_column_int( stmt.get(), column );
}

long SQLRow::RowValue::as_long() const
{
    return static_cast<long>(sqlite3_column_int64( stmt.get(), column ));
}

double SQLRow::RowValue::as_double() const
{
    return sqlite3_column_double( stmt.get(), column );
}


std::string SQLRow::RowValue::GetColumnName() const
{
    return std::string(sqlite3_column_name(stmt.get(), column));
}

SQLEnums::ValueDataType SQLRow::RowValue::GetColumnType()
{
    int typeId = sqlite3_column_type(stmt.get(), column);
    
    switch (typeId) {
        case 1: return SQLEnums::ValueDataType::Integer;
        case 2: return SQLEnums::ValueDataType::Float;
        case 3: return SQLEnums::ValueDataType::String;
        case 4: return SQLEnums::ValueDataType::Blob;
        default: return SQLEnums::ValueDataType::Null;
    }
}



