#pragma once

#include "CreateTable.h"
#include <fstream>
#include <iostream>
#include <string>
using namespace std;

class CSVImporter {
  private:
	char separator;

	string *splitLine(const string &line, int &count) {
		int sepCount = 1;
		for (int i = 0; i < line.length(); i++) {
			if (line[i] == separator) {
				sepCount++;
			}
		}

		string *values = new string[sepCount];
		count = 0;
		string current = "";

		for (int i = 0; i < line.length(); i++) {
			if (line[i] == separator) {
				values[count++] = current;
				current = "";
			} else {
				current += line[i];
			}
		}
		if (current.length() > 0) {
			values[count++] = current;
		}

		return values;
	}

  public:
	CSVImporter() {
		this->separator = ',';
	}

	CSVImporter(char sep) {
		this->separator = sep;
	}

	~CSVImporter() {
	}

	void setSeparator(char sep) {
		this->separator = sep;
	}

	char getSeparator() const {
		return this->separator;
	}

	void importCSVFile(const string &filename, CreateTableCMD &table) {
		ifstream file(filename);

		if (!file.is_open()) {
			throw "Cannot open CSV file";
		}

		string line;
		int lineCnt = 0;

		while (getline(file, line)) {
			lineCnt++;
			int colCount = 0;
			string *values = splitLine(line, colCount);

			if (colCount != table.getColCnt()) {
				delete[] values;
				file.close();
				throw "CSV not good";
			}

			table.addRow(values, colCount);
			delete[] values;
		}

		file.close();
		cout << "Imported " << lineCnt << " row(s) from '" << filename << "'." << endl;
	}
};
