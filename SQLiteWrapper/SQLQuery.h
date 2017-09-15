//
//  SQLQuery.hpp
//  Ventusky_iOS
//
//  Created by Martin Prantl on 22/07/2017.
//  Copyright © 2017 Martin Prantl. All rights reserved.
//

#ifndef SQLQuery_hpp
#define SQLQuery_hpp

#include <memory>
#include <vector>
#include <string>
#include <sstream>

#include "sqlite3.h"

#include "SQLResult.h"

class SQLQuery
{
public:
    
    ~SQLQuery() = default;
    
    SQLResult Select();
    
    template <typename T, typename... Args>
    SQLResult Select( T t, Args... args )
    {
        this->Reset();
        this->ClearBindings();
        set( stmt.get(), 1, t, args... );
        return SQLResult( stmt );
    }
    
    void Execute();
    
    template <typename T, typename... Args>
    void Execute( T t, Args... args )
    {
        this->Reset();
        this->ClearBindings();
        set( stmt.get(), 1, t, args... );
        this->ExecuteStep();
    }
    
    void ClearBindings();
    
    template <typename T>
    void Bind(T t, int index)
    {
        this->autoBind = false;
        set(stmt.get(), index, t);
    }
    
    std::vector<std::string> GetColumnNames() const;
    
    friend class SQLiteWrapper;
	friend class SQLKeyValueTable;
    
protected:
    std::shared_ptr<sqlite3_stmt> stmt;
    bool autoBind;
    
	SQLQuery();
    SQLQuery(sqlite3_stmt * stmt);
    
    
    void Reset();
    void ExecuteStep();
    
	void set(sqlite3_stmt *stmt, int index, int value);
	void set(sqlite3_stmt *stmt, int index, double value);
	void set(sqlite3_stmt *stmt, int index, float value);
	void set(sqlite3_stmt *stmt, int index, std::string value);
	void set(sqlite3_stmt *stmt, int index, const char * value);
	void set(sqlite3_stmt *stmt, int index, char * value);

    template <typename T>
    void set( sqlite3_stmt *stmt, int index, T value)
    {
        std::ostringstream stream;
        stream << value;
        std::string text( stream.str() );
        set(stmt, index, text); //will call set with std::string       
    }
    
    template<typename T, typename... Args>
    void set( sqlite3_stmt *stmt, int index, T value, Args... args)
    {
        set( stmt, index, value );
        set( stmt, index + 1, args...);
    }
    
    
};

#endif /* SQLQuery_hpp */
