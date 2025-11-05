#pragma once

#include <iostream>
#include <regex>
#include <string.h>
#include <string>
using namespace std;

enum DataType { NUMBER,
				STRING,
				DATE,
				UNKNOW };

DataType conversionStringToDT(const string &s) {
	if (s == "NUMBER")
		return DataType::NUMBER;
	if (s == "STRING")
		return DataType::STRING;
	if (s == "DATE")
		return DataType::DATE;
	return DataType::UNKNOW;
};

/*string conversionDTtoString(DataType type) {
	if (type == 0)
		return "number";
	if (type == 1)
		return "text";
	if (type == 2)
		return "date";
	if (type == 3)
		throw "invalid datatype";
};*/

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
		ColumnName = nullptr;
		type = UNKNOW;
		size = 0;
		defaultValue = nullptr;

		cout << "Default constructor used";
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

}; // end of Column class
