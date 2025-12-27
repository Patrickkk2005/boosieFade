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
	const int selectID;
	const int displayID;

  public:
	static int selectCounter;
	static int displayCounter;

	ReportGenerator() : selectID(++selectCounter), displayID(++displayCounter) {
		this->reportDir = "./";
	}

	ReportGenerator(const string &dir) : selectID(++selectCounter), displayID(++displayCounter) {
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

	void generateSelectReport(const string &tableName, CreateTableCMD &table) {

		string filename = this->reportDir + "SELECT_" + StringFuncs::intTOstring(this->selectID) + ".txt";

		ofstream file(filename);

		if (!file.is_open()) {
			throw "Cannot create SELECT report file";
		}

		file << "===== SELECT REPORT #" << this->selectID << " =====" << endl;
		file << "Table: " << tableName << endl;
		file << "========================================" << endl;
		file << "Columns:" << endl;
		for (int i = 0; i < table.getColCnt(); i++) {
			file << "  [" << i << "] " << table[i].getName() << endl;
		}
		file << endl;
		file << "Rows (" << table.getRowCount() << " total):" << endl;
		file << "----------------------------------------" << endl;
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
		file << "----------------------------------------" << endl;
		file << "Total rows selected: " << table.getRowCount() << endl;
		file.close();
		cout << "Report generated: SELECT_" << this->selectID << ".txt" << endl;
	}

	void generateDisplayReport(const string &tableName, CreateTableCMD &table) {

		string filename = this->reportDir + "DISPLAY_" + StringFuncs::intTOstring(this->displayID) + ".txt";

		ofstream file(filename);

		if (!file.is_open()) {
			throw "Cannot create DISPLAY report file";
		}

		file << "===== DISPLAY TABLE REPORT #" << this->displayID << " =====" << endl;
		file << "Table Name: " << tableName << endl;
		file << "================================================" << endl;
		file << "Table Structure:" << endl;
		file << "  Column Count: " << table.getColCnt() << endl;
		file << "  Row Count: " << table.getRowCount() << endl;
		file << endl;
		file << "Column Details:" << endl;
		for (int i = 0; i < table.getColCnt(); i++) {
			Column &col = table[i];
			file << "  [" << i << "] Name: " << col.getName() << endl;
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
		cout << "Report generated: DISPLAY_" << this->displayID << ".txt" << endl;
	}

	int getSelectCount() const {
		return this->selectID;
	}

	int getDisplayCount() const {
		return this->displayID;
	}

}; // end of RepGen class
