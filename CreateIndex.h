#pragma once
#include "CreateTable.h"
#include "Tokenizer.h"
#include <iostream>
#include <string>

using namespace std;

class CreateIndexCMD {
  private:
	char *indexName;
	string tableName;
	string columnName;

	void deepCpyFunc(char *news, char *olds) {
		if (news != nullptr) {
			delete[] news;
			news = nullptr;
		}
		if (olds != nullptr) {
			news = new char[strlen(olds) + 1];
			strcpy(news, olds);
		}
	}

	int findTblIndex(CreateTableCMD *tables, int tableCount) {
		for (int i = 0; i < tableCount; i++) {
			if (tables[i].getTableName() == this->tableName) {
				return i;
			}
		}
		return -1;
	}

	int findColIndex(CreateTableCMD &table) {
		for (int i = 0; i < table.getColCnt(); i++) {
			if (string(table[i].getName()) == this->columnName) {
				return i;
			}
		}
		return -1;
	}

  public:
	CreateIndexCMD(char *indexName, const string &tableName, const string &columnName) {
		this->indexName = nullptr;
		if (indexName == nullptr || tableName.empty() || columnName.empty()) {
			throw "A fild is empty!";
		}
		deepCpyFunc(this->indexName, indexName);
		this->tableName = tableName;
		this->columnName = columnName;
	}

	~CreateIndexCMD() {
		delete[] this->indexName;
	}

	// GETTERS:

	char *getIndexName() {
		return this->indexName;
	}

	string getTableName() {
		return this->tableName;
	}

	string getColumnName() {
		return this->columnName;
	}

	void createIdx(CreateTableCMD *tables, int tableCnt) {
	}

	void createIndex(CreateTableCMD *tables, int tableCount) {
		int tI = findTblIndex(tables, tableCount);
		if (tI == -1) {
			throw "table doesn't exist!";
		}
		int cI = findColIndex(tables[tI]);
		if (cI == -1) {
			throw "column doesn't exist!";
		}
	}

}; // end of create index command class

class CreateIndexParser {
  private:
}; // end of create inde parser class