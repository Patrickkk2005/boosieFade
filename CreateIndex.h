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

	void deepCpyFunc(char *&news, char *olds) {
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
	CreateIndexCMD() {
		this->indexName = nullptr;
		this->tableName = "";
		this->columnName = "";
	}

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
	bool ISkeyword(Token &token, const string &keyword) {
		if (token.type != TokenType::KEYWORD) {
			return false;
		}

		string upperTok = token.content;
		string upperKey = keyword;
		for (char &c : upperTok) {
			c = StringFuncs::toUpper(c);
		}
		for (char &c : upperKey) {
			c = StringFuncs::toUpper(c);
		}

		return upperTok == upperKey;
	}

  public:
	CreateIndexCMD *parse(const string &input) {
		Tokenizer tokenizer(input);
		TokenList *tokens = nullptr;
		tokens = tokenizer.makeTokens();
		if (tokens->getTokenCount() < 6) {
			delete tokens;
			throw "Invalid CREATE INDEX command: to few tokens!";
		}
		if (!ISkeyword((*tokens)[0], "CREATE") || !ISkeyword((*tokens)[1], "INDEX")) {
			delete tokens;
			throw "Invlid command start!";
		}
		if ((*tokens)[2].type != TokenType::STRING) {
			delete tokens;
			throw "Invalid token!";
		}
		char *idxName = new char[(*tokens)[2].content.length() + 1];
		strcpy(idxName, (*tokens)[2].content.c_str());

		int pos = 3;

		if ((*tokens)[pos].type != TokenType::KEYWORD || (*tokens)[pos].content != "ON") {
			delete tokens;
			throw "forgot to put ON!";
		}

		pos++;

		if ((*tokens)[pos].type != TokenType::STRING) {
			delete tokens;
			throw "Invalid table name!";
		}

		string tblName;
		tblName = (*tokens)[pos].content;
		pos++;

		if ((*tokens)[pos].type != TokenType::SYMBOL || (*tokens)[pos].content != "(") {
			delete tokens;
			throw "Expected '(' before column definition!";
		}

		pos++;

		if ((*tokens)[pos].type != TokenType::STRING) {
			delete tokens;
			throw "Invalid table name!";
		}

		string colName;
		colName = (*tokens)[pos].content;
		pos++;

		if ((*tokens)[pos].type != TokenType::SYMBOL || (*tokens)[pos].content != ")") {
			delete tokens;
			throw "Expected ')' after default value!";
		}
		pos++;

		CreateIndexCMD *idx = new CreateIndexCMD(idxName, tblName, colName);
		delete[] idxName;
		delete tokens;
		return idx;
	}

}; // end of create inde parser class