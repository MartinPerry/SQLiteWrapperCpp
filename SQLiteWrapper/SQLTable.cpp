#include "./SQLTable.h"

#include "SQLiteWrapper.h"

SQLTable::SQLTable(const std::string & name, std::shared_ptr<SQLiteWrapper> wrapper) :
	name(name), wrapper(wrapper)
{
}

SQLTable::~SQLTable() 
{
}


std::string SQLTable::ToCSV() const
{
	return this->ToCSV("*", "|");
}

std::string SQLTable::ToCSV(const std::string & columns,
	const std::string & delimeter) const
{

	std::string header = "";
	std::string content = "";

	auto result = this->wrapper->Query("SELECT " + columns + " FROM " + name).Select();
	for (auto r : result)
	{
		int cCount = r.ColumnCount();
		header = "";
		for (int i = 0; i < cCount; i++)
		{
			header += r[i].GetColumnName();
			header += delimeter;

			content += r[i].as_string();
			content += delimeter;
		}
		content += "\n";
	}

	content = header + "\n" + content;

	return content;
}


void SQLTable::Clear() const
{
	this->wrapper->Query("DELETE FROM " + name).Execute();
}

//==============================================================================


SQLKeyValueTable::SQLKeyValueTable(const std::string & name, std::shared_ptr<SQLiteWrapper> wrapper)
	: SQLTable(name, wrapper)
{
	if (wrapper->ExistTable(name) == false)
	{
		wrapper->CreateTable(name, {
			{ "key", SQLEnums::ValueDataType::String },
			{ "value", SQLEnums::ValueDataType::String }
		},
		"", false);
	}

	this->updateQuery = wrapper->Query("UPDATE " + name + " SET value=? WHERE key=?");
	this->selectQuery = wrapper->Query("SELECT value FROM " + name + " WHERE key=?");
}

SQLKeyValueTable::~SQLKeyValueTable()
{
	
}

void SQLKeyValueTable::RemoveNotRegisteredKeys()
{
	auto result = wrapper->Query("SELECT key FROM " + name + "").Select();
	for (auto & row : result)
	{
		std::string tableKey = row[0].as_string();

		bool found = false;
		for (auto & k : keys)
		{
			if (k == tableKey)
			{
				found = true;
				break;
			}
		}

		if (found == false)
		{
			this->RemoveKey(tableKey);
		}
	}
}

void SQLKeyValueTable::AddNewKeyValue(const std::string & key, const std::string & value)
{
	/*
	auto keys = wrapper->Query("PRAGMA table_info(" + name + ")").Select();
	for (auto k : keys)
	{
		int c = k.ColumnCount();
		for (int i = 0; i < c; i++)
		{
			printf("%s ", k.at(i).as_string().c_str());
			printf("%s ", k.at(i).GetColumnName().c_str());
			printf("\n");
		}
	}
	*/

	auto results = wrapper->Query("SELECT COUNT(*) FROM " + name + " WHERE key=?").Select(key);
	int count = results.GetNextRow()->at(0).as_int();
	if (count != 0)
	{		
		return;
	}

	wrapper->Query("INSERT INTO " + name + " (key, value) VALUES(?, ?)").Execute(key, value);
}

void SQLKeyValueTable::RemoveKey(const std::string & key)
{
	wrapper->Query("DELETE FROM " + name + " WHERE key=?").Execute(key);
}

void SQLKeyValueTable::UpdateValue(const std::string & key, const std::string & newValue)
{
	updateQuery.Execute(newValue, key);
};

