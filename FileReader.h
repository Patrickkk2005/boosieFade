#pragma once

#include <fstream>
#include <iostream>
#include <string.h>
#include <string>
using namespace std;

class CommandFileReader {
  private:
	string *filePaths;
	int filePathCount;
	int filePathCap;
	string *commands;
	int commandCount;
	int commandCap;

	bool isTextFile(const string &filePath) {
		if (filePath.length() < 4) {
			return false;
		}
		return filePath.substr(filePath.length() - 4) == ".txt";
	}

	void increaseFilePathCap() {
		if (this->filePathCount < this->filePathCap) {
			return;
		}
		int newCap = (this->filePathCap == 0) ? 5 : this->filePathCap * 2;
		string *newFilePaths = new string[newCap];
		for (int i = 0; i < this->filePathCount; i++) {
			newFilePaths[i] = this->filePaths[i];
		}
		delete[] this->filePaths;
		this->filePaths = newFilePaths;
		this->filePathCap = newCap;
	}

	void increaseCommandCap() {
		if (this->commandCount < this->commandCap) {
			return;
		}
		int newCap = (this->commandCap == 0) ? 10 : this->commandCap * 2;
		string *newCommands = new string[newCap];
		for (int i = 0; i < this->commandCount; i++) {
			newCommands[i] = this->commands[i];
		}
		delete[] this->commands;
		this->commands = newCommands;
		this->commandCap = newCap;
	}

	void readCommandsFromFile(const string &filePath) {
		ifstream file(filePath);
		if (!file.is_open()) {
			throw "Cannot open file";
		}

		string line;
		while (getline(file, line)) {
			if (!line.empty()) {
				increaseCommandCap();
				this->commands[this->commandCount++] = line;
			}
		}
		file.close();
	}

  public:
	CommandFileReader() {
		this->filePaths = nullptr;
		this->filePathCount = 0;
		this->filePathCap = 0;
		this->commands = nullptr;
		this->commandCount = 0;
		this->commandCap = 0;
	}

	~CommandFileReader() {
		delete[] this->filePaths;
		delete[] this->commands;
	}

	void loadCommandsFromArguments(int argcnt, char **argval) {
		int fileCount = 0;

		for (int i = 1; i < argcnt && fileCount < 5; i++) {
			string filePath = argval[i];

			if (!isTextFile(filePath)) {
				throw "Invalid file type";
			}

			increaseFilePathCap();
			this->filePaths[this->filePathCount++] = filePath;

			readCommandsFromFile(filePath);
			fileCount++;
		}

		if (fileCount > 0) {
			cout << "Loaded " << fileCount << " file(s) with " << this->commandCount << " command(s)." << endl;
		}
	}

	int getCommandCount() const {
		return this->commandCount;
	}

	string getCommand(int index) const {
		if (index < 0 || index >= this->commandCount) {
			throw "Invalid command index!";
		}
		return this->commands[index];
	}

	bool hasCommands() const {
		return this->commandCount > 0;
	}

}; // end of FileREader class
