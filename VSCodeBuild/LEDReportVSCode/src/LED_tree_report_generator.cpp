#include <vector>
#include <map>
#include <string>
#include <sstream>
#include <base_utils/ResultCheck.hxx>
#include <base_utils/IFail.hxx>
#include <base_utils/TcResultStatus.hxx>

#include <iostream>
#include <time.h>
#include <tc/emh.h>
#include <tc/tc.h>
#include <tc/tc_startup.h>
#include <schmgt/schmgt_bridge_itk.h>
#include <tccore/aom_prop.h>
#include <pom/pom/pom.h>
#include <tccore/aom.h>
#include <pom/pom/pom_tokens.h>
#include <base_utils/ScopedSmPtr.hxx>
#include <tccore/tctype.h>
#include <tccore/grm.h>
#include <tc/tc_macros.h>
#include <lov/lov.h>
#include <fclasses/tc_string.h>
#include <base_utils/ScopedSmPtr.hxx>
#include <mld/logging/TcMainLogger.hxx>
#include <mld/logging/Logger.hxx>
#include <error_datatypes.h>
#include "base_utils/ScopedSmPtr.hxx"
#include "base_utils/ScopedPtr.hxx"
#include <tccore/project.h>
#include <tccore/project_errors.h>
#include <algorithm>
#include <epm/epm.h>
#include <epm/epm_task_template_itk.h>
#include <epm/signoff.h>
#include <Fnd0Participant/participant.h>
#include <set>
#include <ce/ce.h>
#include <fclasses/AcquireLock.hxx>
#include <tccore/Item.hxx>
#include <tccore/ItemRevision.hxx>
#include <metaframework/BusinessObjectRef.hxx>
#include <regex>
#include <cfm/cfm.h>
#include <bom/bom.h>
#include <ae/ae.h>
#include <tc/folder.h>

#include <windows.h>
#include <atlbase.h>
#include <atlstr.h>
#include <atlsafe.h>

#include"PlaceHolderSubs.h"

//#import "C:\Program Files (x86)\Common Files\Microsoft Shared\OFFICE16\mso.dll" rename("RGB", "MSRGB"), rename("DocumentProperties", "MSDocumentProperties")
//#import "C:\Program Files (x86)\Common Files\Microsoft Shared\VBA\VBA6\VBE6EXT.OLB" raw_interfaces_only, rename("Reference", "ignorethis"), rename("VBE", "testVBE")
//#import "C:\Program Files (x86)\Microsoft Office\OFFICE16\excel.exe" exclude("IFont", "IPicture") rename("RGB", "ignorethis"), rename("DialogBox", "ignorethis"), rename("VBE", "testVBE"), rename("ReplaceText", "EReplaceText"), rename("CopyFile","ECopyFile"), rename("FindText", "EFindText"), rename("NoPrompt", "ENoPrompt"), rename("Excel","EExcel")

#define FREE_AND_VOID(__name__) if(__name__ != NULL) { MEM_free(__name__); __name__ = NULL; }
#define FREE_AND_VOID_RECURSIVE(__name__,__count__) if(__name__ != NULL) { for(int freeAndVoidRecId = 0; freeAndVoidRecId < __count__; freeAndVoidRecId++) {MEM_free(__name__[freeAndVoidRecId]);} MEM_free(__name__); __name__ = NULL; __count__ = 0; }
#define RM_START() int iFail = ITK_ok; ResultStatus rStatus; Teamcenter::Logging::Logger* pLogger = Teamcenter::Logging::Logger::getLogger("Teamcenter.Leonardo.HANDLER"); pLogger->printf(pLogger->LOG_DEBUG, "\n\t\t\t %s Entering...\n", __FUNCTION__); try {
#define RM_END() } catch(IFail & exFail) { pLogger->printf(ERROR_line, exFail.ifail(), exFail.getMessage().c_str()); pLogger->printf(pLogger->LOG_ERROR, "\n\t\t\t ERROR: %d %s \n", exFail.ifail(), exFail.getMessage().c_str()); iFail = exFail.ifail(); }
#define RM_RETURN() pLogger->printf(pLogger->LOG_DEBUG, "\n\t\t\t %s Exiting...\n", __FUNCTION__); return iFail;
#define RM_ERROR(__code__) { EMH_store_error(EMH_severity_error, __code__); rStatus = __code__; }

// Se le seguenti variabili sono definite, allora:
// - Il template Excel non viene scaricato da TC ma viene letto il
//   file TREE_REPORT_GENERATOR_TEST_INPUT
// - Il file di output non viene caricato su TC ma viene salvato
//   come TREE_REPORT_GENERATOR_TEST_OUTPUT + "." + estensione (pdf, xlsx o csv)
// - Il workflow non viene fatto avanzare
// Questa modalit� pu� essere utile per testare la generazione dei report senza
// dover ogni volta modificare/caricare/scaricare dataset da TC n� dover rilanciare
// il workflow.
#define TREE_REPORT_GENERATOR_TEST
//#define TREE_REPORT_GENERATOR_TEST_INPUT "C:\\Users\\infodba\\Desktop\\Brencich\\template.xlsx"
#define TREE_REPORT_GENERATOR_TEST_OUTPUT "C:\\Users\\infodba\\Desktop\\Report Leonardo 09 - 2021\\TreeReport_Branchic\\ReportOutput"
#define TREE_REPORT_GENERATOR_TEST_INPUT "C:\\Users\\infodba\\Desktop\\Report Leonardo 09 - 2021\\TreeReport_Branchic\\ProductTreeIT.xlsx"


