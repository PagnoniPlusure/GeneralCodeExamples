// for_each example
#include <iostream>     // std::cout
#include <algorithm>    // std::for_each
#include <vector>       // std::vector
#include "CommonFunctions.h"
#include "TryExcel.h"
#include <tuple>

using namespace std;

void TryForeach()
{
	vector<int> myvector;
	myvector.push_back(10);
	myvector.push_back(20);
	myvector.push_back(30);

	std::cout << "\nmyvector contains:\n";
	for_each(myvector.begin(), myvector.end(), printIndex);
	std::cout << '\n';

	// or:
	ClassWithPrintIndexMethod myobject;
	cout << "\nmyvector contains:\n";
	for_each(myvector.begin(), myvector.end(), myobject);
	cout << "\n";
}

void TryDistinct()
{
	vector<int> vecTagBlackList = { 1, 2, 3, 3, 3, 10, 1, 2, 3, 7, 7, 8 };

	vector<int>::iterator ip2;

	// Sorting the array
	std::sort(vecTagBlackList.begin(), vecTagBlackList.end());
	// Now v becomes 1 1 2 2 3 3 3 3 7 7 8 10

	// Using std::unique
	ip2 = std::unique(vecTagBlackList.begin(), vecTagBlackList.end());
	// Now v becomes {1 2 3 7 8 10 * * * * * *}
	// * means undefined

	// Resizing the vector so as to remove the undefined terms
	vecTagBlackList.resize(std::distance(vecTagBlackList.begin(), ip2));


	cout << "\n\nProva Distinct: my vector v2 now contains:\n";
	// Displaying the vector after applying std::unique
	for (ip2 = vecTagBlackList.begin(); ip2 != vecTagBlackList.end(); ++ip2) {
		cout << "||" << *ip2 << " ";
	}


	cout << "\n\n\n";
}

void TryUnique()
{
	vector<int> v = { 1, 1, 3, 3, 3, 10, 1, 3, 3, 7, 7, 8 }, i;

	vector<int>::iterator ip;

	// Using std::unique
	ip = std::unique(v.begin(), v.begin() + 12);
	// Now v becomes {1 3 10 1 3 7 8 * * * * *}
	// * means undefined

	// Resizing the vector so as to remove the undefined terms
	v.resize(std::distance(v.begin(), ip));

	cout << "\n\nProva Unique: my vector v now contains:\n";
	// Displaying the vector after applying std::unique
	for (ip = v.begin(); ip != v.end(); ++ip) {
		cout << "||" << *ip << " ";
	}
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

int main()
{
	LED_generate_report();

	//TryTuples();

	//Try_excel_Modify_Cells();

	//TryForeach();

	//TryUnique();

	//TryDistinct();

	return 0;
}
