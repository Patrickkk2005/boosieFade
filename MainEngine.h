#pragma once
#include "CfgManager.h"
#include "Command.h"
#include "CreateIndex.h"
#include "CreateTable.h"
#include "Delete.h"
#include "DisplayTable.h"
#include "DropIndex.h"
#include "DropTable.h"
#include "FileManager.h"
#include "FileReader.h"
#include "Import.h"
#include "Insert.h"
#include "RepGen.h"
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
	FileManager fileManager;
	ReportGenerator reportGenerator;
	CommandFileReader fileReader;
	CommandHistory commandHistory;

  public:
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

		if (configManager.tableSchemaExists(name)) {
			CreateTableCMD *table = configManager.loadTableSchema(name);
			if (table != nullptr) {
				fileManager.loadTableData(*table);
				increaseTableCap();
				this->tables[this->tableCount] = *table;
				this->tableCount++;
				delete table;
				return this->tableCount - 1;
			}
		} else {
			cout << "No .cfg found on disk." << endl;
		}

		return -1;
	}

	string getConfigDirectory() const {
		return this->configManager.getConfigDirectory();
	}

	string getDataDirectory() const {
		return this->fileManager.getDataDir();
	}

	void storeTable(CreateTableCMD &cmd) {
		for (int i = 0; i < this->tableCount; i++) {
			if (this->tables[i].getTableName() == cmd.getTableName()) {
				throw "Table already exists!";
			}
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
				// file manipulations
				configManager.saveTableSchema(*cmd);
				fileManager.createDataFile(cmd->getTableName());
				cout << "Table '" << cmd->getTableName() << "' created." << endl;
				commandHistory.add(new CreateTableCommand(cmd->getTableName(), cmd->getColCnt()));
				delete cmd;
			} else if (upperC.find("DROP TABLE") == 0) {
				DropTableParser parser;
				DropTableCMD *cmd = nullptr;
				cmd = parser.parse(command);
				string tableName = cmd->getTableName();
				int tableIdx = findTableIndex(cmd->getTableName());
				if (tableIdx == -1) {
					delete cmd;
					throw "Table does not exist!";
				}
				cmd->drptbl(this->tables, this->tableCount);
				// file manipulations
				configManager.deleteTableSchema(tableName);
				fileManager.delDataFile(tableName);
				cout << "Table dropped." << endl;
				commandHistory.add(new DropTableCommand(tableName));
				delete cmd;
			} else if (upperC.find("DISPLAY TABLE") == 0) {
				DisplayTableParser parser;
				DisplayTableCMD *cmd = nullptr;
				cmd = parser.parse(command);
				int tableIdx = findTableIndex(cmd->tableName);
				if (tableIdx == -1) {
					delete cmd;
					throw "Table does not exist!";
				}
				cmd->dsptbl(this->tables, this->tableCount);
				// file manipulations
				reportGenerator.generateDisplayReport(cmd->tableName, this->tables[tableIdx]);
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
				int tableIdx = findTableIndex(cmd->getTableName());
				if (tableIdx == -1) {
					delete cmd;
					throw "Table does not exist!";
				}
				cmd->insertInto(this->tables, this->tableCount);
				cout << *cmd << endl;
				// file manipulations
				fileManager.saveTableData(this->tables[tableIdx]);
				cout << "Row inserted into table '" << cmd->getTableName() << "'." << endl;
				commandHistory.add(new InsertCommand(cmd->getTableName(), cmd->getValueCount()));
				delete cmd;
			} else if (upperC.find("SELECT") == 0) {
				SelectParser parser;
				SelectCMD *cmd = nullptr;
				cmd = parser.parse(command);
				int tableIdx = findTableIndex(cmd->getTableName());
				if (tableIdx == -1) {
					delete cmd;
					throw "Table does not exist!";
				}
				cmd->selectFrom(this->tables, this->tableCount);
				// file manipulations
				int *colIdxArr = new int[cmd->getColIdxCount()];
				for (int i = 0; i < cmd->getColIdxCount(); i++) {
					colIdxArr[i] = cmd->getColIdx(i);
				}
				reportGenerator.generateSelectReport(cmd->getTableName(), this->tables[tableIdx], colIdxArr, cmd->getColIdxCount(), cmd->getHasWhere(), cmd->getWhereIdx(), cmd->getWhereVal());
				delete[] colIdxArr;
				cout << *cmd;
				commandHistory.add(new SelectCommand(cmd->getTableName()));
				delete cmd;
			} else if (upperC.find("DELETE FROM") == 0) {
				DeleteParser parser;
				DeleteCMD *cmd = nullptr;
				cmd = parser.parse(command);
				int tableIdx = findTableIndex(cmd->getTableName());
				if (tableIdx == -1) {
					delete cmd;
					throw "Table does not exist!";
				}
				int deleted = cmd->deleteFromWhere(this->tables, this->tableCount);
				// file manipulations
				fileManager.saveTableData(this->tables[tableIdx]);
				cout << "Deleted " << deleted << " row(s) from table '" << cmd->getTableName() << "'." << endl;
				delete cmd;
			} else if (upperC.find("UPDATE") == 0) {
				UpdateParser parser;
				UpdateCMD *cmd = nullptr;
				cmd = parser.parse(command);
				int tableIdx = findTableIndex(cmd->getTableName());
				if (tableIdx == -1) {
					delete cmd;
					throw "Table does not exist!";
				}
				int updated = cmd->updateWhere(this->tables, this->tableCount);
				// file manipulations
				fileManager.saveTableData(this->tables[tableIdx]);
				cout << "Updated " << updated << " row(s) in table '" << cmd->getTableName() << "'." << endl;
				delete cmd;
			} else if (upperC.find("IMPORT") == 0) {
				ImportParser parser;
				ImportCMD *cmd = nullptr;
				cmd = parser.parse(command);
				int tableIdx = findTableIndex(cmd->getTableName());
				if (tableIdx == -1) {
					delete cmd;
					throw "Table does not exist!";
				}
				cmd->importCSV(this->tables, this->tableCount);
				// file manipulations
				fileManager.saveTableData(this->tables[tableIdx]);
				cout << *cmd << endl;
				delete cmd;
			} else if (upperC == "HISTORY") {
				commandHistory.printHistory();
			} else {
				cout << "Available: CREATE TABLE, DROP TABLE, DISPLAY TABLE, CREATE INDEX, DROP INDEX, INSERT INTO, SELECT, DELETE, UPDATE, IMPORT, HISTORY" << endl;
			}
		} catch (const char *err) {
			cout << "SQL Error: " << err << endl;
		} catch (const exception &e) {
			cout << "Error: " << e.what() << endl;
		} catch (...) {
			cout << "An error occurred" << endl;
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
		this->fileManager = FileManager();
	}

	~MainEngine() {
		delete[] this->tables;
		delete[] this->indexes;
	}

	void commandsFromFiles(int argc, char **argv) {
		if (argc <= 1) {
			return;
		}

		fileReader.loadCommandsFromArguments(argc, argv);
		if (fileReader.hasCommands()) {
			cout << "Executing commands from files" << endl;
			for (int i = 0; i < fileReader.getCommandCount(); i++) {
				string cmd = fileReader.getCommand(i);
				processCMD(cmd);
			}
			cout << "Finished executing commands from files"
				 << endl;
		}
	}

	void runapp(int argc, char **argv) {
		cout << "Configuration directory: " << this->configManager.getConfigDirectory() << endl;
		cout << "Data directory: " << this->fileManager.getDataDir() << endl;

		if (argc > 1) {
			commandsFromFiles(argc, argv);
		}

		string input;
		cout << "SQLite ENGINE V3 (type 'exit' to quit)" << endl;
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