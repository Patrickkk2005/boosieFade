#pragma once

#include "CSVImporter.h"
#include "CreateTable.h"
#include "Tokenizer.h"
#include <iostream>
#include <string>
using namespace std;

class ImportCMD {
  private:
	string tableName;
	string csvFilename;

	int findTableIndex(CreateTableCMD *tables, int tableCount) {
		for (int i = 0; i < tableCount; i++) {
			if (tables[i].getTableName() == this->tableName) {
				return i;
			}
		}
		return -1;
	}

  public:
	ImportCMD() {
		this->tableName = "";
		this->csvFilename = "";
	}

	ImportCMD(const string &tableName, const string &csvFilename) {
		this->tableName = tableName;
		this->csvFilename = csvFilename;
	}

	ImportCMD(const ImportCMD &other) {
		this->tableName = other.tableName;
		this->csvFilename = other.csvFilename;
	}

	ImportCMD &operator=(const ImportCMD &other) {
		if (this == &other) {
			return *this;
		}
		this->tableName = other.tableName;
		this->csvFilename = other.csvFilename;
		return *this;
	}

	~ImportCMD() {
	}

	string getTableName() const {
		return this->tableName;
	}

	string getCSVFilename() const {
		return this->csvFilename;
	}

	void importCSV(CreateTableCMD *tables, int tableCount) {
		int idx = findTableIndex(tables, tableCount);
		if (idx == -1) {
			throw "Table does not exist";
		}

		CSVImporter importer(',');
		importer.importCSVFile(this->csvFilename, tables[idx]);
	}

	friend ostream &operator<<(ostream &os, const ImportCMD &cmd) {
		os << "Imported from file '" << cmd.csvFilename << "' into table '" << cmd.tableName << "'";
		return os;
	}
};

class ImportParser {
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
	ImportCMD *parse(const string &input) {
		Tokenizer tokenizer(input);
		TokenList *tokens = tokenizer.makeTokens();

		if (tokens->getTokenCount() < 3) {
			delete tokens;
			throw "Too few tokens";
		}

		if (!isKeyword((*tokens)[0], "IMPORT")) {
			delete tokens;
			throw "Invalid command start";
		}

		if ((*tokens)[1].type != TokenType::STRING) {
			delete tokens;
			throw "Missing table name";
		}

		string tableName = (*tokens)[1].content;

		if ((*tokens)[2].type != TokenType::STRING) {
			delete tokens;
			throw "Missing CSV filename";
		}

		string csvFilename = (*tokens)[2].content;

		delete tokens;
		return new ImportCMD(tableName, csvFilename);
	}
};
