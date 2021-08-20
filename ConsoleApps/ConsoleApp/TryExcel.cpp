#include "TryExcel.h"
#include "CommonFunctions.h"

using namespace Office;
using namespace VBIDE;


#define SEARCH_PH_FAILED()				if(ptrFirstFound == NULL) \
										{ \
											ptrFirstFound = ptrCurrentFound; \
										}
#define SEARCH_PH_START(__id__)	{ \
											EExcel::RangePtr ptrFirstFound = NULL; \
											EExcel::RangePtr ptrCurrentFound = ptrRange->Find(__id__, (ptrRange->Item[1][1]), vtMissing, EExcel::xlPart, EExcel::xlByRows, EExcel::xlNext, VARIANT_TRUE); \
											while(ptrCurrentFound != NULL) \
											{ \
												if(ptrFirstFound != NULL && ptrCurrentFound->GetAddress(vtMissing, vtMissing, EExcel::xlA1) == ptrFirstFound->GetAddress(vtMissing, vtMissing, EExcel::xlA1)) \
												{ \
													break; \
												} \
												std::string value = (char*)((_bstr_t)(ptrCurrentFound->Value2)); \
												if(value == __id__) \
												{
#define SEARCH_PH_END()							} \
												else \
												{ \
													SEARCH_PH_FAILED(); \
												} \
												_variant_t test = static_cast<IDispatch*>(ptrCurrentFound); \
												ptrCurrentFound = ptrRange->FindNext(test); \
											} \
										}

string applicableDocs;           // CM
bool showComponents = true;           // All
string customer;                 // CM
bool showDocuments = true;            // All
bool historicization = false;         // All
string identifier = "Standard";  // All
int levels = 0;                       // All
string outputFormat = "Excel";   // User
string name;                     // CM
string referenceDocs;            // CM
string title;                    // CM
string pageLayout = "A4 H";      // User
string language = "English";     // CM
vector<string> columns;     // All

void SubstitutePHs(
	EExcel::RangePtr ptrRange,
	 vector<tuple<string,string>> placeHolders)
{
	for (auto&& tuple : placeHolders)
	{
		string key, value;
		std::tie(key, value) = tuple;

		std::cout << key << " : " << value  << std::endl;

		ptrRange->Replace(key.c_str(), value.c_str());
	}
}

string GetFilePath(
	string path,
	string fileName)
{
	return path + "\\" + fileName;
}

EExcel::_ApplicationPtr OpenExcel(string filePath)
{
	HRESULT hr = CoInitializeEx(0, COINIT_MULTITHREADED);
	if (FAILED(hr))
	{
		std::cout << "Failed to initialize COM library. Error code = 0x"
			<< std::hex << hr << std::endl;
		return NULL;
	}

	// Create Excel Application Object pointer  
	EExcel::_ApplicationPtr pXL;

	if (FAILED(pXL.CreateInstance("Excel.Application")))
	{
		std::cout << "Failed to initialize Excel::_Application!" << std::endl;
		return NULL;
	}

	// Open the Excel Workbook, but don't make it visible  
	pXL->Workbooks->Open(filePath.c_str());
	//pXL->Visible[ 0 ] = false;   // sometimes generates error?
	pXL->PutVisible(0, FALSE);

	// Access Excel Worksheet and return pointer to Worksheet cells  
	return pXL;
}

int Finalyze(
	EExcel::_ApplicationPtr pXL,
	string filePath)
{
	EExcel::_WorksheetPtr pWksheet = pXL->ActiveSheet;

	// Switch off alert prompting to save as 
	pXL->PutDisplayAlerts(LOCALE_USER_DEFAULT, VARIANT_FALSE);

	// Save the values in book.xml and release resources
	pWksheet->SaveAs(filePath.c_str());
	pWksheet->Release();

	// And switch back on again...
	pXL->PutDisplayAlerts(LOCALE_USER_DEFAULT, VARIANT_TRUE);

	pXL->Quit();

	return 0;
}

