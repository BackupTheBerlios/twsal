#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <vector>
#include <stdlib.h>

using namespace std;
 
#include "twsal.h"

int main(int argc, char *argv[])
{
	twsal * script = new twsal();
	if (argc > 1)
	{
		if (argv[1][0] != '-')
		{
			script->open(argv[1]);
			script->execute();
		}
	}/* else
	{
		string code;
		char th;
		while ((th = getchar()) != '\n')
		{
			code += th;
		}
		script.load(code);
		script.execute();
	}*/
	return 0;
}