int writeError(tag_t task, const std::string& message)
{
	int num = 0;
	tag_t* list = NULL;
	char* name = NULL;
	RM_START();
	FREE_AND_VOID(list);
	std::string shortMessage = message;
	if (shortMessage.length() > 240)
	{
		shortMessage = shortMessage.substr(0, 237) + "...";
	}
	rStatus = AOM_ask_value_tags(task, "successors", &num, &list);
	for (int i = 0; i < num; i++)
	{
		FREE_AND_VOID(name);
		tag_t errTask = list[i];
		rStatus = AOM_ask_value_string(errTask, "object_name", &name);
		if (tc_strstr(name, "Error") != NULL)
		{
			rStatus = AOM_lock(errTask);
			rStatus = AOM_set_value_string(errTask, "object_desc", shortMessage.c_str());
			rStatus = AOM_save(errTask);
			rStatus = AOM_unlock(errTask);
		}
	}
	RM_END();
	FREE_AND_VOID(list);
	FREE_AND_VOID(name);
	RM_RETURN();
}

int goWorkflow(tag_t task, const char* status)
{
#ifdef TREE_REPORT_GENERATOR_TEST
	return 0;
#endif
	bool success = false;
	RM_START();
	rStatus = AOM_lock(task);
	rStatus = EPM_set_task_result(task, status);
	rStatus = AOM_save(task);
	rStatus = AOM_unlock(task);
	success = true;
	rStatus = EPM_trigger_action(task, EPM_complete_action, "");
	RM_END();
	if (!success)
	{
		AOM_unlock(task);
	}
	RM_RETURN();
}

typedef struct
{
	bool productTree = false;
	bool productTreeUser = false;
	bool productTreeCM = false;
	bool projectTree = false;
	bool projectTreeUser = false;
	bool projectTreeCM = false;
	bool CM = false;
	bool User = false;
	tag_t mainItem = NULLTAG;
	tag_t configForm = NULLTAG;
	tag_t rootTask = NULLTAG;
	std::string revRule = "Working; Any Status";
	// Configuration
	std::string applicableDocs;           // CM
	bool showComponents = true;           // All
	std::string customer;                 // CM
	date_t date = POM_null_date();        // CM
	bool showDocuments = true;            // All
	bool historicization = false;         // All
	std::string identifier = "Standard";  // All
	int levels = 0;                       // All
	std::string outputFormat = "Excel";   // User
	std::string name;                     // CM
	std::string referenceDocs;            // CM
	std::string title;                    // CM
	std::string pageLayout = "A4 H";      // User
	std::string language = "English";     // CM
	std::vector<std::string> columns;     // All
	// Read-only fields
	std::string project;                  // CM
} TreeReportInfo;

int readConfig(TreeReportInfo& info)
{
	char* tempString = NULL;
	int count = 0;
	char** strings = NULL;
	RM_START();
	tag_t form = info.configForm;
	if (info.CM)
	{
		FREE_AND_VOID(tempString);
		rStatus = AOM_ask_value_string(form, "led4ApplicableDocs", &tempString);
		info.applicableDocs = tempString;
		FREE_AND_VOID(tempString);
		rStatus = AOM_ask_value_string(form, "led4Customer", &tempString);
		info.customer = tempString;
		rStatus = AOM_ask_value_date(form, "led4Date", &(info.date));
		info.outputFormat = "PDF";
		FREE_AND_VOID(tempString);
		rStatus = AOM_ask_value_string(form, "led4Name", &tempString);
		info.name = tempString;
		FREE_AND_VOID(tempString);
		rStatus = AOM_ask_value_string(form, "led4ReferenceDocs", &tempString);
		info.referenceDocs = tempString;
		FREE_AND_VOID(tempString);
		rStatus = AOM_ask_value_string(form, "led4Title", &tempString);
		info.title = tempString;
		FREE_AND_VOID(tempString);
		rStatus = AOM_ask_value_string(form, "led4Project", &tempString);
		info.project = tempString;
		FREE_AND_VOID(tempString);
		rStatus = AOM_ask_value_string(form, "led4Language", &tempString);
		info.language = tempString;
	}
	if (info.User)
	{
		FREE_AND_VOID(tempString);
		rStatus = AOM_ask_value_string(form, "led4OutputFormat", &tempString);
		info.outputFormat = tempString;
		FREE_AND_VOID(tempString);
		rStatus = AOM_ask_value_string(form, "led4PageLayout", &tempString);
		info.pageLayout = tempString;
	}
	rStatus = AOM_ask_value_logical(form, "led4Components", &(info.showComponents));
	rStatus = AOM_ask_value_logical(form, "led4Documents", &(info.showDocuments));
	rStatus = AOM_ask_value_logical(form, "led4Historicization", &(info.historicization));
	FREE_AND_VOID(tempString);
	rStatus = AOM_ask_value_string(form, "led4Identifier", &tempString);
	info.identifier = tempString;
	rStatus = AOM_ask_value_int(form, "led4Level", &(info.levels));
	FREE_AND_VOID(strings);
	rStatus = AOM_ask_value_strings(form, "led4Columns", &count, &strings);
	for (int i = 0; i < count; i++)
	{
		info.columns.push_back(strings[i]);
	}

	RM_END();
	FREE_AND_VOID(tempString);
	FREE_AND_VOID(strings);
	RM_RETURN();
}

