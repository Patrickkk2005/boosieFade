#pragma once
#include "CreateIndex.h"
#include "Tokenizer.h"
#include <iostream>
#include <string.h>
#include <string>
using namespace std;

class DropIndexCMD {
  private:
	string indexName;

	int findIndexIndex(CreateIndexCMD *indexes, int indexCount) {
		for (int i = 0; i < indexCount; i++) {
			if (indexes[i].getIndexName() == this->indexName) {
				return i;
			}
		}
		return -1;
	}

  public:
	DropIndexCMD(const string &name) {
		if (name.empty()) {
			throw "name cannot be empty!";
		}
		this->indexName = name;
	}

	void drpidx(CreateIndexCMD *indexes, int &indexCount) {
		int idx = findIndexIndex(indexes, indexCount);
		if (idx == -1) {
			throw "Index does not exist!";
		}
		for (int i = idx; i < indexCount - 1; i++) {
			indexes[i] = indexes[i + 1];
		}
		indexCount--;
	}
}; // end of drop index cmd class

class DropIndexParser {
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
	DropIndexCMD *parse(const string &input) {
		Tokenizer tokenizer(input);
		TokenList *tokens = nullptr;
		tokens = tokenizer.makeTokens();

		if (tokens->getTokenCount() < 3) {
			delete tokens;
			throw "Invalid DROP INDEX command: to few tokens!";
		}

		if (!isKeyword((*tokens)[0], "DROP") || !isKeyword((*tokens)[1], "INDEX")) {
			delete tokens;
			throw "Invalid command start!";
		}

		int pos = 2;
		if ((*tokens)[pos].type != TokenType::IDENTIFIER) {
			delete tokens;
			throw "missing index name";
		}
		string name = (*tokens)[pos].content;
		delete tokens;
		return new DropIndexCMD(name);
	}
}; // end of drop index parser class