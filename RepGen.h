#pragma once

#include "CreateTable.h"
#include "Tokenizer.h"
#include <fstream>
#include <iostream>
#include <string>
using namespace std;

class ReportGenerator {
  private:
	string reportDir;

  public:
	static int selectCounter;
	static int displayCounter;

	ReportGenerator() {
		this->reportDir = "./";
	}

	ReportGenerator(const string &dir) {
		this->reportDir = dir;
		if (this->reportDir[this->reportDir.length() - 1] != '/') {
			this->reportDir += "/";
		}
	}

	~ReportGenerator() {
	}

	void setReportDirectory(const string &dir) {
		this->reportDir = dir;
		if (this->reportDir[this->reportDir.length() - 1] != '/') {
			this->reportDir += "/";
		}
	}

	string getReportDirectory() const {
		return this->reportDir;
	}

	void generateSelectReport(const string &tableName, CreateTableCMD &table, int *colIdx, int colIdxCnt, bool hasWhere, int whereIdx, const string &whereVal) {
		int selID = selectCounter++;

		string filename = this->reportDir + "SELECT_" + StringFuncs::intTOstring(selID) + ".txt";

		ofstream file(filename);

		if (!file.is_open()) {
			throw "Cannot create SELECT report file";
		}

		file << "===== SELECT REPORT #" << selID << " =====" << endl;
		file << "Table: " << tableName << endl;
		file << "========================================" << endl;
		file << "Selected Columns:" << endl;
		for (int i = 0; i < colIdxCnt; i++) {
			file << "  - " << table[colIdx[i]].getName() << endl;
		}
		file << endl;
		if (hasWhere) {
			file << "WHERE condition: " << table[whereIdx].getName() << " = " << whereVal << endl;
		} else {
			file << "WHERE condition: none" << endl;
		}
		file << "----------------------------------------" << endl;
		file << "Rows:" << endl;
		int rowsPrinted = 0;
		for (int i = 0; i < table.getRowCount(); i++) {
			string *row = table.getRow(i);
			if (hasWhere) {
				if (row[whereIdx] != whereVal) {
					continue;
				}
			}
			rowsPrinted++;
			file << "Row " << rowsPrinted << ": ";
			for (int j = 0; j < colIdxCnt; j++) {
				int colPosition = colIdx[j];
				file << table[colPosition].getName() << "=" << row[colPosition];
				if (j < colIdxCnt - 1) {
					file << " | ";
				}
			}
			file << endl;
		}
		file << "----------------------------------------" << endl;
		file << "Total rows selected: " << rowsPrinted << endl;
		file.close();
		cout << "Report generated: SELECT_" << selID << ".txt" << endl;
	}

	void generateDisplayReport(const string &tableName, CreateTableCMD &table) {
		int disID = displayCounter++;

		string filename = this->reportDir + "DISPLAY_" + StringFuncs::intTOstring(disID) + ".txt";

		ofstream file(filename);

		if (!file.is_open()) {
			throw "Cannot create DISPLAY report file";
		}

		file << "===== DISPLAY TABLE REPORT #" << disID << " =====" << endl;
		file << "Table Name: " << tableName << endl;
		file << "================================================" << endl;
		file << "Table Structure:" << endl;
		file << "  Column Count: " << table.getColCnt() << endl;
		file << "  Row Count: " << table.getRowCount() << endl;
		file << endl;
		file << "Column Details:" << endl;
		for (int i = 0; i < table.getColCnt(); i++) {
			Column &col = table[i];
			file << "  Name: " << col.getName() << endl;
			file << "      Size: " << col.getSize() << endl;
		}
		file << endl;
		file << "Table Data:" << endl;
		file << "================================================" << endl;
		if (table.getRowCount() == 0) {
			file << "(Empty table - no data)" << endl;
		} else {
			for (int i = 0; i < table.getRowCount(); i++) {
				string *row = table.getRow(i);
				file << "Row " << (i + 1) << ": ";
				for (int j = 0; j < table.getColCnt(); j++) {
					file << row[j];
					if (j < table.getColCnt() - 1) {
						file << " | ";
					}
				}
				file << endl;
			}
		}
		file << "================================================" << endl;
		file << "Total rows in table: " << table.getRowCount() << endl;
		file.close();
		cout << "Report generated: DISPLAY_" << disID << ".txt" << endl;
	}

	int getSelectCount() const {
		return selectCounter;
	}

	int getDisplayCount() const {
		return displayCounter;
	}

}; // end of RepGen clas
