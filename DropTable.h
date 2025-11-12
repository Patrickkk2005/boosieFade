#pragma once

#include "Tokenizer.h"
#include <iostream>
#include <string.h>
#include <string>
using namespace std;

class DropTableCMD {
  public:
	string tableName;
	DropTableCMD(const string &name) {
		if (name.empty()) {
			throw "name cannot be empty!";
		}
		this->tableName = name;
	}
	friend ostream &operator<<(ostream &os, const DropTableCMD &c) {
		os << "DROP TABLE: " << c.tableName << endl;
		return os;
	}
}; // end of drop table command class

class DropTableParser {
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
	DropTableCMD *parse(const string &input) {
		Tokenizer tokenizer(input);
		TokenList *tokens = tokenizer.makeTokens();

		if (tokens->getTokenCount() < 3) {
			delete tokens;
			throw "too few tokens!";
		}

		if (!isKeyword((*tokens)[0], "DROP") || !isKeyword((*tokens)[1], "TABLE")) {
			delete tokens;
			throw "invalid cmd start!";
		}

		int pos = 2;
		if ((*tokens)[pos].type != TokenType::IDENTIFIER) {
			delete tokens;
			throw "missing table name";
		}
		string name = (*tokens)[pos].content;
		delete tokens;
		return new DropTableCMD(name);
	}
};