auto userSessionTag = NULLTAG;
auto cm0GlobalChangeContextTag = NULLTAG;
auto activeChangeTag = NULLTAG;		
		
		
		rStatus = CE_current_user_session_tag(&userSessionTag);

		rStatus = AOM_ask_value_tag(
			userSessionTag,
			"cm0GlobalChangeContext",
			&cm0GlobalChangeContextTag);

		info.changeId = info.changeId+"ciao,";
		if (cm0GlobalChangeContextTag != NULLTAG)
		{
			rStatus = ITEM_ask_item_of_rev(
				cm0GlobalChangeContextTag,
				&activeChangeTag);

			rStatus = ITEM_ask_id2(
				activeChangeTag,
				&activeChangeId);

			info.change = activeChangeTag;
			info.changeId = activeChangeId.getString();

			pLogger->printf(
				pLogger->LOG_DEBUG,
				"\n\t\t\t active change: %s\n",
				activeChangeId.getString());
		}
		else
		{
			pLogger->printf(
				pLogger->LOG_DEBUG,
				"\n\t\t\t Warning: No Active Change in the user session.");
		}