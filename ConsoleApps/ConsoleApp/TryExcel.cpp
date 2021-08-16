#include "TryExcel.h"

#include <string>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <comdef.h>
#include <windows.h>
#include <vector>

#import "C:\Program Files (x86)\Microsoft Office\root\VFS\ProgramFilesCommonX86\Microsoft Shared\OFFICE16\mso.dll" rename("RGB", "MSRGB"), rename("DocumentProperties", "MSDocumentProperties")
#import "C:\Program Files (x86)\Microsoft Office\root\VFS\ProgramFilesCommonX86\Microsoft Shared\VBA\VBA6\VBE6EXT.OLB" raw_interfaces_only, rename("Reference", "ignorethis"), rename("VBE", "testVBE")
#import "C:\Program Files (x86)\Microsoft Office\root\OFFICE16\excel.exe" exclude("IFont", "IPicture") rename("RGB", "ignorethis"), rename("DialogBox", "ignorethis"), rename("VBE", "testVBE"), rename("ReplaceText", "EReplaceText"), rename("CopyFile","ECopyFile"), rename("FindText", "EFindText"), rename("NoPrompt", "ENoPrompt"), rename("Excel","EExcel")



using namespace Office;
using namespace VBIDE;
using namespace std;

//const string filePath = "D:\\devel\\CodeGeneralExamples\\ConsoleApps\\TryExcel.xls";

// Configuration
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


int LED_generate_report ()
{
	string filePath = GetFilePath(
		"D:\\devel\\CodeGeneralExamples\\ConsoleApps",
		"TemplateLED.xlsx");

	EExcel::_ApplicationPtr pXL = OpenExcel(filePath);

	if (pXL == NULL)
		return 20;//TODO

	EExcel::_WorksheetPtr pWksheet = pXL->ActiveSheet;
	EExcel::RangePtr pRange = pWksheet->Cells;

	return 0;
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

	return 0;
}