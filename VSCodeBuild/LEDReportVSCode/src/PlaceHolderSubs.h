#include <tuple>
#include <vector>
#include <string>
#include <iostream>

#import "C:\Program Files (x86)\Microsoft Office\root\VFS\ProgramFilesCommonX86\Microsoft Shared\OFFICE16\mso.dll" rename("RGB", "MSRGB"), rename("DocumentProperties", "MSDocumentProperties")
#import "C:\Program Files (x86)\Microsoft Office\root\VFS\ProgramFilesCommonX86\Microsoft Shared\VBA\VBA6\VBE6EXT.OLB" raw_interfaces_only, rename("Reference", "ignorethis"), rename("VBE", "testVBE")
#import "C:\Program Files (x86)\Microsoft Office\root\OFFICE16\excel.exe" exclude("IFont", "IPicture") rename("RGB", "ignorethis"), rename("DialogBox", "ignorethis"), rename("VBE", "testVBE"), rename("ReplaceText", "EReplaceText"), rename("CopyFile","ECopyFile"), rename("FindText", "EFindText"), rename("NoPrompt", "ENoPrompt"), rename("Excel","EExcel")

using namespace std;

void SubstitutePHs(EExcel::RangePtr ptrRange);