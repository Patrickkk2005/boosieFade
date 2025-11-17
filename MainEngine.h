#pragma once
#include "CreateTable.h"
#include "DisplayTable.h"
#include "DropTable.h"
#include <iostream>
#include <string.h>
#include <string>
using namespace std;

class MainEngine {
  public:
	MainEngine() {
		this->tables = nullptr;
		this->tableCount = 0;
		this->tableCap = 0;
	}

	~MainEngine() {
		delete[] this->tables;
	}

	void runapp() {
		string input;
		cout << "SQLite ENGINE V1 (type 'exit' to quit)" << endl;
		while (true) {
			cout << "sql>> ";
			getline(cin, input);

			if (input == "exit") {
				cout << "Goodbye!" << endl;
				break;
			}

			if (input.empty()) {
				cout << "Please enter a command";
			}

			processCMD(input);
		}
	}

  private:
	CreateTableCMD *tables;
	int tableCount;
	int tableCap;

	void increaseTableCap() {
		if (this->tableCount < this->tableCap) {
			return;
		}
		int newCap = (this->tableCap == 0) ? 4 : this->tableCap * 2;
		CreateTableCMD *newTables = new CreateTableCMD[newCap];
		for (int i = 0; i < this->tableCount; i++) {
			newTables[i] = this->tables[i];
		}
		delete[] this->tables;
		this->tables = newTables;
		this->tableCap = newCap;
	}

	int findTableIndex(const string &name) {
		for (int i = 0; i < this->tableCount; i++) {
			if (this->tables[i].getTableName() == name) {
				return i;
			}
		}
		return -1;
	}

	void storeTable(CreateTableCMD &cmd) {
		if (findTableIndex(cmd.getTableName()) != -1) {
			throw "Table already exists!";
		}
		increaseTableCap();
		this->tables[this->tableCount++] = cmd;
	}

	void processCMD(string &command) {
		try {
			string upperC = command;
			for (char &c : upperC) {
				c = StringFuncs::toUpper(c);
			}
			if (upperC.find("CREATE TABLE") == 0) {
				CreateTableParser parser;
				CreateTableCMD *cmd = nullptr;
				try {
					cmd = parser.parse(command);
					storeTable(*cmd);
					cout << "Table '" << cmd->getTableName() << "' created." << endl;
					delete cmd;
				} catch (const char *err) {
					if (cmd)
						delete cmd;
					throw;
				}
			} else if (upperC.find("DROP TABLE") == 0) {
				DropTableParser parser;
				DropTableCMD *cmd = nullptr;
				cmd = parser.parse(command);
				cmd->drptbl(this->tables, this->tableCount);
				cout << "Table '" << cmd->tableName << "' dropped." << endl;
				delete cmd;
			} else if (upperC.find("DISPLAY TABLE") == 0) {
				DisplayTableParser parser;
				DisplayTableCMD *cmd = nullptr;
				cmd = parser.parse(command);
				cmd->dsptbl(this->tables, this->tableCount);
				delete cmd;
			} else {
				cout << "For now only CREATE TABLE, DROP TABLE and DISPLAY TABLE is available!" << endl;
			}
		} catch (const char *err) {
			cout << "SQL Error: " << err << endl;
		} catch (const exception &e) {
			cout << "Error: " << e.what() << endl;
		} catch (...) {
			cout << "An unexpected error occurred" << endl;
		}
	}
};