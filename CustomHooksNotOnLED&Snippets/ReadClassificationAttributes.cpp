tag_t IMAN_classification = NULLTAG;

		rStatus = GRM_find_relation_type("IMAN_classification", &IMAN_classification);

		tag_t ico = NULLTAG;
		tag_t* icosList = NULL;
		int icosCount = 0;
		int attrsCount = 0;
		char** attrsNames = NULL;
		char** attrsValues = NULL;

		//CA00004A00
		rStatus = GRM_list_secondary_objects_only(info.mainItem, IMAN_classification, &icosCount, &icosList);

		if (icosCount > 0)
		{
			rStatus = ICS_ask_attributes_of_classification_obj(icosList[0], &attrsCount, &attrsNames, &attrsValues);
			for (int j = 0; j < attrsCount; j++)
			{
				//TODO
				pLogger->printf(pLogger->LOG_INFO, "\n\t\t\t %s = %s", attrsNames[j], attrsValues[j]);
			}
		}
	}