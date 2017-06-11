//============================================================================
// Name        : FiniteDifferencePricing.cpp
// Author      : pmontalb
// Version     :
// Copyright   : 
// Description :
//============================================================================

#include <iostream>
#include <string>
#include <algorithm>
#include <gtest/gtest.h>

char* getCmdOption(char ** begin, char ** end, const std::string & option)
{
    char ** itr = std::find(begin, end, option);
    if (itr != end && ++itr != end)
    {
        return *itr;
    }
    return 0;
}

bool cmdOptionExists(char** begin, char** end, const std::string& option)
{
    return std::find(begin, end, option) != end;
}

int main(int argc, char * argv[])
{
	if(cmdOptionExists(argv, argv+argc, "-test"))
	{
		::testing::InitGoogleTest(&argc, argv);
		return RUN_ALL_TESTS();
	}

	// main

	return 0;
}