int LED_generate_report()
{
	bool useEngTemplate = true;

	string templateName = useEngTemplate ? "ProdTemplate_EN.xlsx" : "ProdTemplate_IT.xlsx";

	string templateFilePath = GetFilePath(
		"D:\\devel\\CodeGeneralExamples\\ConsoleApps",
		templateName);

	EExcel::_ApplicationPtr templateExcelPointer = OpenExcel(templateFilePath);

	if (templateExcelPointer == NULL)
		return 20;//TODO

	EExcel::_WorksheetPtr pWksheet = templateExcelPointer->ActiveSheet;
	EExcel::RangePtr ptrRange = pWksheet->Cells;

	vector<tuple<string, string>> placeHolders;

	placeHolders.push_back(std::make_tuple("<title>", "LEONARDO ELECTRONICS REPORT SAMPLE"));
	placeHolders.push_back(std::make_tuple("<docNumber>", "PG10004.08.2237TR"));
	placeHolders.push_back(std::make_tuple("<docRev>", "02.00"));
	placeHolders.push_back(std::make_tuple("<docDate>", "25/06/2021"));
	placeHolders.push_back(std::make_tuple("<cageCode>", "A1615"));
	placeHolders.push_back(std::make_tuple("<customer>", "PD&CM"));
	placeHolders.push_back(std::make_tuple("<projName>", "REPORT TCU"));
	placeHolders.push_back(std::make_tuple("<system>", ""));
	placeHolders.push_back(std::make_tuple("<subSystem>", ""));
	placeHolders.push_back(std::make_tuple("<cdrl>", "N/A"));
	placeHolders.push_back(std::make_tuple("<contractId>", ""));
	placeHolders.push_back(std::make_tuple("<unclassPages>", "10"));
	placeHolders.push_back(std::make_tuple("<unclassConPages>", ""));
	placeHolders.push_back(std::make_tuple("<restrPages>", ""));
	placeHolders.push_back(std::make_tuple("<confidPages>", ""));
	placeHolders.push_back(std::make_tuple("<secretPages>", ""));
	placeHolders.push_back(std::make_tuple("<totPages>", "10"));
	placeHolders.push_back(std::make_tuple("<telAuthor>", "+39 06 41503294"));
	placeHolders.push_back(std::make_tuple("<emailAuthor>", "stefano.quartucci@leonardocompany.com"));
	placeHolders.push_back(std::make_tuple("<author>", "Stefano Quartucci"));

	SubstitutePHs(
		ptrRange,
		placeHolders);

	string templateModifiedFilePath = GetFilePath(
		"D:\\devel\\CodeGeneralExamples\\ConsoleApps",
		"LEDReport.xlsx");

	return Finalyze(
		templateExcelPointer,
		templateModifiedFilePath);
}



int Try_excel_Modify_Cells()
{
	string filePath = GetFilePath(
		"D:\\devel\\CodeGeneralExamples\\ConsoleApps",
		"TryExcel.xls");

	EExcel::_ApplicationPtr pXL = OpenExcel(filePath);

	if (pXL == NULL)
		return 20;//TODO

	EExcel::_WorksheetPtr pWksheet = pXL->ActiveSheet;
	EExcel::RangePtr pRange = pWksheet->Cells;

	// Read an Excel data cell. (Note Excel cells start from index = 1)
	double value1 = pRange->Item[1][1];
	std::cout << "Value in CELL [1][1] = " << value1 << std::endl;
	double value2 = pRange->Item[1][2];
	std::cout << "Value in CELL [1][2] = " << value2 << std::endl;

	// Write/modify Excel data cells + save. (reopen xls file to verify)
	std::cout << std::endl;
	std::cout << "Modifying Excel cell values..." << std::endl;
	std::cout << std::endl;
	pRange->Item[1][1] = 5.4321;
	pRange->Item[1][2] = 1.1211;

	// Output new value
	value1 = pRange->Item[1][1];
	std::cout << "New value in CELL [1][1] = " << value1 << std::endl;
	value2 = pRange->Item[1][2];
	std::cout << "New value in CELL [1][2] = " << value2 << std::endl;

	return Finalyze(
		pXL,
		filePath);
}