int getCodeRevIC(const std::string& identifier, const std::string& type, const std::string& itemId, const std::string& revId, tag_t item, tag_t rev, std::string* pCode, std::string* pRev, std::string* pIC)
{
	char* temp = NULL;
	RM_START();
	if (type == "LED4Document")
	{
		*pCode = itemId;
		*pRev = revId;
	}
	else if (type == "LED4EngPart" || type == "LED4CustPart")
	{
		FREE_AND_VOID(temp);
		rStatus = AOM_ask_value_string(item, "led4BaseCode", &temp);
		std::string baseCode = temp;
		FREE_AND_VOID(temp);
		rStatus = AOM_ask_value_string(rev, "led4ConfStatus", &temp);
		std::string confStatus = temp;
		std::string major = confStatus.substr(0, 2);
		std::string minor = confStatus.substr(2, 2);
		if (identifier == "Normalized")
		{
			*pCode = baseCode + major;
			*pRev = minor;
		}
		else if (identifier == "Extended")
		{
			*pCode = baseCode + major;
			*pRev = minor;
			*pIC = major;
		}
		else //if (identifier == "Standard")
		{
			*pCode = baseCode;
			*pRev = confStatus;
		}
	}
	else
	{
		*pCode = itemId;
	}
	RM_END();
	FREE_AND_VOID(temp);
	RM_RETURN();
}

typedef struct
{
	int level = 0;
	std::map<std::string, std::string> data;
} TreeReportLine;

typedef struct
{
	std::vector<TreeReportLine> lines;
} TreeReportData;

int readLineData(tag_t line, tag_t rev, tag_t item, const std::string& type, const TreeReportInfo& info, TreeReportLine& data, int level)
{
	int count = 0;
	tag_t* tags = NULL;
	char* temp = NULL;
	RM_START();
	std::map<std::string, std::string>& map = data.data;
	FREE_AND_VOID(temp);
	rStatus = AOM_ask_value_string(item, "item_id", &temp);
	map["ItemID"] = temp;
	FREE_AND_VOID(temp);
	rStatus = AOM_ask_value_string(rev, "item_revision_id", &temp);
	map["RevID"] = temp;

	// Name
	FREE_AND_VOID(temp);
	rStatus = AOM_ask_value_string(rev, "object_name", &temp);
	map["Name"] = temp;

	// Quantity
	map["Quantity"] = "";
	if (line != NULLTAG)
	{
		FREE_AND_VOID(temp);
		rStatus = AOM_ask_value_string(line, "bl_quantity", &temp);
		map["Quantity"] = temp;
	}

	// Owner
	FREE_AND_VOID(temp);
	rStatus = AOM_UIF_ask_value(rev, "owning_user", &temp);
	map["Owner"] = temp;

	// WF Status, Released Date, Status Date
	FREE_AND_VOID(tags);
	rStatus = AOM_ask_value_tags(rev, "process_stage_list", &count, &tags);
	bool underWorkflow = (count > 0);
	FREE_AND_VOID(tags);
	rStatus = WSOM_ask_release_status_list(rev, &count, &tags);
	std::string lastStatus;
	date_t dateReleased = POM_null_date();
	if (count > 0)
	{
		FREE_AND_VOID(temp);
		rStatus = AOM_ask_value_string(tags[count - 1], "name", &temp);
		lastStatus = temp;
		if (lastStatus.substr(0, 4) == "LED4")
		{
			lastStatus = lastStatus.substr(4);
		}
		rStatus = AOM_ask_value_date(tags[count - 1], "date_released", &dateReleased);
	}
	std::string wfStatus;
	if (lastStatus.length() > 0)
	{
		wfStatus = lastStatus;
	}
	else if(underWorkflow)
	{
		wfStatus = "Started";
	}
	else
	{
		wfStatus = "Not Started";
	}
	map["WF Status"] = wfStatus.c_str();
	map["Released Date"] = "";
	map["Status Date"] = "";
	if (lastStatus.length() > 0)
	{
		FREE_AND_VOID(temp);
		rStatus = ITK_date_to_string(dateReleased, &temp);
		if (lastStatus == "Released")
		{
			map["Released Date"] = temp;
		}
		else
		{
			map["Status Date"] = temp;
		}
	}

	// Expected Date
	map["Expected Date"] = "";
	if (type == "LED4EngPart")
	{
		FREE_AND_VOID(temp);
		rStatus = AOM_ask_value_string(rev, "led4PrevEndDate", &temp);
		map["Expected Date"] = temp;
	}

	// Code, Revision, IC
	std::string scode, srev, sic;
	rStatus = getCodeRevIC(info.identifier, type, map["ItemID"], map["RevID"], item, rev, &scode, &srev, &sic);
	map["Code"] = scode;
	map["Revision"] = srev;
	map["IC"] = sic;
	for (int lev = 0; lev < level; lev++)
	{
		map["Code"] = "  " + map["Code"];
	}

	// Cage Code
	map["Cage Code"] = "";
	if (type == "LED4EngPart")
	{
		FREE_AND_VOID(temp);
		rStatus = AOM_ask_value_string(rev, "led4CageCode", &temp);
		map["Cage Code"] = temp;
	}

	// Alternative Code, Alternative Revision, Alternative IC, Alternative Cage Code
	map["Alternative Code"] = "";
	map["Alternative Revision"] = "";
	map["Alternative IC"] = "";
	map["Alternative Cage Code"] = "";
	tag_t altRel = NULLTAG;
	rStatus = GRM_find_relation_type("LED4Alternatives", &altRel);
	FREE_AND_VOID(tags);
	rStatus = GRM_list_secondary_objects_only(rev, altRel, &count, &tags);
	for (int i = 0; i < count; i++)
	{
		tag_t thisRev = tags[i];
		tag_t thisItem = NULLTAG;
		rStatus = ITEM_ask_item_of_rev(thisRev, &thisItem);
		FREE_AND_VOID(temp);
		rStatus = AOM_ask_value_string(thisItem, "item_id", &temp);
		std::string thisItemId = temp;
		FREE_AND_VOID(temp);
		rStatus = AOM_ask_value_string(thisRev, "item_revision_id", &temp);
		std::string thisRevId = temp;
		FREE_AND_VOID(temp);
		rStatus = AOM_ask_value_string(thisItem, "object_type", &temp);
		std::string thisType = temp;
		std::string scode, srev, sic;
		rStatus = getCodeRevIC(info.identifier, thisType, thisItemId, thisRevId, thisItem, thisRev, &scode, &srev, &sic);
		std::string scagecode;
		if (thisType == "LED4EngPart")
		{
			FREE_AND_VOID(temp);
			rStatus = AOM_ask_value_string(thisRev, "led4CageCode", &temp);
			scagecode = temp;
		}
		std::stringstream sscode, ssrev, ssic, sscc;
		sscode << "Alternative Code";
		ssrev << "Alternative Revision";
		ssic << "Alternative IC";
		sscc << "Alternative Cage Code";
		if (i > 0)
		{
			sscode << " " << (i + 1);
			ssrev << " " << (i + 1);
			ssic << " " << (i + 1);
			sscc << " " << (i + 1);
		}
		map[sscode.str()] = scode;
		map[ssrev.str()] = srev;
		map[ssic.str()] = sic;
		map[sscc.str()] = scagecode;
	}

	// OEM Code, OEM Revision, OEM IC, OEM Cage Code
	map["OEM Code"] = "";
	map["OEM Revision"] = "";
	map["OEM IC"] = "";
	map["OEM Cage Code"] = "";
	if (type == "LED4EngPart" || type == "LED4CustPart")
	{
		FREE_AND_VOID(temp);
		rStatus = AOM_ask_value_string(item, "led4OEPartNumber", &temp);
		map["OEM Code"] = temp;
	}

	// TODO: bisogna ancora leggere e valorizzare i seguenti attributi
	map["NSN Code"] = "";
	map["Serialized"] = "";
	map["Intervention"] = "";
	map["Generator Change"] = "";

	RM_END();
	FREE_AND_VOID(temp);
	FREE_AND_VOID(tags);
	RM_RETURN();
}

