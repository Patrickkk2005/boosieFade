#include "EngineHeaders.h"
#include <iostream>
#include <regex>
#include <string.h>
#include <string>
using namespace std;

int main() {
	Column col1("boja", "STRING", 10, "bussy");
	cout << col1.getName() << endl
		 << col1.getSize() << endl
		 << col1.getDefVal();
}