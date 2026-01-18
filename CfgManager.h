#pragma once

#include "CreateTable.h"
#include "Tokenizer.h"
#include <fstream>
#include <iostream>
#include <string.h>
#include <string>
using namespace std;

class ConfigurationManager {
  private:
	string configDir;

	// Format: columnName|dataType|size|defaultVal
	bool parseConfigLine(const string &line, string &colName, string &dataType, int &size, string &defVal) {
		int pos1 = line.find('|');
		if (pos1 == string::npos)
			return false;

		int pos2 = line.find('|', pos1 + 1);
		if (pos2 == string::npos)
			return false;

		int pos3 = line.find('|', pos2 + 1);
		if (pos3 == string::npos)
			return false;

		colName = line.substr(0, pos1);
		dataType = line.substr(pos1 + 1, pos2 - pos1 - 1);
		size = StringFuncs::stringTOint(line.substr(pos2 + 1, pos3 - pos2 - 1));
		defVal = line.substr(pos3 + 1);

		return true;
	}

	string getConfigFilename(const string &tableName) {
		return tableName + ".cfg";
	}

  public:
	ConfigurationManager() {
		this->configDir = "./";
	}

	ConfigurationManager(const string &dir) {
		this->configDir = dir;
		if (this->configDir[this->configDir.length() - 1] != '/') {
			this->configDir += "/";
		}
	}

	~ConfigurationManager() {
	}

	void saveTableSchema(CreateTableCMD &table) {
		string filename = this->configDir + getConfigFilename(table.getTableName());
		ofstream file(filename, ios::trunc);

		if (!file.is_open()) {
			throw "Cannot open configuration file for table";
		}

		for (int i = 0; i < table.getColCnt(); i++) {
			Column &col = const_cast<CreateTableCMD &>(table)[i];
			file << col.getName() << "|"
				 << CreateTableUtils::conversionDTtoString(col.getType()) << "|"
				 << col.getSize() << "|"
				 << (col.getDefVal() != nullptr ? col.getDefVal() : "NULL")
				 << "\n";
		}

		file.close();
		cout << "Configuration saved for table '" << table.getTableName() << "'." << endl;
	}

	CreateTableCMD *loadTableSchema(const string &tableName) {
		string filename = this->configDir + getConfigFilename(tableName);
		ifstream file(filename);

		if (!file.is_open()) {
			return nullptr;
		}

		CreateTableCMD *table = new CreateTableCMD(tableName.c_str());
		string line;

		while (getline(file, line)) {
			if (line.empty())
				continue;

			string colName, dataType, defVal;
			int size;

			if (!parseConfigLine(line, colName, dataType, size, defVal)) {
				file.close();
				delete table;
				throw "Invalid format";
			}

			if (defVal == "NULL") {
				defVal = "";
			}

			Column col(colName.c_str(), dataType.c_str(), size, defVal.c_str());
			table->addColumn(col);
		}

		file.close();
		cout << "Configuration loaded for table '" << tableName << "'." << endl;
		return table;
	}

	bool tableSchemaExists(const string &tableName) {
		string filename = this->configDir + getConfigFilename(tableName);
		ifstream file(filename);
		return file.good();
	}

	void deleteTableSchema(const string &tableName) {
		string filename = this->configDir + getConfigFilename(tableName);
		if (remove(filename.c_str()) == 0) {
			cout << "Configuration deleted for table '" << tableName << "'." << endl;
		}
	}

	void setConfigDirectory(const string &dir) {
		this->configDir = dir;
		if (this->configDir[this->configDir.length() - 1] != '/') {
			this->configDir += "/";
		}
	}

	string getConfigDirectory() const {
		return this->configDir;
	}

}; // end of Cfgmanager class
