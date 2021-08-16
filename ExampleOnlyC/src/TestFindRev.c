#include <tc/tc.h>
#include <tc/emh.h>
#include <tccore/item.h>

#define ECHO(...) { printf(__VA_ARGS__); TC_write_syslog(__VA_ARGS__); }
#define ECHOLN(...) { ECHO(__VA_ARGS__); ECHO("\n"); }
#define IFERR_ABORT(X) (report_error( __FILE__, __LINE__, #X, X, TRUE))
#define IFERR_REPORT(X) (report_error( __FILE__, __LINE__, #X, X, FALSE))
#define SAFE_MEM_FREE(X) { if (X!=NULL) { MEM_free(X); X=NULL; }}

static int report_error(char *file, int line, char *call, int status,
	logical exit_on_error)
{
	if (status != ITK_ok)
	{
		int
			n_errors = 0,
			*severities = NULL,
			*statuses = NULL;
		char
			**messages;

		EMH_ask_errors(&n_errors, &severities, &statuses, &messages);
		if (n_errors > 0)
		{
			ECHO("\n%s\n", messages[n_errors - 1]);
			EMH_clear_errors();
		}
		else
		{
			char *error_message_string;
			EMH_get_error_string(NULLTAG, status, &error_message_string);
			ECHO("\n%s\n", error_message_string);
		}

		ECHO("error %d at line %d in %s\n", status, line, file);
		ECHO("%s\n", call);

		if (exit_on_error)
		{
			ECHOLN("Exiting program!");
			exit(status);
		}
	}

	return status;
}

static GTAC_find_rev(char *item_id,char *rev_id,tag_t *rev)
{
	int n=0;
	tag_t *items=NULL;
	const char *names[1] ={"item_id"},*values[1]={item_id};
	
	IFERR_REPORT(ITEM_find_item_revs_by_key_attributes(1,names,values,rev_id,&n,&items));
	
	if(n == 0)
		*rev=items[0];
	
	if(n > 1)
		return 66;//oppure la costant ITEM_Multiple che è l'errore della itk;

	if(items)
		MEM_free(items);
	
	return 0;
}
	
int ITK_user_main(int argc, char *argv[])
{
	tag_t revision = NULLTAG;
	tag_t item = NULLTAG;

	char * id = ITK_ask_cli_argument("-id=");

	char * revId = ITK_ask_cli_argument("-revId=");


	IFERR_ABORT(ITK_initialize_text_services(ITK_BATCH_TEXT_MODE));

	if (id == NULL || revId == NULL)
	{
		ECHOLN("Wrong parameters....");
		return ITK_ok;
	}

	IFERR_ABORT(ITK_auto_login());

	ECHOLN("param id: %s , param revId: %s",id,revId);

	IFERR_ABORT(GTAC_find_rev(id, revId, &revision));

	if (revision == NULLTAG)
	{
		ECHOLN("Revision not found!");
		return ITK_ok;
	}
	else
	{
		char* name = NULL;
		char* descr = NULL;

		IFERR_ABORT( AOM_ask_value_string(revision,"object_name", &name));
		ECHOLN("Revision name: %s, revID: %s", name, revId);
		
		IFERR_ABORT( AOM_ask_value_string(revision,"object_desc", &descr));
		ECHOLN("Revision descr: %s, revID: %s", descr, revId);
		
		SAFE_MEM_FREE(name);
		SAFE_MEM_FREE(descr);
	}	

	return ITK_ok;
}





















