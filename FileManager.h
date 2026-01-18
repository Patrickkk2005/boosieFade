#pragma once

#include "CreateTable.h"
#include <fstream>
#include <iostream>
#include <string.h>
#include <string>
using namespace std;

class FileManager {
  private:
	string dataDir;

	string getDataFilename(const string &tableName) {
		return tableName + ".dat";
	}

	void writeStringToFile(ofstream &file, const string &str) {
		int len = str.length();
		file.write((const char *)&len, sizeof(int));
		file.write(str.c_str(), len);
	}

	bool readStringFromFile(ifstream &file, string &str) {
		int len;
		file.read((char *)&len, sizeof(int));

		if (file.fail() || len < 0) {
			return false;
		}

		char *buffer = new char[len + 1];
		file.read(buffer, len);
		buffer[len] = '\0';
		str = buffer;
		delete[] buffer;

		return !file.fail();
	}

  public:
	FileManager() {
		this->dataDir = "./";
	}

	FileManager(const string &dir) {
		this->dataDir = dir;
		if (this->dataDir[this->dataDir.length() - 1] != '/') {
			this->dataDir += "/";
		}
	}

	~FileManager() {
	}

	void createDataFile(const string &tableName) {
		string filename = this->dataDir + getDataFilename(tableName);
		ofstream file(filename, ios::binary | ios::trunc);

		if (!file.is_open()) {
			throw "Cannot create data file for table";
		}

		file.close();
		cout << "Data file created for table '" << tableName << "'." << endl;
	}

	void saveTableData(CreateTableCMD &table) {
		string filename = this->dataDir + getDataFilename(table.getTableName());
		ofstream file(filename, ios::binary | ios::trunc);

		if (!file.is_open()) {
			throw "Cannot open data file for writing";
		}

		int rowCount = table.getRowCount();
		file.write((const char *)&rowCount, sizeof(int));

		for (int i = 0; i < rowCount; i++) {
			string *row = table.getRow(i);
			int colCount = table.getColCnt();

			file.write((const char *)&colCount, sizeof(int));

			for (int j = 0; j < colCount; j++) {
				writeStringToFile(file, row[j]);
			}
		}

		file.close();
	}

	bool loadTableData(CreateTableCMD &table) {
		string filename = this->dataDir + getDataFilename(table.getTableName());
		ifstream file(filename, ios::binary);

		if (!file.is_open()) {
			return false;
		}

		int rowCount;
		file.read((char *)&rowCount, sizeof(int));

		if (file.fail()) {
			file.close();
			return false;
		}

		for (int i = 0; i < rowCount; i++) {
			int colCount;
			file.read((char *)&colCount, sizeof(int));

			if (file.fail()) {
				file.close();
				return false;
			}

			string *row = new string[colCount];

			for (int j = 0; j < colCount; j++) {
				if (!readStringFromFile(file, row[j])) {
					delete[] row;
					file.close();
					return false;
				}
			}

			table.addRow(row, colCount);
			delete[] row;
		}

		file.close();
		return true;
	}

	void delDataFile(const string &tableName) {
		string filename = this->dataDir + getDataFilename(tableName);
		if (remove(filename.c_str()) == 0) {
			cout << "Data file deleted for table '" << tableName << "'." << endl;
		}
	}

	bool dataFileExists(const string &tableName) {
		string filename = this->dataDir + getDataFilename(tableName);
		ifstream file(filename, ios::binary);
		return file.good();
	}

	void setDataDir(const string &dir) {
		this->dataDir = dir;
		if (this->dataDir[this->dataDir.length() - 1] != '/') {
			this->dataDir += "/";
		}
	}

	string getDataDir() const {
		return this->dataDir;
	}

}; // end of File Manager class
