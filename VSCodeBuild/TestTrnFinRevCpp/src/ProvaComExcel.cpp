
#include <string>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <comdef.h>
#include <windows.h>

//#include <CppITKLib.h>
#include <metaframework/BusinessObjectRef.hxx>

#import "C:\Program Files (x86)\Common Files\Microsoft Shared\OFFICE16\mso.dll" rename("RGB", "MSRGB"), rename("DocumentProperties", "MSDocumentProperties")
#import "C:\Program Files (x86)\Common Files\Microsoft Shared\VBA\VBA6\VBE6EXT.OLB" raw_interfaces_only, rename("Reference", "ignorethis"), rename("VBE", "testVBE")
#import "C:\Program Files (x86)\Microsoft Office\OFFICE16\excel.exe" exclude("IFont", "IPicture") rename("RGB", "ignorethis"), rename("DialogBox", "ignorethis"), rename("VBE", "testVBE"), rename("ReplaceText", "EReplaceText"), rename("CopyFile","ECopyFile"), rename("FindText", "EFindText"), rename("NoPrompt", "ENoPrompt"), rename("Excel","EExcel")



using namespace Office;
using namespace VBIDE;
using namespace Teamcenter;


int main()
{
	// tag_t rootTask = NULLTAG;
	// tag_t task = givenMsg.task;
	// tag_t *refs,*targets = NULL;
	// int refCount, tarCount = 0;

	// //ECHOLN("Starting Report Generation...");

	// EPM_ask_root_task(task,&rootTask);

	// EPM_ask_attachments(rootTask, EPM_target_attachment,&tarCount ,&targets);
	// EPM_ask_attachments(rootTask, EPM_reference_attachment,&refCount, &refs);

	// for (int i = 0; i < tarCount; i++)
	// {

	// }

	EExcel::_ApplicationPtr ptrApp; 

	return 0;
}