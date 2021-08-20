#include "CommonFunctions.h"
#include "TryExcel.h"
#include "Tuples.h"
#include "Costructs.h"
#include "Containers.h"
#include "Lambdas.h"

int main()
{
#pragma region Costructs

	TryForeach();

	TryUnique();

	TryDistinct();

#pragma endregion	

#pragma region Excel and LED-Report

	LED_generate_report();

	Try_excel_Modify_Cells();

#pragma endregion

#pragma region Containers

	TryMap();

#pragma endregion	

#pragma region Tuples

	TryTuples();

	TryTie();

#pragma endregion

#pragma region Lambda Expressions

	tryLambdaConstExpr();

#pragma endregion


	return 0;
}