int recursiveReadData(tag_t line, tag_t revInInput, int level, const TreeReportInfo& info, TreeReportData& data)
{
	int count = 0;
	tag_t* children = NULL;
	char* objectType = NULL;
	RM_START();
	tag_t rev = NULLTAG;
	if (revInInput != NULLTAG)
	{
		rev = revInInput;
	}
	else
	{
		rStatus = AOM_ask_value_tag(line, "bl_revision", &rev);
	}
	tag_t item = NULLTAG;
	rStatus = ITEM_ask_item_of_rev(rev, &item);
	FREE_AND_VOID(objectType);
	rStatus = WSOM_ask_object_type2(item, &objectType);
	std::string objType = objectType;
	bool consider = true;
	if (objType == "LED4CorpPart" && !info.showComponents)
	{
		consider = false;
	}
	if (consider)
	{
		TreeReportLine lineData;
		lineData.level = level;
		lineData.data["Type"] = objType;
		if (lineData.data["Type"].substr(0, 4) == "LED4")
		{
			lineData.data["Type"] = lineData.data["Type"].substr(4);
		}
		std::stringstream ss;
		ss << level;
		lineData.data["Level"] = ss.str();
		rStatus = readLineData(line, rev, item, objType, info, lineData, level);
		data.lines.push_back(lineData);
		if (info.levels == 0 || info.levels > level)
		{
			if (line != NULLTAG)
			{
				FREE_AND_VOID(children);
				rStatus = BOM_line_ask_child_lines(line, &count, &children);
				for (int i = 0; i < count; i++)
				{
					rStatus = recursiveReadData(children[i], NULLTAG, level + 1, info, data);
				}
			}
			if (info.showDocuments)
			{
				tag_t rel = NULLTAG;
				rStatus = GRM_find_relation_type("LED4DocumentedBy", &rel);
				FREE_AND_VOID(children);
				rStatus = GRM_list_secondary_objects_only(rev, rel, &count, &children);
				for (int i = 0; i < count; i++)
				{
					rStatus = recursiveReadData(NULLTAG, children[i], level + 1, info, data);
				}
			}
		}
	}
	RM_END();
	FREE_AND_VOID(children);
	FREE_AND_VOID(objectType);
	RM_RETURN();
}

int readData(const TreeReportInfo& info, TreeReportData& data)
{
	tag_t window = NULLTAG;
	tag_t* bvrList = NULL;
	int bvrCount = 0;
	RM_START();
	tag_t topItem = info.mainItem;
	FREE_AND_VOID(bvrList);
	rStatus = ITEM_rev_list_bom_view_revs(topItem, &bvrCount, &bvrList);
	if (bvrCount == 0)
	{
		return 0;// TODO remove just for debug
		//RM_ERROR(-1); // TODO: staccare un codice di errore apposito
	}
	tag_t revRule = NULLTAG;
	rStatus = CFM_find(info.revRule.c_str(), &revRule);
	rStatus = BOM_create_window(&window);
	rStatus = BOM_set_window_config_rule(window, revRule);
	tag_t topLine = NULLTAG;
	rStatus = BOM_set_window_top_line(window, NULLTAG, topItem, bvrList[0], &topLine);
	rStatus = recursiveReadData(topLine, NULLTAG, 0, info, data);
	RM_END();
	FREE_AND_VOID(bvrList);
	if(window != NULLTAG) BOM_close_window(window);
	RM_RETURN();
}

