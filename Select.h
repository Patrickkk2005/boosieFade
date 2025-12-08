#pragma once
#include "CreateTable.h"
#include "Tokenizer.h"
#include <iostream>
#include <string>
using namespace std;

class SelectCMD {
  private:
	string tableName;
	CreateTableCMD *tableRef;
	string *requestedCols;
	int reqCnt;
	int *colIdx;
	int colIdxCnt;
	bool selectAll;
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

	void filterColumns() {
		if (this->tableRef == nullptr) {
			return;
		}
		if (this->colIdx != nullptr) {
			delete[] this->colIdx;
			this->colIdx = nullptr;
			this->colIdxCnt = 0;
		}
		if (this->selectAll) {
			this->colIdxCnt = this->tableRef->getColCnt();
			if (this->colIdxCnt > 0) {
				this->colIdx = new int[this->colIdxCnt];
				for (int i = 0; i < this->colIdxCnt; i++) {
					this->colIdx[i] = i;
				}
			}
			return;
		}
		this->colIdxCnt = this->reqCnt;
		if (this->colIdxCnt > 0) {
			this->colIdx = new int[this->colIdxCnt];
			for (int i = 0; i < this->reqCnt; i++) {
				int idx = findColIndex(*this->tableRef, this->requestedCols[i]);
				if (idx == -1) {
					throw "Requested column does not exist!";
				}
				this->colIdx[i] = idx;
			}
		}
	}

  public:
	SelectCMD() {
		this->tableName = "";
		this->tableRef = nullptr;
		this->requestedCols = nullptr;
		this->reqCnt = 0;
		this->colIdx = nullptr;
		this->colIdxCnt = 0;
		this->selectAll = false;
		this->hasWhere = false;
		this->whereIdx = -1;
	}

	SelectCMD(const string &name) {
		if (name.empty()) {
			throw "missing table name";
		}
		this->tableName = name;
		this->tableRef = nullptr;
		this->requestedCols = nullptr;
		this->reqCnt = 0;
		this->colIdx = nullptr;
		this->colIdxCnt = 0;
		this->selectAll = false;
		this->hasWhere = false;
		this->whereIdx = -1;
	}

	SelectCMD(const SelectCMD &other) {
		this->tableName = other.tableName;
		this->tableRef = other.tableRef;
		this->reqCnt = other.reqCnt;
		this->requestedCols = nullptr;
		if (this->reqCnt > 0 && other.requestedCols != nullptr) {
			this->requestedCols = new string[this->reqCnt];
			for (int i = 0; i < this->reqCnt; i++) {
				this->requestedCols[i] = other.requestedCols[i];
			}
		}
		this->selectAll = other.selectAll;
		this->hasWhere = other.hasWhere;
		this->whereCol = other.whereCol;
		this->whereVal = other.whereVal;
		this->whereIdx = other.whereIdx;
		this->colIdxCnt = other.colIdxCnt;
		this->colIdx = nullptr;
		if (this->colIdxCnt > 0 && other.colIdx != nullptr) {
			this->colIdx = new int[this->colIdxCnt];
			for (int i = 0; i < this->colIdxCnt; i++) {
				this->colIdx[i] = other.colIdx[i];
			}
		}
	}

	SelectCMD &operator=(const SelectCMD &other) {
		if (this == &other) {
			return *this;
		}
		delete[] this->requestedCols;
		delete[] this->colIdx;
		this->tableName = other.tableName;
		this->tableRef = other.tableRef;
		this->reqCnt = other.reqCnt;
		this->requestedCols = nullptr;
		if (this->reqCnt > 0 && other.requestedCols != nullptr) {
			this->requestedCols = new string[this->reqCnt];
			for (int i = 0; i < this->reqCnt; i++) {
				this->requestedCols[i] = other.requestedCols[i];
			}
		}
		this->selectAll = other.selectAll;
		this->hasWhere = other.hasWhere;
		this->whereCol = other.whereCol;
		this->whereVal = other.whereVal;
		this->whereIdx = other.whereIdx;
		this->colIdxCnt = other.colIdxCnt;
		this->colIdx = nullptr;
		if (this->colIdxCnt > 0 && other.colIdx != nullptr) {
			this->colIdx = new int[this->colIdxCnt];
			for (int i = 0; i < this->colIdxCnt; i++) {
				this->colIdx[i] = other.colIdx[i];
			}
		}
		return *this;
	}

	~SelectCMD() {
		delete[] this->requestedCols;
		delete[] this->colIdx;
	}

	void selectFrom(CreateTableCMD *tables, int tableCount) {
		int idx = findTableIndex(tables, tableCount);
		if (idx == -1) {
			throw "Table does not exist!";
		}
		this->tableRef = &tables[idx];
		if (this->hasWhere) {
			this->whereIdx = findColIndex(*this->tableRef, this->whereCol);
			if (this->whereIdx == -1) {
				throw "WHERE column doenst exist!";
			}
		}
		filterColumns();
	}

