#ifndef SQLTable_hpp
#define SQLTable_hpp

#ifdef _MSC_VER
#define my_strdup(a) _strdup(a)
#else
#define my_strdup(a) strdup(a)
#endif


class SQLiteWrapper;

#include <string>
#include <memory>
#include <vector>

#include "./SQLEnums.h"
#include "./SQLQuery.h"

class SQLTable 
{
public:
	typedef struct TableEntry
	{
		std::string name;
		SQLEnums::ValueDataType type;		
	} TableEntry;
		
	~SQLTable();
	
	std::string ToCSV() const;
	std::string ToCSV(const std::string & columns, const std::string & delimeter) const;

	virtual void Clear();
	void AddColumn(const std::string & colName, SQLEnums::ValueDataType type);

	friend class SQLiteWrapper;

protected:
	std::string name;
	std::shared_ptr<SQLiteWrapper> wrapper;
	
	SQLTable(const std::string & name, std::shared_ptr<SQLiteWrapper> wrapper);
};

//===============================================================================

#define REGISTER_VARIABLE(type, name) KeyValueProperty<type> name{ #name, this };
#define REGISTER_VARIABLE_DEFAULT(type, name, defaultVal) KeyValueProperty<type> name{ #name, this, defaultVal };

class SQLKeyValueTable : public SQLTable 
{
public:
	SQLKeyValueTable(const std::string & name, std::shared_ptr<SQLiteWrapper> wrapper);
	virtual ~SQLKeyValueTable();

    void Clear() override;
    
	void EnableRemovalOfNonRegisteredKeys();
	void DisableRemovalOfNonRegisteredKeys();
	void RemoveKey(const std::string & key);

	bool ExistKey(const std::string & key);

	template <typename T, typename = typename std::enable_if<std::is_arithmetic<T>::value, T>::type>
	void AddNewKeyValue(const std::string & key, const T & newValue);
	void AddNewKeyValue(const std::string & key, const std::string & value);
		
	template <typename T, typename = typename std::enable_if<std::is_arithmetic<T>::value, T>::type>
	void AddNewKeyOrUpdateValue(const std::string & key, const T & newValue);
	void AddNewKeyOrUpdateValue(const std::string & key, const std::string & newValue);

	template <typename T, typename = typename std::enable_if<std::is_arithmetic<T>::value, T>::type>
	void UpdateValue(const std::string & key, const T & newValue);
	void UpdateValue(const std::string & key, const std::string & newValue);


	template <typename T>	
	RET_VAL_SAME(std::string) GetValue(const std::string & key);
	
	template <typename T>	
	RET_VAL_GROUP(is_integral) GetValue(const std::string & key);

	template <typename T>
	RET_VAL_GROUP(is_floating_point) GetValue(const std::string & key);
	

protected:

	template <typename T> 
	class KeyValueProperty {	
	protected:
		T value = T();
		std::string key = "";
		SQLKeyValueTable * parent = nullptr;

		friend class SQLKeyValueTable;

	public:	
				
		KeyValueProperty(const std::string & key, SQLKeyValueTable * parent, const T & defaultVal = T()) 
			: key(key), parent(parent)
		{
			parent->RegisterPropertyName(*this, key, defaultVal);

		}

		T & operator = (const T &i) 
		{ 
			parent->UpdateValue(key, i);
			return value = i; 
		};

		// Implicit conversion back to T. 
		operator const T & () 
		{ 
			//value = parent->GetValue<std::decay<T>>(key);
			value = parent->GetValue<typename std::decay<T>::type>(key);
			return value; 
		};
		
		/*
		// This template class member function template serves the purpose to make
		// typing more strict. Assignment to this is only possible with exact identical
		// types.
		template <typename T2> T2 & operator = (const T2 &i) {
			T2 &guard = value;
			throw guard; // Never reached.
		}
		*/
	};


	SQLQuery updateQuery;
	SQLQuery selectQuery;
	std::vector<std::string> keys;
	bool enableNotregisteredKeysRemoval;

	void RemoveNotRegisteredKeys();

	template <typename T>
	void RegisterPropertyName(KeyValueProperty<T> & p, const std::string & key, const T & defaultValue)
	{
		this->AddNewKeyValue(key, defaultValue);		
		p.key = key;
		p.parent = this;	
		keys.push_back(key);
	}
};


class SQLSimpleKeyValueTable : protected SQLKeyValueTable
{
public:

	SQLSimpleKeyValueTable(const std::string & name, std::shared_ptr<SQLiteWrapper> wrapper)
		: SQLKeyValueTable(name, wrapper) {	}

	virtual ~SQLSimpleKeyValueTable() { this->RemoveNotRegisteredKeys(); }
};

class SQLAdvancedKeyValueTable : public SQLKeyValueTable
{
public:

	SQLAdvancedKeyValueTable(const std::string & name, std::shared_ptr<SQLiteWrapper> wrapper)
		: SQLKeyValueTable(name, wrapper) {	}

	virtual ~SQLAdvancedKeyValueTable() { this->RemoveNotRegisteredKeys(); }
};

//===============================================================================

template <typename T, typename>
void SQLKeyValueTable::AddNewKeyValue(const std::string & key, const T & newValue)
{
	this->AddNewKeyValue(key, std::to_string(newValue));
};

template <typename T, typename>
void SQLKeyValueTable::AddNewKeyOrUpdateValue(const std::string & key, const T & newValue)
{
	this->AddNewKeyOrUpdateValue(key, std::to_string(newValue));
};

template <typename T, typename>
void SQLKeyValueTable::UpdateValue(const std::string & key, const T & newValue)
{
	this->UpdateValue(key, std::to_string(newValue));
};


template <typename T>
RET_VAL_SAME(std::string) SQLKeyValueTable::GetValue(const std::string & key)
{
	auto s = this->selectQuery.Select(key);
    auto tmp = s.GetNextRow();
    if (tmp == nullptr)
    {
        return T();
    }
	return tmp->at(0).as_string();
};


template <typename T>
RET_VAL_GROUP(is_integral) SQLKeyValueTable::GetValue(const std::string & key)
{
	auto s = this->selectQuery.Select(key);
    auto tmp = s.GetNextRow();
    if (tmp == nullptr)
    {
        return T();
    }
	return static_cast<T>(tmp->at(0).as_long());
};

template <typename T>
RET_VAL_GROUP(is_floating_point) SQLKeyValueTable::GetValue(const std::string & key)
{
	auto s = this->selectQuery.Select(key);
    auto tmp = s.GetNextRow();
    if (tmp == nullptr)
    {
        return T();
    }
	return static_cast<T>(tmp->at(0).as_double());
};


#endif
