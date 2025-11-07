#pragma once

#include "CreateTable.h"
#include <iostream>
#include <string.h>
#include <string>
using namespace std;

class MainEngine {
  public:
	void runapp() {
		string input;
		cout << "SQLite ENGINE V1 (type 'exit' to quit)" << endl;
		while (true) {
			cout << "sql>> ";
			getline(cin, input);

			if (input == "exit") {
				cout << "Goodbye!" << endl;
				break;
			}

			if (input.empty()) {
				cout << "Please enter a command";
			}

			processCMD(input);
		}
	}

  private:
	void processCMD(string &command) {
		try {
			string upperC = command;
			for (char &c : upperC) {
				c = StringFuncs::toUpper(c);
			}
			if (upperC.find("CREATE TABLE") == 0) {
				CreateTableParser parser;
				CreateTableCMD *cmd = nullptr;
				try {
					cmd = parser.parse(command);
					cout << *cmd;
					delete cmd;
				} catch (const char *err) {
					if (cmd)
						delete cmd;
					throw;
				}
			} else {
				cout << "For now only CREATE TABLE is available!" << endl;
			}
		} catch (const char *err) {
			cout << "SQL Error: " << err << endl;
		} catch (const std::exception &e) {
			cout << "Error: " << e.what() << endl;
		} catch (...) {
			cout << "An unexpected error occurred" << endl;
		}
	}
};