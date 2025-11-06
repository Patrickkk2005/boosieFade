#pragma once

#include "Tokenizer.h"
#include <iostream>
#include <regex>
#include <string.h>
#include <string>
using namespace std;

enum DataType { INTEGER,
				FLOAT,
				TEXT,
				UNKNOW };

DataType conversionStringToDT(const string &s) {
	if (s == "INTEGER")
		return DataType::INTEGER;
	if (s == "FLOAT")
		return DataType::FLOAT;
	if (s == "TEXT")
		return DataType::TEXT;
	return DataType::UNKNOW;
};

string conversionDTtoString(DataType type) {
	if (type == INTEGER)
		return "integer";
	if (type == FLOAT)
		return "float";
	if (type == TEXT)
		return "text";
	if (type == UNKNOW)
		throw "invalid datatype";
	return "UNKNOWN";
};

class Column {
  private:
	char *ColumnName;
	int size;
	char *defaultValue;
	DataType type;

	void deepCopyFunc(char *&newstr, const char *oldstr) {
		if (newstr != nullptr) {
			delete[] newstr;
			newstr = nullptr;
		}
		if (oldstr != nullptr) {
			newstr = new char[strlen(oldstr) + 1];
			strcpy(newstr, oldstr);
		}
	}

  public:
	Column() {
		this->ColumnName = nullptr;
		this->type = UNKNOW;
		this->size = 0;
		this->defaultValue = nullptr;

		cout << "Default constructor used" << endl;
	}

	Column(const char *ColumnName, const char *typeString, int size, const char *defaultValue) {
		this->ColumnName = nullptr;
		this->defaultValue = nullptr;

		setName(ColumnName);
		setDatatype(typeString);
		setSize(size);
		setDefVal(defaultValue);

		cout << "Para Constructor used" << endl;
	}

	Column(const Column &other) {
		this->ColumnName = nullptr;
		this->defaultValue = nullptr;

		deepCopyFunc(this->ColumnName, other.ColumnName);
		deepCopyFunc(this->defaultValue, other.defaultValue);

		this->size = other.size;
		this->type = other.type;
	}

	Column &operator=(const Column &other) {
		if (this == &other) {
			return *this;
		}
		deepCopyFunc(this->ColumnName, other.ColumnName);
		deepCopyFunc(this->defaultValue, other.defaultValue);

		this->size = other.size;
		this->type = other.type;

		return *this;
	}

	~Column() {
		delete[] this->ColumnName;
		delete[] this->defaultValue;
	}

	// GETTERS:

	char *getName() {
		return ColumnName;
	}

	DataType getType() {
		return type;
	}

	int getSize() {
		return size;
	}

	char *getDefVal() {
		return defaultValue;
	}

	// SETTERS:

	void setName(const char *name) {
		deepCopyFunc(this->ColumnName, name);
	}

	void setDefVal(const char *val) {
		deepCopyFunc(this->defaultValue, val);
	}

	void setSize(int value) {
		if (value <= 0)
			throw "Size must be greater than 0!";
		this->size = value;
	}

	void setDatatype(const char *datatype) {
		DataType newtype = conversionStringToDT(datatype);
		if (newtype == DataType::UNKNOW) {
			throw "Invalid DataType!";
		}
		this->type = newtype;
	}

	friend ostream &operator<<(ostream &os, const Column &col) {
		os << "Column Name: " << col.ColumnName << endl
		   << ", Type: " << col.type << endl
		   << ", Size: " << col.size << endl
		   << ", Default Value: " << col.defaultValue << endl;
		return os;
	}

	bool operator==(const Column &other) {
		return (this->ColumnName != nullptr && other.ColumnName != nullptr && strcmp(this->ColumnName, other.ColumnName) == 0) && this->type == other.type && this->size == other.size;
	}

	explicit operator const char *() const {
		return this->ColumnName;
	}

	bool operator!() const {
		return this->ColumnName == nullptr;
	}

	Column operator+(int value) const {
		Column newCol = *this;
		newCol.size = this->size + value;
		return newCol;
	}

}; // end of Column class

class CreateTableCMD {
  private:
	string tableName;
	bool IFnotExist;
	Column *columns;
	int columnCnt;
	int columnCap;

  public:
	static int commandCounter;
	const int cmdID;

	CreateTableCMD() : cmdID(++commandCounter) {
		this->IFnotExist = false;
		this->columns = nullptr;
		this->columnCnt = 0;
		this->columnCap = 0;
	}

	CreateTableCMD(const char *name, bool ifq = false) : cmdID(++commandCounter) {
		this->tableName = name;
		if (tableName.empty()) {
			throw "Name cannot be empty!";
		}
		this->IFnotExist = ifq;
		this->columns = nullptr;
		this->columnCnt = 0;
		this->columnCap = 0;
	}

	CreateTableCMD(const CreateTableCMD &otherTable) : cmdID(++commandCounter) {
		this->tableName = otherTable.tableName;
		this->IFnotExist = otherTable.IFnotExist;
		this->columnCnt = otherTable.columnCnt;
		this->columnCap = otherTable.columnCap;

		this->columns = new Column[this->columnCap];
		for (int i = 0; i < columnCnt; i++) {
			this->columns[i] = otherTable.columns[i];
		}
	}

	CreateTableCMD &operator=(const CreateTableCMD &otherTable) {
		if (this == &otherTable) {
			return *this;
		}

		delete[] this->columns;

		this->tableName = otherTable.tableName;
		this->IFnotExist = otherTable.IFnotExist;
		this->columnCnt = otherTable.columnCnt;
		this->columnCap = otherTable.columnCap;

		this->columns = new Column[this->columnCap];
		for (int i = 0; i < columnCnt; i++) {
			this->columns[i] = otherTable.columns[i];
		}

		return *this;
	}

	~CreateTableCMD() {
		delete[] this->columns;
	}

	void addColumn(const Column &col) {
		if (columnCnt >= columnCap) {
			int newCap;
			if (columnCap == 0) {
				newCap = 5;
			} else
				newCap = newCap * 2;
			Column *newCol = new Column[newCap];

			if (columns != nullptr)
				for (int i = 0; i < newCap; i++) {
					newCol[i] = this->columns[i];
				}

			delete[] this->columns;

			this->columns = newCol;
			this->columnCap = newCap;
		}
		this->columns[columnCnt++] = col;
	}

	// GETTERS:

	string getTableName() {
		return this->tableName;
	}

	int getColCnt() {
		return this->columnCnt;
	}

	Column &operator[](int i) {
		if (i < 0 || i >= columnCnt) {
			throw "Invalid Index";
		}
		return columns[i];
	}

	friend ostream &operator<<(ostream &os, const CreateTableCMD &cmd) {
		os << endl
		   << "Create Table ID: " << cmd.cmdID << endl
		   << "Table Name: " << cmd.tableName << endl
		   << "Column Count: " << cmd.columnCnt << endl
		   << "Columns: " << endl;
		for (int i = 0; i < cmd.columnCnt; i++) {
			os << "	" << i + 1 << ". " << cmd.columns[i] << endl;
		}
		os << "_______________________________________" << endl;
		return os;
	}

}; // end of create table command class

class CreateTableParser {

}; // end of parser class
