#include "Lambdas.h"
#include "CommonFunctions.h"

//declare a lambda expression as constexpr (or use it in a constant expression) 
//when the initialization of each captured or introduced data member is allowed
//within a constant expression.
int tryLambdaConstExpr()
{
	int y = 32;
	auto answer = [y]() constexpr
	{
		int x = 10;
		return y + x;
	};

	auto response = answer();

	return response;
}