	string getTableName() {
		return this->tableName;
	}

	friend ostream &operator<<(ostream &os, SelectCMD &cmd) {
		CreateTableCMD &tbl = *cmd.tableRef;
		os << "Selected rows from table '" << tbl.getTableName() << "':" << endl;
		for (int i = 0; i < tbl.getRowCount(); i++) {
			string *row = tbl.getRow(i);
			if (cmd.hasWhere) {
				if (cmd.whereIdx < 0 || cmd.whereIdx >= tbl.getColCnt()) {
					continue;
				}
				if (row[cmd.whereIdx] != cmd.whereVal) {
					continue;
				}
			}
			os << "Row " << i + 1 << ": ";
			for (int j = 0; j < cmd.colIdxCnt; j++) {
				int colPosition = cmd.colIdx[j];
				os << tbl[colPosition].getName() << "=" << row[colPosition];
				if (j < cmd.colIdxCnt - 1) {
					os << ", ";
				}
			}
			os << endl;
		}
		return os;
	}

	void setColumnsAll() {
		this->selectAll = true;
	}

	void addRequestedColumn(const string &col) {
		string *newCols = new string[this->reqCnt + 1];
		for (int i = 0; i < this->reqCnt; i++) {
			newCols[i] = this->requestedCols[i];
		}
		newCols[this->reqCnt] = col;
		delete[] this->requestedCols;
		this->requestedCols = newCols;
		this->reqCnt++;
	}

	void setWhere(const string &col, const string &val) {
		this->hasWhere = true;
		this->whereCol = col;
		this->whereVal = val;
	}

	int getRequestedCount() {
		return this->reqCnt;
	}

	void setTableName(const string &name) {
		this->tableName = name;
	}
}; // end of sekect cmd class

class SelectParser {
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
	SelectCMD *parse(const string &input) {
		Tokenizer tokenizer(input);
		TokenList *tokens = tokenizer.makeTokens();

		if (tokens->getTokenCount() < 4) {
			delete tokens;
			throw "Invalid SELECT command: too few tokens";
		}

		if (!isKeyword((*tokens)[0], "SELECT")) {
			delete tokens;
			throw "Invalid command start!";
		}

		int pos = 1;
		SelectCMD *cmd = new SelectCMD();

		if ((*tokens)[pos].type == TokenType::KEYWORD && isKeyword((*tokens)[pos], "ALL")) {
			cmd->setColumnsAll();
			pos++;
		} else {
			while (pos < tokens->getTokenCount()) {
				if ((*tokens)[pos].type == TokenType::KEYWORD && isKeyword((*tokens)[pos], "FROM")) {
					break;
				}
				if ((*tokens)[pos].type != TokenType::STRING && (*tokens)[pos].type != TokenType::KEYWORD) {
					delete cmd;
					delete tokens;
					throw "Invalid column in SELECT";
				}
				cmd->addRequestedColumn((*tokens)[pos].content);
				pos++;
				if (pos < tokens->getTokenCount() && (*tokens)[pos].type == TokenType::SYMBOL && (*tokens)[pos].content == ",") {
					pos++;
					continue;
				}
				if (pos < tokens->getTokenCount() && (*tokens)[pos].type == TokenType::KEYWORD && isKeyword((*tokens)[pos], "FROM")) {
					break;
				}
				if (pos < tokens->getTokenCount()) {
					delete cmd;
					delete tokens;
					throw "Expected ',' or FROM";
				}
			}
			if (cmd->getRequestedCount() == 0) {
				delete cmd;
				delete tokens;
				throw "No columns specified";
			}
		}

		if (pos >= tokens->getTokenCount() || !isKeyword((*tokens)[pos], "FROM")) {
			delete cmd;
			delete tokens;
			throw "Missing FROM";
		}
		pos++;

		if (pos >= tokens->getTokenCount() || (*tokens)[pos].type != TokenType::STRING) {
			delete cmd;
			delete tokens;
			throw "Missing table name";
		}
		cmd->setTableName((*tokens)[pos].content);
		pos++;

		if (pos < tokens->getTokenCount() && isKeyword((*tokens)[pos], "WHERE")) {
			pos++;
			if (pos >= tokens->getTokenCount() || ((*tokens)[pos].type != TokenType::STRING && (*tokens)[pos].type != TokenType::KEYWORD)) {
				delete cmd;
				delete tokens;
				throw "Missing column name in WHERE";
			}
			string whereColumn = (*tokens)[pos].content;
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
			string whereValue = (*tokens)[pos].content;
			cmd->setWhere(whereColumn, whereValue);
		}

		delete tokens;
		return cmd;
	}
};
