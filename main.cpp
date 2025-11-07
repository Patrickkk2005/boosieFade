#include "MainEngine.h"
#include <iostream>
#include <regex>
#include <string.h>
#include <string>
using namespace std;

int CreateTableCMD::commandCounter = 0;

int main() {
	/*Column col1("userID", "NUMBER", 10, "0"); // test with param constructor
	cout << "col1: " << col1 << endl;
	Column col2; // normal assig
	col2.setName("username");
	col2.setDatatype("DATE");
	col2.setSize(50);
	col2.setDefVal("guest");
	cout << "col2: " << col2 << endl;
	Column col3 = col1;
	cout << "col3 (copy of col1): " << col3 << endl;
	col2 = col1;
	cout << "col2 (assigned from col1): " << col2 << endl;
	cout << "Is col2 ! (invalid)? " << (!col2) << endl;
	cout << "col3 + 5: " << (col3 + 5) << endl;
	cout << "=================================================";
	// START OF SECOND TEST
	Column c1("ID", "NUMBER", 4, "0");
	Column c2("NAME", "STRING", 50, "Anonymous");
	Column c3("BIRTH_DATE", "DATE", 8, "CURRENT_DATE");
	CreateTableCMD command1("Users", true);
	command1.addColumn(c1);
	command1.addColumn(c2);
	command1.addColumn(c3);

	cout << command1;

	CreateTableCMD command2 = command1;

	cout << command2;*/

	// REAL TEST

	MainEngine engine;
	engine.runapp();
};