int writeTableInExcel(EExcel::_ApplicationPtr& app, EExcel::_WorkbookPtr& book, EExcel::_WorksheetPtr& sheet, const std::vector<std::vector<std::string>>& table)
{
	RM_START();

	sheet->Cells->NumberFormat = "@";
	int columns = (int)table[0].size();
	int lines = (int)table.size();
	SAFEARRAYBOUND bounds[2];
	bounds[1].cElements = (ULONG)columns;
	bounds[1].lLbound = 1;
	bounds[0].cElements = (ULONG)lines;
	bounds[0].lLbound = 1;
	CComSafeArray<BSTR> safeArray(bounds, 2);
	for (int i = 0; i < lines; i++)
	{
		for (int j = 0; j < columns; j++)
		{
			_bstr_t str(table[i][j].c_str());
			LONG indexes[2];
			indexes[0] = i + 1;
			indexes[1] = j + 1;
			safeArray.MultiDimSetAt(indexes, str);
		}
	}
	VARIANT vt;
	VariantInit(&vt);
	vt.vt = VT_BSTR | VT_ARRAY;
	vt.parray = safeArray.Detach();
	_variant_t variant(vt);
	std::string lastChar;
	int lastCharMajor = (columns - 1) / 26;
	int lastCharMinor = (columns - 1) % 26;
	if (lastCharMajor == 0)
	{
		lastChar += (char)('A' + lastCharMinor);
	}
	else
	{
		lastChar += (char)('A' + lastCharMajor - 1);
		lastChar += (char)('A' + lastCharMinor);
	}
	sheet->Activate();
	std::stringstream ss;
	ss << "A1:" << lastChar << lines;
	std::string rangeStr = ss.str();
	EExcel::RangePtr range = sheet->Range[rangeStr.c_str()];
	range->Value2 = variant;
	range->Borders->LineStyle = EExcel::xlContinuous;
	range->Borders->Weight = EExcel::xlThin;
	range->Borders->ColorIndex = 16;
	sheet->Cells->NumberFormat = "@";
	sheet->Cells->Columns->AutoFit();
	sheet->Cells->WrapText = true;

	for (int i = 1; i <= columns; i++)
	{
		double val = ((EExcel::RangePtr)(range->Columns->Item[i]))->EntireColumn->ColumnWidth.dblVal;
		if (val > 20.0)
		{
			((EExcel::RangePtr)(range->Columns->Item[i]))->EntireColumn->ColumnWidth = 20.0;
		}
	}

	std::stringstream ssTopLine;
	ssTopLine << "A1:" << lastChar << "1";
	rangeStr = ssTopLine.str();
	range = sheet->Range[rangeStr.c_str()];
	int red = 200;
	int green = 200;
	int blue = 200;
	int val = (blue * 0x10000) + (green * 0x100) + red;
	range->Interior->Color = val;
	range->Interior->Pattern = EExcel::xlPatternSolid;
	range->Font->Bold = true;

	((EExcel::RangePtr)(sheet->Cells->Item[1, 1]))->Select();
	app->ActiveWindow->ScrollRow = 1;
	app->ActiveWindow->ScrollColumn = 1;

	RM_END();
	RM_RETURN();
}

int RM_create_item(const std::string& itemType, const std::string& itemRevType,
	const std::map<std::string, std::string>& itemParams, const std::map<std::string, std::string>& revParams,
	tag_t* outItem, tag_t* outRev)
{
	tag_t itemTypeTag = NULLTAG;
	tag_t itemRevTypeTag = NULLTAG;
	tag_t creItemInputTag = NULLTAG;
	tag_t creItemRevInputTag = NULLTAG;
	const char* cstr = NULL;
	tag_t itemTag = NULLTAG;
	tag_t uomTag = NULLTAG;
	tag_t revTag = NULLTAG;
	RM_START();
	rStatus = TCTYPE_find_type(itemType.c_str(), NULL, &itemTypeTag);
	rStatus = TCTYPE_find_type(itemRevType.c_str(), NULL, &itemRevTypeTag);
	rStatus = TCTYPE_construct_create_input(itemTypeTag, &creItemInputTag);
	rStatus = TCTYPE_construct_create_input(itemRevTypeTag, &creItemRevInputTag);
	rStatus = AOM_set_value_tag(creItemInputTag, "revision", creItemRevInputTag);
	for (std::map<std::string, std::string>::const_iterator it = itemParams.begin(); it != itemParams.end(); it++)
	{
		cstr = it->second.c_str();
		rStatus = AOM_set_value_string(creItemInputTag, const_cast<char*>(it->first.c_str()), cstr);
	}
	for (std::map<std::string, std::string>::const_iterator it = revParams.begin(); it != revParams.end(); it++)
	{
		cstr = it->second.c_str();
		rStatus = AOM_set_value_string(creItemRevInputTag, const_cast<char*>(it->first.c_str()), cstr);
	}
	rStatus = TCTYPE_create_object(creItemInputTag, &itemTag);
	rStatus = AOM_save_with_extensions(itemTag);
	rStatus = AOM_unlock(itemTag);
	rStatus = ITEM_ask_latest_rev(itemTag, &revTag);
	rStatus = AOM_save_with_extensions(revTag);
	rStatus = AOM_unlock(revTag);
	*outItem = itemTag;
	*outRev = revTag;
	RM_END();
	RM_RETURN();
}



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

