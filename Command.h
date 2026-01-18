#pragma once
#include <iostream>
#include <string>
using namespace std;

class Command {
  protected:
	string tableName;

  public:
	Command() : tableName("") {}
	Command(const string &name) : tableName(name) {}
	virtual ~Command() {}

	virtual void execute() = 0;

	virtual string getType() const = 0;

	virtual string getDescription() const {
		return tableName;
	}

	string getTableName() const {
		return tableName;
	}
};

class SelectCommand : public Command {
  public:
	SelectCommand(const string &name) : Command(name) {}
	void execute() override {
		cout << "SELECT executed on " << tableName << endl;
	}
	string getType() const override {
		return "SELECT";
	}
	string getDescription() const override {
		return "SELECT from " + tableName;
	}
};

class InsertCommand : public Command {
  private:
	int rowCount;

  public:
	InsertCommand(const string &name, int rows) : Command(name), rowCount(rows) {}
	void execute() override {
		cout << "INSERT executed on " << tableName << endl;
	}
	string getType() const override {
		return "INSERT";
	}
	string getDescription() const override {
		return "INSERT into " + tableName + " (" + to_string(rowCount) + " values)";
	}
};

class CreateTableCommand : public Command {
  private:
	int colCount;

  public:
	CreateTableCommand(const string &name, int cols) : Command(name), colCount(cols) {}
	void execute() override {
		cout << "CREATE TABLE executed: " << tableName << endl;
	}
	string getType() const override {
		return "CREATE TABLE";
	}
	string getDescription() const override {
		return "CREATE TABLE " + tableName;
	}
};

class DropTableCommand : public Command {
  public:
	DropTableCommand(const string &name) : Command(name) {}
	void execute() override {
		cout << "DROP TABLE executed: " << tableName << endl;
	}
	string getType() const override {
		return "DROP TABLE";
	}
	string getDescription() const override {
		return "DROP TABLE " + tableName;
	}
};

class CommandHistory {
  private:
	Command **commands;
	int cnt;
	int cap;

  public:
	CommandHistory() : commands(nullptr), cnt(0), cap(0) {}

	~CommandHistory() {
		for (int i = 0; i < cnt; i++)
			delete commands[i];
		delete[] commands;
	}

	void add(Command *cmd) {
		if (cnt >= cap) {
			int tempCap;
			if (cap == 0) {
				tempCap = 5;
			} else
				tempCap = cap * 2;
			Command **tempArr = new Command *[tempCap];
			for (int i = 0; i < cnt; i++)
				tempArr[i] = commands[i];
			delete[] commands;
			commands = tempArr;
			cap = tempCap;
		}
		commands[cnt++] = cmd;
	}

	void printHistory() const {
		cout << "=== COMMAND HISTORY FOR CREATE, DROP, INSERT AND SELECT CMDS===" << endl;
		for (int i = 0; i < cnt; i++) {
			cout << (i + 1) << ". " << commands[i]->getType() << " || " << commands[i]->getDescription() << endl;
		}
		cout << "====================================" << endl;
	}
};
