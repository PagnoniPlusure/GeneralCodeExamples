#include <tc/tc.h>

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
ECHO("\n%s\n", messages[n_errors-1]);
EMH_clear_errors();
}
else
{
char *error_message_string;
EMH_get_error_string (NULLTAG, status, &error_message_string);
ECHO("\n%s\n", error_message_string);
}

ECHO("error %d at line %d in %s\n", status, line, file);
ECHO("%s\n", call);

if (exit_on_error)
{
ECHO("Exiting program!\n");
exit (status);
}
}

return status;
}

int ITK_user_main(int argc,char *argv[])
{
int status;
char *message;

IFERR_ABORT(ITK_initialize_text_services(ITK_BATCH_TEXT_MODE));

status = ITK_auto_login();
if (status != ITK_ok)
{
EMH_ask_error_text(status,&message);

ECHO("ERROR with ITK_auto_login\"%d \",\"%s\"\nFile: %s\nLine:%d\n", status, message, __FILE__, __LINE__);
//printf("ERROR with ITK_auto_login\"%d \",\"%s\"\nFile: %s\nLine:%d\n", status, message, __FILE__, __LINE__);
return status;
}
else
{
	ECHO("Login successfull\n");
//printf("Login successfull\n");
}

ITK_exit_module(TRUE);

return ITK_ok;
}