string GetFilePath(
	string path,
	string fileName)
{
	return path + "\\" + fileName;
}

int generateTreeReport(EExcel::_ApplicationPtr& app, const TreeReportInfo& info, const TreeReportData& data)
{
	int count = 0;
	tag_t* list = NULL;
	char* temp = NULL;
	std::string fileName;
	std::string outputFileName;
	RM_START();
#ifdef TREE_REPORT_GENERATOR_TEST
	fileName = TREE_REPORT_GENERATOR_TEST_INPUT;
	outputFileName = TREE_REPORT_GENERATOR_TEST_OUTPUT;
#else
	std::string tempDir = getenv("TEMP");
	if (tempDir.length() == 0)
	{
		RM_ERROR(-1); // TODO: staccare un codice di errore apposito
	}
	long seed = (long)time(NULL);
	std::stringstream ssf;
	ssf << tempDir << "\\" << "treereporttemplate" << seed << ".xlsx";
	fileName = ssf.str();
	std::stringstream sso;
	sso << tempDir << "\\" << "treereport" << seed;
	outputFileName = sso.str();
	tag_t templateItem = NULLTAG;
	rStatus = ITEM_find_item("TreeReportGenerator", &templateItem);
	if (templateItem == NULLTAG)
	{
		RM_ERROR(-1); // TODO: staccare un codice di errore apposito
	}
	tag_t templateRev = NULLTAG;
	rStatus = ITEM_ask_latest_rev(templateItem, &templateRev);
	FREE_AND_VOID(list);
	rStatus = AOM_ask_value_tags(templateRev, "TC_Attaches", &count, &list);
	tag_t ds = NULLTAG;
	for (int i = 0; i < count; i++)
	{
		FREE_AND_VOID(temp);
		rStatus = AOM_ask_value_string(list[i], "object_name", &temp);
		if ((tc_strcmp(temp, "Product_EN") == 0 && info.productTree && info.language == "English") ||
			(tc_strcmp(temp, "Project_EN") == 0 && info.projectTree && info.language == "English") ||
			(tc_strcmp(temp, "Product_IT") == 0 && info.productTree && info.language == "Italian") ||
			(tc_strcmp(temp, "Project_IT") == 0 && info.projectTree && info.language == "Italian"))
		{
			ds = list[i];
			break;
		}
	}
	if (ds == NULLTAG)
	{
		RM_ERROR(-1); // TODO: staccare un codice di errore apposito
	}
	rStatus = AE_export_named_ref(list[0], "excel", fileName.c_str());
#endif
	app.CreateInstance(__uuidof(EExcel::Application));
	app->PutDisplayAlerts(LOCALE_USER_DEFAULT, VARIANT_FALSE);

	//string filePath="file://" + fileName;

	bool useEngTemplate = false;

	string templateName = useEngTemplate ? "ProductTreeEN.xlsx" : "ProductTreeIT.xlsx";

	string templateFilePath = GetFilePath(
		"D:\\devel\\CodeGeneralExamples\\ConsoleApps",
		templateName);

	EExcel::_WorkbookPtr book = app->Workbooks->Open(templateFilePath.c_str());

	//pXL->PutVisible(0, FALSE);

	for (int k = 1; k <= book->Sheets->Count; k++)
	{
		EExcel::_WorksheetPtr sheet = (EExcel::_WorksheetPtr)book->Sheets->Item[k];
		std::string name = (char*)sheet->Name;
		if (name == "Header")
		{
			// if (!info.CM) TODO Just for debug
			// {
			// 	sheet->Delete();
			// 	k--;
			// 	continue;
			// }

			SubstitutePHs(sheet->Cells);

			//EExcel::RangePtr ptrRange = sheet->Cells;
			//SEARCH_PH_START("<TITLE>");
			//ptrCurrentFound->Value2 = info.title.c_str();
			//SEARCH_PH_END();
		}
		else if (name == "Report")
		{
			// Gestione delle colonne "Alternative <qualcosa>" che potrebbero dover essere duplicate
			// in base a quante alternative sono presenti al massimo
			int howManyAlternatives = 1;
			for (std::vector<TreeReportLine>::const_iterator it = data.lines.begin(); it != data.lines.end(); it++)
			{
				for (int i = howManyAlternatives+1; i <= 100; i++)
				{
					std::stringstream ss;
					ss << "Alternative Code " << i;
					if (it->data.find(ss.str()) != it->data.end())
					{
						howManyAlternatives = i;
					}
					else
					{
						break;
					}
				}
			}
			std::vector<std::string> columns;
			if (howManyAlternatives > 1)
			{
				std::vector<std::string> altCols;
				for (std::vector<std::string>::const_iterator it = info.columns.begin(); it != info.columns.end(); it++)
				{
					if (it->substr(0, 11) == "Alternative")
					{
						altCols.push_back(*it);
					}
					else if (altCols.size() > 0)
					{
						for (int alt = 2; alt <= howManyAlternatives; alt++)
						{
							for (std::vector<std::string>::const_iterator it2 = altCols.begin(); it2 != altCols.end(); it2++)
							{
								std::stringstream ss;
								ss << *it2 << " " << alt;
								columns.push_back(ss.str());
							}
						}
						altCols.clear();
					}
					columns.push_back(*it);
				}
				if (altCols.size() > 0)
				{
					for (int alt = 2; alt <= howManyAlternatives; alt++)
					{
						for (std::vector<std::string>::const_iterator it2 = altCols.begin(); it2 != altCols.end(); it2++)
						{
							std::stringstream ss;
							ss << *it2 << " " << alt;
							columns.push_back(ss.str());
						}
					}
				}
			}
			else
			{
				columns = info.columns;
			}

			// Impostazione layout di pagina
			if (info.User)
			{
				if (info.pageLayout == "A3 H")
				{
					sheet->PageSetup->Orientation = EExcel::xlLandscape;
					sheet->PageSetup->PaperSize = EExcel::xlPaperA3;
				}
				else if (info.pageLayout == "A3 V")
				{
					sheet->PageSetup->Orientation = EExcel::xlPortrait;
					sheet->PageSetup->PaperSize = EExcel::xlPaperA3;
				}
				else if (info.pageLayout == "A4 V")
				{
					sheet->PageSetup->Orientation = EExcel::xlPortrait;
					sheet->PageSetup->PaperSize = EExcel::xlPaperA4;
				}
				else if (info.pageLayout == "A4 H")
				{
					sheet->PageSetup->Orientation = EExcel::xlLandscape;
					sheet->PageSetup->PaperSize = EExcel::xlPaperA4;
				}
			}

			// Generazione tabella
			std::vector<std::vector<std::string>> table;
			std::vector<std::string> line;
			for (std::vector<std::string>::const_iterator it = columns.begin(); it != columns.end(); it++)
			{
				line.push_back(*it);
			}
			table.push_back(line);
			for (std::vector<TreeReportLine>::const_iterator it = data.lines.begin(); it != data.lines.end(); it++)
			{
				std::vector<std::string> line;
				for (std::vector<std::string>::const_iterator it2 = columns.begin(); it2 != columns.end(); it2++)
				{
					if (it->data.find(*it2) != it->data.end())
					{
						line.push_back(it->data.find(*it2)->second);
					}
					else
					{
						line.push_back("");
					}
				}
				table.push_back(line);
			}
			rStatus = writeTableInExcel(app, book, sheet, table);
		}
	}
	std::string datasetType;
	std::string namedRef;
	std::string datasetName = "Report";

outputFileName= GetFilePath(
		"D:\\devel\\CodeGeneralExamples\\ConsoleApps",
		"ReportLEDBrenchich.xls");		

	if (info.outputFormat == "Excel")
	{
		datasetType = "MSExcelX";
		namedRef = "excel";
		outputFileName += ".xlsx";
		book->SaveAs((outputFileName).c_str(), EExcel::xlOpenXMLWorkbook, "", "", false, false, EExcel::xlExclusive);
	}
	else if (info.outputFormat == "PDF")
	{
		datasetType = "PDF";
		namedRef = "PDF_Reference";
		outputFileName += ".pdf";
		book->ExportAsFixedFormat(EExcel::xlTypePDF, outputFileName.c_str());
	}
	else if (info.outputFormat == "CSV")
	{
		datasetType = "Text";
		namedRef = "Text";
		outputFileName += ".csv";
		book->SaveAs((outputFileName).c_str(), EExcel::xlCSV, "", "", false, false, EExcel::xlExclusive);
	}
#ifndef TREE_REPORT_GENERATOR_TEST
	tag_t dsType = NULLTAG;
	rStatus = AE_find_datasettype2(datasetType.c_str(), &dsType);
	tag_t dataset = NULLTAG;
	rStatus = AE_create_dataset_with_id(dsType, datasetName.c_str(), "", "", "", &dataset);
	rStatus = AE_import_named_ref(dataset, namedRef.c_str(), outputFileName.c_str(), "", SS_BINARY);
	rStatus = AOM_save_with_extensions(dataset);
	rStatus = AOM_unlock(dataset);
	tag_t objectToAttachToWF = NULLTAG;
	if (info.User)
	{
		objectToAttachToWF = dataset;
	}
	else if (info.CM)
	{
		tag_t docItem = NULLTAG;
		tag_t docRev = NULLTAG;
		std::map<std::string, std::string> mapItem;
		std::map<std::string, std::string> mapRev;
		std::string name = info.name.length() > 0 ? info.name : "Report";
		std::string itemId = (info.productTree ? "ProductTreeReport" : "ProjectTreeReport");
		mapItem["item_id"] = itemId;
		mapItem["led4Entity"] = "CO";
		mapItem["led4Standard"] = "Yes";
		mapItem["object_name"] = name;
		mapRev["item_revision_id"] = "A";
		mapRev["led4ShortDesc"] = (info.productTree ? "ProductTreeReport" : "ProjectTreeReport");
		mapRev["object_name"] = name;
		mapRev["led4TypePrj"] = "RPT";
		mapRev["led4SubTypePrj"] = "Technical Report, Generic";
		rStatus = RM_create_item("LED4Document", "LED4DocumentRevision", mapItem, mapRev, &docItem, &docRev);
		objectToAttachToWF = docItem;
		tag_t attaches = NULLTAG;
		rStatus = GRM_find_relation_type("TC_Attaches", &attaches);
		tag_t rel = NULLTAG;
		rStatus = GRM_create_relation(docRev, dataset, attaches, NULLTAG, &rel);
		rStatus = GRM_save_relation(rel);
		rStatus = AOM_unlock(rel);
	}
	int refAtt = EPM_reference_attachment;
	rStatus = AOM_lock(info.rootTask);
	rStatus = EPM_add_attachments(info.rootTask, 1, &objectToAttachToWF, &refAtt);
	rStatus = AOM_save(info.rootTask);
	rStatus = AOM_unlock(info.rootTask);
	if (info.historicization)
	{
		if (info.User)
		{
			tag_t user = NULLTAG;
			rStatus = AOM_ask_owner(info.rootTask, &user);
			tag_t folder = NULLTAG;
			rStatus = AOM_ask_value_tag(user, "newstuff_folder", &folder);
			rStatus = AOM_lock(folder);
			rStatus = FL_insert(folder, objectToAttachToWF, 999);
			rStatus = AOM_save(folder);
			rStatus = AOM_unlock(folder);
		}
		else if (info.CM)
		{
			tag_t docRev = NULLTAG;
			rStatus = ITEM_ask_latest_rev(objectToAttachToWF, &docRev);
			tag_t projItem = NULLTAG;
			rStatus = ITEM_find_item(info.project.c_str(), &projItem);
			tag_t projRev = NULLTAG;
			rStatus = ITEM_ask_latest_rev(projItem, &projRev);
			tag_t outputRel = NULLTAG;
			rStatus = GRM_find_relation_type("LED4ProjOutput", &outputRel);
			tag_t rel = NULLTAG;
			rStatus = GRM_create_relation(projRev, docRev, outputRel, NULLTAG, &rel);
			rStatus = GRM_save_relation(rel);
			rStatus = AOM_unlock(rel);
		}
	}
#endif
	RM_END();
	FREE_AND_VOID(list);
	FREE_AND_VOID(temp);
#ifndef TREE_REPORT_GENERATOR_TEST
	if (app)
	{
		try
		{
			app->Quit();
			app.Release();
		}
		catch (...)
		{
		}
	}
	if (fileName.length() > 0)
	{
		system(("del \"" + fileName + "\"").c_str());
	}
	if (outputFileName.length() > 0)
	{
		system(("del \"" + outputFileName + "\"").c_str());
	}
#endif
	RM_RETURN();
}

