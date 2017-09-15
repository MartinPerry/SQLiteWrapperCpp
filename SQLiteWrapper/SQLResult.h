//
//  SQLResult.hpp
//  Ventusky_iOS
//
//  Created by Martin Prantl on 22/07/2017.
//  Copyright © 2017 Martin Prantl. All rights reserved.
//

#ifndef SQLResult_hpp
#define SQLResult_hpp

#include <memory>
#include <string>
#include <unordered_map>

#include "SQLRow.h"


class SQLResult
{
public:

    class const_iterator
    {
    public:
        typedef std::forward_iterator_tag iterator_category;
        
        const_iterator(SQLResult * res) : res(res) { }
        const_iterator operator++() { res->GetNextRow(); return *this; }
        //self_type operator++(int junk) { ptr_++; return *this; }
        const SQLRow& operator*() { return res->row; }
        const SQLRow* operator->() { return &(res->row); }
        //bool operator==(const const_iterator& rhs){ return !this->res->isValid; }
        bool operator!=(const const_iterator& rhs) { return (this->res->isValid); }
        
        friend class SQLResult;
    private:
        SQLResult * res;
    };
    
    SQLResult( const SQLResult & res);
    
    
    const const_iterator begin();
    const const_iterator end();
    
    const SQLRow * GetNextRow();
    
    void Reset();
    int ColumnCount() const;
    
    
    friend class SQLRow;
    friend class SQLRow::RowValue;
    friend class SQLQuery;
    friend class const_iterator;
    
private:
    std::shared_ptr<sqlite3_stmt> stmt;
    bool isValid;
    SQLRow row;
    std::unordered_map<std::string, int> assocKeyMapping;
    
    
    SQLResult(std::shared_ptr<sqlite3_stmt> stmt);
    void CreateNameIndexMapping();
};

#endif /* SQLResult_hpp */
