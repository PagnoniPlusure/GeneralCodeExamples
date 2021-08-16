#define ITK_EXE
#include <CppITKLib.hxx>
#include <metaframework/BusinessObjectRef.hxx>
#include <tccore/ItemRevision.hxx>

static tag_t *GTAC_find_rev(
	string item_id,
	string rev_id)
{
	tag_t rev=NULLTAG;
	int n = 0;
	tag_t *items = NULL;
	const char *names[1] = {"item_id"}, *values[1] = {item_id.c_str()};

	SMN_THROW(ITEM_find_item_revs_by_key_attributes(
		1,
		names,
		values,
		rev_id.c_str(),
		&n,
		&items));

	if (n == 0)
		throw "Error:No revision found.";

	if (n == 1)
		rev = items[0];

	if (n > 1)
		throw "Error:Find more than one revision.";

	SAFE_MEM_FREE(items);

	return &rev;
}

tag_t findRevion(
	string id,
	string revId)
{
	tag_t *revision = NULL;

	if (id.empty() || revId.empty())
	{
		ECHOLN("Wrong parameters. Usage -id=...,-revId=...");
		return NULLTAG;
	}

	ECHOLN("param id: %s , param revId: %s", id.c_str(), revId.c_str());

	revision = GTAC_find_rev(id, revId);

	return *revision;
}

int ITK_user_main(int argc, char *argv[])
{
	tag_t revision = NULLTAG;

	try
	{
		SMN_THROW(ITK_initialize_text_services(ITK_BATCH_TEXT_MODE));
		SMN_THROW(ITK_auto_login());
		ECHOLN("Login OK");

		char *id = ITK_ask_cli_argument("-id=");
		char *revId = ITK_ask_cli_argument("-revId=");

		revision = findRevion(id, revId);

		if (revision == NULLTAG)
		{
			ECHOLN("Revision not found!");
		}
		else
		{
			char *name = NULL;
			char *descr = NULL;
			bool res = false;
			string revName = "";

			BusinessObjectRef<Teamcenter::ItemRevision> foundItemRev(revision);

			SMN_THROW(foundItemRev->getObject_name(revName, res));

			//SMN_THROW(AOM_ask_value_string(revision, "object_name", &name));
			ECHOLN("Revision name: %s, revID: %s", revName.c_str(), revId);

			//SMN_THROW(AOM_ask_value_string(revision, "object_desc", &descr));
			//ECHOLN("Revision descr: %s, revID: %s", descr, revId);

			SAFE_MEM_FREE(name);
			SAFE_MEM_FREE(descr);
		}
	}
	SMN_CATCH
	return ITK_ok;
}
