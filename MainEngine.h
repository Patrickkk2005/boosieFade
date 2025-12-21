#pragma once
#include "CfgManager.h"
#include "CreateIndex.h"
#include "CreateTable.h"
#include "Delete.h"
#include "DisplayTable.h"
#include "DropIndex.h"
#include "DropTable.h"
#include "FileReader.h"
#include "Insert.h"
#include "Select.h"
#include "Update.h"
#include <iostream>
#include <string.h>
#include <string>
using namespace std;

class MainEngine {
  private:
	CreateTableCMD *tables;
	int tableCount;
	int tableCap;
	CreateIndexCMD *indexes;
	int indexCount;
	int indexCap;
	ConfigurationManager configManager;

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

	void increaseIndexCap() {
		if (this->indexCount < this->indexCap) {
			return;
		}
		int newCap = (this->indexCap == 0) ? 4 : this->indexCap * 2;
		CreateIndexCMD *newIndexes = new CreateIndexCMD[newCap];
		for (int i = 0; i < this->indexCount; i++) {
			newIndexes[i] = this->indexes[i];
		}
		delete[] this->indexes;
		this->indexes = newIndexes;
		this->indexCap = newCap;
	}

	void storeIndex(CreateIndexCMD &cmd) {
		if (findIndexIndex(cmd.getIndexName()) != -1) {
			throw "Index already exists!";
		}
		increaseIndexCap();
		this->indexes[this->indexCount++] = cmd;
	}

	int findIndexIndex(const char *name) {
		for (int i = 0; i < this->indexCount; i++) {
			if (strcmp(this->indexes[i].getIndexName(), name) == 0) {
				return i;
			}
		}
		return -1;
	}

	int findTableIndex(const string &name) {
		for (int i = 0; i < this->tableCount; i++) {
			if (this->tables[i].getTableName() == name) {
				return i;
			}
		}
		return -1;
	}

	string getConfigDirectory() const {
		return this->configManager.getConfigDirectory();
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
				cmd = parser.parse(command);
				storeTable(*cmd);
				// for file
				configManager.saveTableSchema(*cmd);
				cout << "Table '" << cmd->getTableName() << "' created." << endl;
				delete cmd;
			} else if (upperC.find("DROP TABLE") == 0) {
				DropTableParser parser;
				DropTableCMD *cmd = nullptr;
				cmd = parser.parse(command);
				string tableName = cmd->getTableName();
				cmd->drptbl(this->tables, this->tableCount);
				// for file
				configManager.deleteTableSchema(tableName);
				cout << "Table dropped." << endl;
				delete cmd;
			} else if (upperC.find("DISPLAY TABLE") == 0) {
				DisplayTableParser parser;
				DisplayTableCMD *cmd = nullptr;
				cmd = parser.parse(command);
				cmd->dsptbl(this->tables, this->tableCount);
				delete cmd;
			} else if (upperC.find("CREATE INDEX") == 0) {
				CreateIndexParser parser;
				CreateIndexCMD *cmd = nullptr;
				cmd = parser.parse(command);
				storeIndex(*cmd);
				cout << "Index '" << cmd->getIndexName() << "' created on table '" << cmd->getTableName() << "' column '" << cmd->getColumnName() << "'." << endl;
				delete cmd;
			} else if (upperC.find("DROP INDEX") == 0) {
				DropIndexParser parser;
				DropIndexCMD *cmd = nullptr;
				cmd = parser.parse(command);
				cmd->drpidx(this->indexes, this->indexCount);
				cout << "Index dropped." << endl;
				delete cmd;
			} else if (upperC.find("INSERT INTO") == 0) {
				InsertParser parser;
				InsertCMD *cmd = nullptr;
				cmd = parser.parse(command);
				cmd->insertInto(this->tables, this->tableCount);
				cout << *cmd << endl;
				cout << "Row inserted into table '" << cmd->getTableName() << "'." << endl;
				delete cmd;
			} else if (upperC.find("SELECT") == 0) {
				SelectParser parser;
				SelectCMD *cmd = nullptr;
				cmd = parser.parse(command);
				cmd->selectFrom(this->tables, this->tableCount);
				cout << *cmd;
				delete cmd;
			} else if (upperC.find("DELETE FROM") == 0) {
				DeleteParser parser;
				DeleteCMD *cmd = nullptr;
				cmd = parser.parse(command);
				int deleted = cmd->deleteFromWhere(this->tables, this->tableCount);
				cout << "Deleted " << deleted << " row(s) from table '" << cmd->getTableName() << "'." << endl;
				delete cmd;
			} else if (upperC.find("UPDATE") == 0) {
				UpdateParser parser;
				UpdateCMD *cmd = nullptr;
				cmd = parser.parse(command);
				int updated = cmd->updateWhere(this->tables, this->tableCount);
				cout << "Updated " << updated << " row(s) in table '" << cmd->getTableName() << "'." << endl;
				delete cmd;
			} else {
				cout << "For now only CREATE TABLE, DROP TABLE, DISPLAY TABLE, CREATE INDEX, DROP INDEX, INSERT INTO, SELECT, DELETE and UPDATE are available!" << endl;
			}
		} catch (const char *err) {
			cout << "SQL Error: " << err << endl;
		} catch (const exception &e) {
			cout << "Error: " << e.what() << endl;
		} catch (...) {
			cout << "An unexpected error occurred" << endl;
		}
	}

  public:
	MainEngine() {
		this->tables = nullptr;
		this->tableCount = 0;
		this->tableCap = 0;
		this->indexes = nullptr;
		this->indexCount = 0;
		this->indexCap = 0;
		this->configManager = ConfigurationManager();
	}

	~MainEngine() {
		delete[] this->tables;
		delete[] this->indexes;
	}

	void runapp() {
		cout << "Configuration directory: " << this->configManager.getConfigDirectory() << endl;
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
				cout << endl;
				continue;
			}

			processCMD(input);
		}
	}
};