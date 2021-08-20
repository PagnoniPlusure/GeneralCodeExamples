#include "Tuples.h"

tuple<int, string, double> f()
{
	int i{ 108 };
	string s{ "Some text" };
	double d{ .01 };
	return { i,s,d };
}

void TryTie()
{
	auto t = f();
	cout << get<0>(t) << " " << get<1>(t) << " " << get<2>(t) << endl;

	// --or--

	int myval;
	string myname;
	double mydecimal;
	tie(myval, myname, mydecimal) = f();
	cout << myval << " " << myname << " " << mydecimal << endl;
}

void TryTuples()
{
	std::vector<std::tuple<int, int, int>>  tuples;

	tuples.push_back(std::make_tuple(21, 20, 19));
	tuples.push_back(std::make_tuple(18, 17, 19));

	//for (auto[X, Y, Z] : tuples)  //works only from C++ 17
	//{
	//	std::cout << X << " " << Y << " " << Z << std::endl;
	//}

	tuples.push_back(std::make_tuple(21, 20, 19));
	tuples.push_back(std::make_tuple(18, 17, 19));

	for (auto&& tuple : tuples)
	{
		int X, Y, Z;
		std::tie(X, Y, Z) = tuple;

		std::cout << X << " " << Y << " " << Z << std::endl;
	}

}