#include <iostream>
#include <cassert>
#include <cstdio>

#include "UnitTest.h"
#include "CoinReleasePlan.h"

using namespace std;
using namespace crp;

int usage(const char* progName)
{
	printf("usage: %s height\n",progName);
	return 0;
}

int main(int argc, const char* argv[])
{
	DoUnitTest();
	
	if(argc < 2)
		return usage(argv[0]);
	
	Height height = atoi(argv[1]);
	
	BlockValue v = crp::GetBlockValue(height);
	cout << v.ToString() << endl;
	return 0;
}
