/*==============================================================================
File description:

	Filename: LED_propagate_issue_date_on_parts.cpp
	Module  : custom_easyplus_handlers

   Action Handler to set issue date property of a Change Revision to relatives
   Documents and Parts

   (UATIssueList - IU218)
================================================================================
   Date         Name
   29-Jun-2021  Alessandro Pagnoni
==============================================================================*/

#include "utility.h"
#include <chrono>

#ifdef __cplusplus
extern "C"
#endif

int LED_propagate_issue_date_on_parts(EPM_action_message_t givenMsg)
{
	#pragma region Constants

	const char * DocRevTypeRealName = "LED4DocumentRevision"; 
	const char * IssueDateChangeRealName = "led4IssueDate";
	const char * IssueDateDocsPartsRealName = "led4PrevEndDate";
	const char * RevisionPartRealName = "LED4EngCRNRevision";
	const char * RelationTypeSolutionName = "CMHasSolutionItem";
	const char * HeaderLogMsg = "Enter into LED_propagate_issue_date_on_parts() handler \n";
	const char * TailLogMsg = "Exit from LED_propagate_issue_date_on_parts() handler \n";
	const char * DurationLogMsg = "/t Time spent to execute handler: %f seconds \n";

	#pragma endregion

	//Flow controls Flags
	int retCode = ITK_ok;
	int	ifail = ITK_ok;
	ResultStatus	rStatus;
	
	//change issue date
	date_t  changeIssueDate;
	char* changeIssueDateString = NULL;

	//primary objects
	tag_t	rootTaskItem = NULLTAG;
	int		rootTaskAttachmentCount = 0;
	tag_t*	rootTaskAttachment = NULLTAG;
	char *curRootTaskAttachmentName = NULL;
	char *curRootTaskAttachmentType = NULL;
	char *secondaryObjectClassName = NULL;
	tag_t rootTask = NULLTAG;

	//secondary objects
	tag_t* secondaryObjects = NULLTAG;
	int secondaryObjectsCount = 0;
	char * secondaryObjectName = NULL;
	char * secondaryObjectType = NULL;
	tag_t secondaryObjectsClassId = NULLTAG;
	tag_t relationtype = NULLTAG;
	Teamcenter::Logging::Logger* pLogger = Teamcenter::Logging::Logger::getLogger("Teamcenter.Leonardo.HANDLER");

	auto timeStart = std::chrono::system_clock::now();

	//Prints enter handler message into logs
	pLogger->printf(
		pLogger->LOG_DEBUG,
		HeaderLogMsg);

	try
	{
		//Gets root task and its attachments
		rStatus = EPM_ask_root_task(
			givenMsg.task,
			&rootTaskItem);

		//Gets root task attachments
		rStatus = EPM_ask_attachments(
			rootTaskItem,
			EPM_target_attachment,
			&rootTaskAttachmentCount,
			&rootTaskAttachment);

		//Prints root task attachments count into logs
		pLogger->printf(
			pLogger->LOG_DEBUG,
			"\t Found %d root Task Attachments. \n",
			rootTaskAttachmentCount);

		for (int j = 0; j < rootTaskAttachmentCount; j++)
		{
			//Gets current root task attachment type
			rStatus = WSOM_ask_object_type2(
				rootTaskAttachment[j],
				&curRootTaskAttachmentType);

			//Checks if LED4EngCRNRevision otherwise skip to next item
			if (tc_strcasecmp(RevisionPartRealName, curRootTaskAttachmentType) != 0)
				return retCode;

			//Gets current root task attachment name
			rStatus = WSOM_ask_name2(
				rootTaskAttachment[j],
				&curRootTaskAttachmentName);

			//Prints current root task attachment name into logs
			pLogger->printf(
				pLogger->LOG_DEBUG,
				"Root Task Attachment Name: %s \n",
				curRootTaskAttachmentName);

			//Gets change issue date 
			rStatus = AOM_ask_value_date(
				rootTaskAttachment[j],
				IssueDateChangeRealName,
				&changeIssueDate);

			//Convert change issue date to string
			rStatus = ITK_date_to_string(
				changeIssueDate,
				&changeIssueDateString);

			//Print change issue date into logs
			pLogger->printf(
				pLogger->LOG_DEBUG,
				"\t change Issue Date String:  %s \n",
				changeIssueDateString);

			//Gets solution relation type item
			rStatus = GRM_find_relation_type(
				RelationTypeSolutionName,
				&relationtype);

			//Gets change's objects with relation of type solution
			rStatus = GRM_list_secondary_objects_only(
				rootTaskAttachment[j],
				relationtype,
				&secondaryObjectsCount,
				&secondaryObjects);

			//Prints change's objects with relation of type solution into logs
			pLogger->printf(
				pLogger->LOG_DEBUG,
				"\t found %d secondary objects \n",
				secondaryObjectsCount);

			for (int i = 0; i < secondaryObjectsCount; i++)
			{
				//Gets secondary object name
				rStatus = WSOM_ask_name2(
					secondaryObjects[i],
					&secondaryObjectName);

				//Prints secondary object name into logs
				pLogger->printf(
					pLogger->LOG_DEBUG,
					"\t secondaryObjectName %s \n",
					secondaryObjectName);

				//Gets secondary object type
				rStatus = WSOM_ask_object_type2(
					secondaryObjects[i],
					&secondaryObjectType);

				//Prints secondary object type into logs
				pLogger->printf(
					pLogger->LOG_DEBUG,
					"\t secondaryObjectType %s \n",
					secondaryObjectType);

				//bypass role check
				POM_AM__set_application_bypass(TRUE);

				//lock item (transation)
				rStatus = AOM_lock(secondaryObjects[i]);

				//set issue date for doc revision items
				if (strcmp(secondaryObjectType, DocRevTypeRealName) == 0)
					rStatus = AOM_set_value_date(
						secondaryObjects[i],
						IssueDateDocsPartsRealName,
						changeIssueDate);

				else //set issue date for other components
				{
					char* issueDateStr = nullptr; 

					//Convert issue change date into correct string format for property
					DATE_date_to_string(changeIssueDate, "%Y%m%d%H%M%S", &issueDateStr);

					rStatus = AOM_set_value_string(
						secondaryObjects[i],
						IssueDateDocsPartsRealName,
						issueDateStr);

					//Prints issueDateStr into logs
					pLogger->printf(
						pLogger->LOG_DEBUG,
						"\t new issue Date %s for %s \n",
						issueDateStr,
						IssueDateDocsPartsRealName);

					SAFE_MEM_FREE(issueDateStr);
				}

				//persist on db
				rStatus = AOM_save_without_extensions(secondaryObjects[i]);

				//unlock item (exit transation)
				rStatus = AOM_unlock(secondaryObjects[i]);
			}
		}
		//free heap
		SAFE_MEM_FREE(curRootTaskAttachmentName);
		SAFE_MEM_FREE(curRootTaskAttachmentType);
		SAFE_MEM_FREE(secondaryObjectClassName);
		SAFE_MEM_FREE(changeIssueDateString);
		SAFE_MEM_FREE(rootTaskAttachment);
		SAFE_MEM_FREE(secondaryObjects);
		SAFE_MEM_FREE(secondaryObjectName);
		SAFE_MEM_FREE(secondaryObjectType);

		auto timeEnd = std::chrono::system_clock::now();
		std::chrono::duration<double> duration = timeEnd - timeStart;

		pLogger->printf(
			pLogger->LOG_DEBUG,
			DurationLogMsg,
			duration.count());

		//Prints exit handler message into logs
		pLogger->printf(
			pLogger->LOG_DEBUG,
			TailLogMsg);

		//exit no error
		return retCode;
	}
	catch (IFail & exFail)
	{
		//free heap
		SAFE_MEM_FREE(curRootTaskAttachmentName);
		SAFE_MEM_FREE(curRootTaskAttachmentType);
		SAFE_MEM_FREE(secondaryObjectClassName);
		SAFE_MEM_FREE(changeIssueDateString);
		SAFE_MEM_FREE(rootTaskAttachment);
		SAFE_MEM_FREE(secondaryObjects);
		SAFE_MEM_FREE(secondaryObjectName);
		SAFE_MEM_FREE(secondaryObjectType);

		//log error
		pLogger->printf(
			ERROR_line,
			exFail.ifail(),
			exFail.getMessage().c_str());

		//Gets error code
		int errorCode = exFail.ifail();

		//Prints error message into logs
		pLogger->printf(
			pLogger->LOG_DEBUG,
			"\t Error: %d %s \n",
			errorCode,
			exFail.getMessage().c_str());

		//Prints exit handler message into logs
		pLogger->printf(
			pLogger->LOG_DEBUG,
			TailLogMsg);

		//exit with error code
		return errorCode;
	}
}