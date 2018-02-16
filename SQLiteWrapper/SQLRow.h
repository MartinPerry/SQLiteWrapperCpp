//
//  SQLRow.hpp
//  Ventusky_iOS
//
//  Created by Martin Prantl on 23/07/2017.
//  Copyright © 2017 Martin Prantl. All rights reserved.
//

#ifndef SQLRow_hpp
#define SQLRow_hpp

#include <memory>
#include <string>
#include <unordered_map>

#include "sqlite3.h"
#include "SQLEnums.h"

class SQLResult;


class SQLRow
{
public:
            
    class RowValue
    {
    public:
        
        std::string as_string() const;
        int as_int() const;
        long as_long() const;
        double as_double() const;
        
		template <typename T>
		RET_VAL_SAME(std::string) as() const
		{
			return as_string();
		};

		template <typename T>
		RET_VAL_GROUP(is_integral) as() const
		{
			return static_cast<T>(as_long());
		};

		template <typename T>
		RET_VAL_GROUP(is_floating_point) as() const
		{
			return static_cast<T>(as_double());
		};

        std::string GetColumnName() const;
		SQLEnums::ValueDataType GetColumnType();
        friend class SQLRow;
        
    private:
        
        std::shared_ptr<sqlite3_stmt> stmt;
        const int column;
        
        RowValue(std::shared_ptr<sqlite3_stmt> stmt, const int column ) : stmt(stmt), column( column )
        {
        }
    };
    
    RowValue operator []( const int index ) const;
    RowValue operator []( const std::string & key ) const;
    RowValue at( const int index ) const;
    RowValue at( const std::string & key ) const;
    int ColumnCount();
    
    friend class SQLResult;
    
private:
    SQLResult * res;
    std::shared_ptr<sqlite3_stmt> stmt;
    
    SQLRow( SQLResult * res, std::shared_ptr<sqlite3_stmt> stmt);
    SQLRow() = default;
};



#endif /* SQLRow_hpp */
