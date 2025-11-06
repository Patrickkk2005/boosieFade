#pragma once

#include "CreateTable.h"
#include <iostream>
#include <regex>
#include <string.h>
#include <string>
using namespace std;

class MainEngine {
  public:
	void runapp() {
		string input;
		while (true) {
			cout << "sql>> ";
			getline(cin, input);
			if (input == "exit") {
				cout << "goodbye!!!";
				break;
			}
			if (input.empty()) {
				continue;
			}
		}
	}

  private:
	void processCMD(string &command) {
		string upperC;
		for (char &c : upperC) {
			c = toupper(c);
		}
		if (upperC.rfind("CREATE TABLE", 0) == 0) {
			// CreateTableParser parser;
			// CreateTableCMD *cmd = nullptr;
			//  cmd=parser.parse(command);
			// cout << *cmd;
		} else
			cout << "Command not recognized!";
	}
};