extern "C" int ITK_user_main(int argc, char **argv)
{
	ResultStatus rStatus;
	int retCode = 0;
	EExcel::_ApplicationPtr app;
	tag_t task = NULLTAG;
	tag_t rootTask = NULLTAG;
	int targetAttachCount = 0;
	tag_t* targetAttachList = NULL;
	int refAttachCount = 0;
	tag_t* refAttachList = NULL;
	char* objectType = NULL;
	try
	{
		CoInitializeEx(NULL, COINIT_SPEED_OVER_MEMORY | COINIT_MULTITHREADED);
		rStatus = ITK_auto_login();
		char* taskStr = ITK_ask_cli_argument("-task=");
		ITK__convert_uid_to_tag(taskStr, &task);
		rStatus = EPM_ask_root_task(task, &rootTask);

		FREE_AND_VOID(targetAttachList);
		rStatus = EPM_ask_attachments(rootTask, EPM_target_attachment, &targetAttachCount, &targetAttachList);
		FREE_AND_VOID(refAttachList);
		rStatus = EPM_ask_attachments(rootTask, EPM_reference_attachment, &refAttachCount, &refAttachList);

		if (targetAttachCount != 1 || refAttachCount != 1)
		{
			RM_ERROR(-1); // TODO: staccare un codice di errore apposito
		}

		TreeReportInfo info;
		info.mainItem = targetAttachList[0];
		info.configForm = refAttachList[0];
		info.rootTask = rootTask;
		FREE_AND_VOID(objectType);
		rStatus = WSOM_ask_object_type2(info.configForm, &objectType);
		std::string objType(objectType);
		info.productTreeUser = (objType == "LED4ReportConfProdUs");
		info.productTreeCM = (objType == "LED4ReportConfProdCM");
		info.productTree = (info.productTreeUser || info.productTreeCM);
		info.projectTreeUser = (objType == "LED4ReportConfProjUs");
		info.projectTreeCM = (objType == "LED4ReportConfProjCM");
		info.projectTree = (info.projectTreeUser || info.projectTreeCM);
		info.CM = (info.productTreeCM || info.projectTreeCM);
		info.User = (info.productTreeUser || info.projectTreeUser);
		rStatus = readConfig(info);
		TreeReportData data;
		rStatus = readData(info, data);
		rStatus = generateTreeReport(app, info, data);
		rStatus = goWorkflow(task, "Success");
		//std::cout << "Success" << std::endl;
	}
	catch (IFail& fail)
	{
		std::string errorMessage = fail.getMessage();
		try
		{
			rStatus = writeError(task, errorMessage);
		}
		catch (...)
		{
			// Nothing to do
		}
		try
		{
			rStatus = goWorkflow(task, "Error");
		}
		catch (...)
		{
			retCode = -1;
		}
		//std::cout << "Error" << std::endl;
	}
	catch (...)
	{
		std::string errorMessage = "Unknown error";
		try
		{
			rStatus = writeError(task, errorMessage);
		}
		catch (...)
		{
			// Nothing to do
		}
		try
		{
			rStatus = goWorkflow(task, "Error");
		}
		catch (...)
		{
			retCode = -1;
		}
		//std::cout << "Error" << std::endl;
	}
	FREE_AND_VOID(targetAttachList);
	FREE_AND_VOID(refAttachList);
	FREE_AND_VOID(objectType);
	ITK_exit_module(true);
	CoUninitialize();
	return retCode;
}
