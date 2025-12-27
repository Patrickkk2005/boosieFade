#pragma once

#include "CreateTable.h"
#include "Tokenizer.h"
#include <iostream>
#include <string.h>
#include <string>

using namespace std;

class DisplayTableCMD {
  public:
	string tableName;
	DisplayTableCMD(const string &tableName) {
		if (tableName.empty()) {
			throw "missing table name!";
		}
		this->tableName = tableName;
	}
	void dsptbl(CreateTableCMD *tables, int tableCount) {
		int idx = findTableIndex(tables, tableCount);
		if (idx == -1) {
			throw "Table does not exist!";
		}
		cout << tables[idx];
	}

	int findTableIndex(CreateTableCMD *tables, int tableCount) {
		for (int i = 0; i < tableCount; i++) {
			if (tables[i].getTableName() == this->tableName) {
				return i;
			}
		}
		return -1;
	}
}; // end of display command class

class DisplayTableParser {
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
	DisplayTableCMD *parse(const string &input) {
		Tokenizer tokenizer(input);
		TokenList *tokens = tokenizer.makeTokens();

		if (tokens->getTokenCount() < 3) {
			throw "too few tokens!";
		}

		if (!ISkeyword((*tokens)[0], "DISPLAY") || !ISkeyword((*tokens)[1], "TABLE")) {
			delete tokens;
			throw "invalid command start!";
		}

		if ((*tokens)[2].type != TokenType::STRING) {
			delete tokens;
			throw "missing table name!";
		}

		string name = (*tokens)[2].content;
		delete tokens;
		return new DisplayTableCMD(name);
	}
};