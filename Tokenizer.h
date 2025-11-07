#pragma once

#include <iostream>
#include <regex>
#include <string.h>
#include <string>
using namespace std;

class StringFuncs { // BECAUSE WE CANNOT USE CSTRING :(
  public:
	int stringTOint(const string &s) {
		int result = 0;
		bool neg = false;
		int i = 0;
		if (s.length() > 0 && s[0] == '-') {
			neg = true;
			i++;
		}
		for (; i < s.length(); i++) {
			char c = s[i];
			if (c < '0' || c > '9') {
				break;
			}
			result = result * 10 + (c - '0');
		}
		if (neg == true) {
			return -result;
		} else
			return result;
	}

	static bool isSpace(char c) {
		return (c == ' ');
	}

	static bool isAlpha(char c) {
		return ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'));
	}

	static bool isDigit(char c) {
		return (c >= '0' && c <= '9');
	}

	static bool isAlfNum(char c) {
		return (isAlpha(c) || isDigit(c));
	}

	static char toUpper(char c) {
		if (c >= 'a' && c <= 'z') {
			return c - 32;
		}
		return c;
	}
}; // end of string functions class

enum TokenType { KEYWORD,
				 IDENTIFIER,
				 NUMBER,
				 STRING,
				 SYMBOL,
				 UNKNOWN
};

struct Token {
	TokenType type;
	string content;
	int line;
};

class TokenList {
  private:
	Token *tokens;
	int tokenCnt;
	int cap;

  public:
	TokenList() {
		this->tokenCnt = 0;
		this->cap = 10;
		this->tokens = new Token[cap];
	}

	~TokenList() {
		delete[] this->tokens;
	}

	void addToken(Token tok) {
		if (tokenCnt >= cap) {
			cap = cap * 2;
			Token *newTokens = new Token[cap];
			for (int i = 0; i < tokenCnt; i++) {
				newTokens[i] = this->tokens[i];
			}

			delete[] this->tokens;

			this->tokens = newTokens;
		}
		tokens[tokenCnt++] = tok;
	}

	// GETTERS

	int getTokenCount() {
		return this->tokenCnt;
	}

	Token &operator[](int i) {
		if (i < 0 || i >= tokenCnt) {
			throw "INvalid index!";
		}
		return tokens[i];
	}
}; // end of token list class

class Tokenizer {
  private:
	string input;
	int pos;
	TokenList *tokens;

  public:
	const char *keywords[21] = {"CREATE", "TABLE", "IF", "NOT", "EXISTS", "INTEGER", "FLOAT", "TEXT", "DEFAULT", "SELECT", "FROM", "WHERE", "INSERT", "INTO", "VALUES", "UPDATE", "SET", "DELETE", "DROP", "INDEX", "ON"};
	const int keywcnt = 21; // will add along the way
	Tokenizer(const string &s) {
		this->input = s;
		this->pos = 0;
		this->tokens = new TokenList();
	}

	TokenList *makeTokens() {
		while (pos < input.length()) {
			while (pos < input.length() && StringFuncs::isSpace(input[pos])) {
				pos++;
			}
			if (pos >= input.length()) {
				break;
			}

			char current = input[pos];

			if (StringFuncs::isAlpha(current)) {
				string word;
				while (pos < input.length() && (StringFuncs::isAlfNum(input[pos]) || input[pos] == '_')) {
					word += input[pos];
					pos++;
				}
				Token token;
				string upperWord = word;
				for (char &c : upperWord)
					c = StringFuncs::toUpper(c);
				bool isKeyword = false;
				for (int i = 0; i < keywcnt; i++) {
					if (upperWord == keywords[i]) {
						isKeyword = true;
						break;
					}
				}
				if (isKeyword)
					token.type = TokenType::KEYWORD;
				else
					token.type = TokenType::IDENTIFIER;
				token.content = word;
				token.line = 1;
				tokens->addToken(token);
			} else if (StringFuncs::isDigit(current)) {
				string number;
				while (pos < input.length() && StringFuncs::isDigit(input[pos])) {
					number += input[pos];
					pos++;
				}
				Token token;
				token.type = TokenType::NUMBER;
				token.content = number;
				token.line = 1;
				tokens->addToken(token);
			} else if (current == '(' || current == ')' || current == '.' || current == '*' || current == '=' || current == ';' || current == ',') {
				Token token;
				token.type = TokenType::SYMBOL;
				token.content = string(1, current);
				tokens->addToken(token);
				pos++;
			} else {
				cout << "invalid character " << current << endl;
				pos++;
			}
		}
		return tokens;
	}

}; // end of tokenizer class
