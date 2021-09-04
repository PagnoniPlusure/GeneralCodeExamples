int setClassificationSys(TreeReportInfo& info)
{
	char* parentId = NULL;
	char* childId = NULL;
	char* parentClassName = NULL;
	char* childClassName = NULL;
	logical isClassified;
	tag_t classObj = NULLTAG;
	tag_t childClass = NULLTAG;
	tag_t parentClass = NULLTAG;

	RM_START();

	rStatus = ICS_is_wsobject_classified(
		info.mainItem,
		&isClassified);

	if (isClassified)
	{

		rStatus = ICS_ask_classification_object(
			info.mainItem,
			&classObj);

		rStatus = ICS_ask_class_of_classification_obj(
			classObj,
			&childClass);

		rStatus = ICS_ask_parent(
			childClass,
			&parentClass);

		rStatus = ICS_ask_id_name(
			childClass,
			&childId,
			&childClassName);

		rStatus = ICS_ask_id_name(
			parentClass,
			&parentId,
			&parentClassName);

		string sysForPh = string(parentClassName);
		string subSysForPh = string(childClassName);
		info.system = sysForPh + " / " + subSysForPh;
	}
	else
	{
		info.system = "N/A";
	}

	RM_END();
	FREE_AND_VOID(childId);
	FREE_AND_VOID(parentId);
	FREE_AND_VOID(parentClassName);
	FREE_AND_VOID(childClassName);
	RM_RETURN();
}