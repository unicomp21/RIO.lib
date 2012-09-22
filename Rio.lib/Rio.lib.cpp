// Rio.lib.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "../include/RIO.lib.h"
#include <iostream>

int _tmain(int argc, _TCHAR* argv[])
{
	TUUID test;
	std::cout << (std::string)test << std::endl;

	TMessage msg;
	msg["arg1"] = "val1";
	msg["arg2"] = "val2";
	msg.Dump(std::cout);

	std::vector<char> buffer;
	msg.Append(buffer);

	TMessage msg2;
	msg2.Read(buffer, 0);
	msg2.Dump(std::cout);

	return 0;
}
