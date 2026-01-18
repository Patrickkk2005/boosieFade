#pragma once
#include "CreateTable.h"
#include "Tokenizer.h"
#include <iostream>
#include <string.h>
#include <string>
using namespace std;

class DeleteCMD {
  private:
	string tableName;
	CreateTableCMD *tableRef;
	bool hasWhere;
	string whereCol;
	string whereVal;
	int whereIdx;

	int findTableIndex(CreateTableCMD *tables, int tableCount) {
		for (int i = 0; i < tableCount; i++) {
			if (tables[i].getTableName() == this->tableName) {
				return i;
			}
		}
		return -1;
	}

	int findColIndex(CreateTableCMD &tbl, const string &colName) {
		for (int i = 0; i < tbl.getColCnt(); i++) {
			if (tbl[i].getName() != nullptr && colName == tbl[i].getName()) {
				return i;
			}
		}
		return -1;
	}

	int deleteRowsWhere(CreateTableCMD &tbl, int colIdx, const string &value) {
		if (colIdx < 0 || colIdx >= tbl.getColCnt()) {
			throw "Invalid column index";
		}
		int deleted = 0;
		for (int i = 0; i < tbl.getRowCount();) {
			if (tbl.getCell(i, colIdx) == value) {
				tbl.removeAt(i);
				deleted++;
			} else {
				i++;
			}
		}
		return deleted;
	}

  public:
	DeleteCMD() {
		this->tableName = "";
		this->tableRef = nullptr;
		this->hasWhere = false;
		this->whereCol = "";
		this->whereVal = "";
		this->whereIdx = -1;
	}
	DeleteCMD(const string &name) {
		if (name.empty()) {
			throw "name cannot be empty!";
		}
		this->tableName = name;
		this->tableRef = nullptr;
		this->hasWhere = false;
		this->whereCol = "";
		this->whereVal = "";
		this->whereIdx = -1;
	}
	DeleteCMD(const DeleteCMD &other) {
		this->tableName = other.tableName;
		this->tableRef = other.tableRef;
		this->hasWhere = other.hasWhere;
		this->whereCol = other.whereCol;
		this->whereVal = other.whereVal;
		this->whereIdx = other.whereIdx;
	}
	DeleteCMD &operator=(const DeleteCMD &other) {
		if (this == &other) {
			return *this;
		}
		this->tableName = other.tableName;
		this->tableRef = other.tableRef;
		this->hasWhere = other.hasWhere;
		this->whereCol = other.whereCol;
		this->whereVal = other.whereVal;
		this->whereIdx = other.whereIdx;
		return *this;
	}
	~DeleteCMD() {
		this->tableRef = nullptr;
	}
	void setTableName(const string &name) {
		if (name.empty()) {
			throw "name cannot be empty!";
		}
		this->tableName = name;
	}
	void setWhere(const string &col, const string &val) {
		this->hasWhere = true;
		this->whereCol = col;
		this->whereVal = val;
	}

	string getTableName() {
		return this->tableName;
	}

	int deleteFromWhere(CreateTableCMD *tables, int tableCount) {
		int tblIdx = findTableIndex(tables, tableCount);
		if (tblIdx == -1) {
			throw "Table does not exist!";
		}
		CreateTableCMD &tbl = tables[tblIdx];
		this->tableRef = &tbl;

		if (!this->hasWhere) {
			throw "WHERE clause is required for DELETE";
		}
		this->whereIdx = findColIndex(tbl, this->whereCol);
		if (this->whereIdx == -1) {
			throw "Column in WHERE clause does not exist!";
		}

		return deleteRowsWhere(tbl, this->whereIdx, this->whereVal);
	}
};

class DeleteParser {
  private:
	bool isKeyword(Token &tok, const string &keyword) {
		if (tok.type != TokenType::KEYWORD) {
			return false;
		}
		string upperTok = tok.content;
		string upperK = keyword;
		for (char &c : upperTok) {
			c = StringFuncs::toUpper(c);
		}
		for (char &c : upperK) {
			c = StringFuncs::toUpper(c);
		}
		return upperTok == upperK;
	}

  public:
	DeleteCMD *parse(const string &input) {
		Tokenizer tokenizer(input);
		TokenList *tokens = tokenizer.makeTokens();

		if (tokens->getTokenCount() < 6) {
			delete tokens;
			throw "Invalid DELETE command: too few tokens";
		}

		if (!isKeyword((*tokens)[0], "DELETE") || !isKeyword((*tokens)[1], "FROM")) {
			delete tokens;
			throw "Invalid command start!";
		}

		if ((*tokens)[2].type != TokenType::STRING) {
			delete tokens;
			throw "Missing table name!";
		}
		DeleteCMD *cmd = new DeleteCMD((*tokens)[2].content);

		int pos = 3;
		if (pos >= tokens->getTokenCount() || !isKeyword((*tokens)[pos], "WHERE")) {
			delete cmd;
			delete tokens;
			throw "DELETE requires WHERE";
		}
		pos++;

		if (pos >= tokens->getTokenCount() || ((*tokens)[pos].type != TokenType::STRING && (*tokens)[pos].type != TokenType::KEYWORD)) {
			delete cmd;
			delete tokens;
			throw "Missing column name in WHERE";
		}
		string col = (*tokens)[pos].content;
		pos++;

		if (pos >= tokens->getTokenCount() || (*tokens)[pos].type != TokenType::SYMBOL || (*tokens)[pos].content != "=") {
			delete cmd;
			delete tokens;
			throw "Missing '=' in WHERE";
		}
		pos++;

		if (pos >= tokens->getTokenCount() || ((*tokens)[pos].type != TokenType::STRING && (*tokens)[pos].type != TokenType::NUMBER && (*tokens)[pos].type != TokenType::KEYWORD)) {
			delete cmd;
			delete tokens;
			throw "Missing value in WHERE";
		}
		string val = (*tokens)[pos].content;
		cmd->setWhere(col, val);

		delete tokens;
		return cmd;
	}
}; // edn of delete parser class
