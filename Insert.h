#pragma once

#include "CreateTable.h"
#include "Tokenizer.h"
#include <iostream>
#include <string>
using namespace std;

class InsertCMD {
  private:
	string tableName;
	string *values;
	int valueCnt;

	void copyValues(const string *src, int cnt) {
		if (this->values != nullptr) {
			delete[] this->values;
			this->values = nullptr;
		}
		this->valueCnt = cnt;
		if (cnt > 0 && src != nullptr) {
			this->values = new string[cnt];
			for (int i = 0; i < cnt; i++) {
				this->values[i] = src[i];
			}
		}
	}

	int findTableIndex(CreateTableCMD *tables, int tableCount) {
		for (int i = 0; i < tableCount; i++) {
			if (tables[i].getTableName() == this->tableName) {
				return i;
			}
		}
		return -1;
	}

  public:
	InsertCMD() {
		this->tableName = "";
		this->values = nullptr;
		this->valueCnt = 0;
	}

	InsertCMD(const string &tableName, string *vals, int cnt) {
		if (tableName.empty()) {
			throw "missing table name!";
		}
		this->tableName = tableName;
		this->valueCnt = cnt;
		this->values = nullptr;
		copyValues(vals, cnt);
	}

	InsertCMD(const InsertCMD &other) {
		this->tableName = other.tableName;
		this->values = nullptr;
		this->valueCnt = 0;
		copyValues(other.values, other.valueCnt);
	}

	InsertCMD &operator=(const InsertCMD &other) {
		if (this == &other) {
			return *this;
		}
		this->tableName = other.tableName;
		copyValues(other.values, other.valueCnt);
		return *this;
	}

	~InsertCMD() {
		delete[] this->values;
	}

	void insertInto(CreateTableCMD *tables, int tableCount) {
		int idx = findTableIndex(tables, tableCount);
		if (idx == -1) {
			throw "Table does not exist!";
		}
		if (tables[idx].getColCnt() != this->valueCnt) {
			throw "Invalid number of values for table!";
		}
		tables[idx].addRow(this->values, this->valueCnt);
	}

	string getTableName() {
		return this->tableName;
	}

	int getValueCount() {
		return this->valueCnt;
	}

	string &operator[](int i) {
		if (i < 0 || i >= this->valueCnt) {
			throw "Invalid value index";
		}
		return this->values[i];
	}

	bool operator==(const InsertCMD &other) {
		if (this->tableName != other.tableName || this->valueCnt != other.valueCnt) {
			return false;
		}
		for (int i = 0; i < this->valueCnt; i++) {
			if (this->values[i] != other.values[i]) {
				return false;
			}
		}
		return true;
	}

	bool operator!() const {
		return this->tableName.empty() || this->valueCnt == 0;
	}

	friend ostream &operator<<(ostream &os, const InsertCMD &cmd) {
		os << "Inserted: " << cmd.tableName << " values (";
		for (int i = 0; i < cmd.valueCnt; i++) {
			os << cmd.values[i];
			if (i < cmd.valueCnt - 1) {
				os << ", ";
			}
		}
		os << ")";
		return os;
	}
};

class InsertParser {
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
	InsertCMD *parse(const string &input) {
		Tokenizer tokenizer(input);
		TokenList *tokens = tokenizer.makeTokens();

		if (tokens->getTokenCount() < 6) {
			delete tokens;
			throw "Invalid INSERT command: too few tokens";
		}

		if (!isKeyword((*tokens)[0], "INSERT") || !isKeyword((*tokens)[1], "INTO")) {
			delete tokens;
			throw "Invalid command start!";
		}

		if ((*tokens)[2].type != TokenType::STRING) {
			delete tokens;
			throw "Missing table name!";
		}
		string tableName = (*tokens)[2].content;

		int pos = 3;
		if ((*tokens)[pos].type != TokenType::KEYWORD || !isKeyword((*tokens)[pos], "VALUES")) {
			delete tokens;
			throw "Expected VALUES!";
		}
		pos++;

		if ((*tokens)[pos].type != TokenType::SYMBOL || (*tokens)[pos].content != "(") {
			delete tokens;
			throw "Missing '('!";
		}
		pos++;

		int x = 0;
		string *tempVals = new string[tokens->getTokenCount()];
		while (pos < tokens->getTokenCount()) {
			if ((*tokens)[pos].type == TokenType::SYMBOL && (*tokens)[pos].content == ")") {
				pos++;
				break;
			}

			if ((*tokens)[pos].type != TokenType::STRING && (*tokens)[pos].type != TokenType::NUMBER) {
				delete[] tempVals;
				delete tokens;
				throw "Invalid value token";
			}
			tempVals[x++] = (*tokens)[pos].content;
			pos++;

			if (!((*tokens)[pos].type == TokenType::SYMBOL && (*tokens)[pos].content == ")" || (*tokens)[pos].content == ",") || pos >= tokens->getTokenCount()) {
				delete[] tempVals;
				delete tokens;
				throw "Expected ',' or ')' after value";
			}

			if ((*tokens)[pos].type == TokenType::SYMBOL && (*tokens)[pos].content == ")") {
				pos++;
				break;
			}

			if ((*tokens)[pos].type == TokenType::SYMBOL && (*tokens)[pos].content == ",") {
				pos++;
			}
		}

		if (x == 0) {
			delete[] tempVals;
			delete tokens;
			throw "No values provided!";
		}

		InsertCMD *cmd = new InsertCMD(tableName, tempVals, x);
		delete[] tempVals;
		delete tokens;
		return cmd;
	}
};
