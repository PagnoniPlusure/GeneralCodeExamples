#pragma once
#ifndef __CPPITKLIB_HXX__
#define __CPPITKLIB_HXX__

// START - All OOTB TC include
#include <tc/emh.h>
#include <ps/ps.h>
#include <bom/bom_attr.h>
#include <tc/folder.h>
#include <ics/ics.h>
#include <tcinit/tcinit.h>
#include <pom/enq/enq.h>
#include <tc/tc_startup.h>
#include <tccore/workspaceobject.h>
#include <ecm/ecm.h>
#include <tccore/custom.h>
#include <tccore/part.h>
#include <ict/ict_userservice.h>
#include <unidefs.h>
#include <tccore/item.h>
#include <tccore/aom_prop.h>
#include <ae/ae.h>
#include <sa/am.h>
#include <sa/sa.h>
#include <tccore/aom.h>
#include <tccore/tctype.h>
#include <tc/iman_preferences.h>
#include <tc/tc.h>
#include <property/nr.h>
#include <sa/tcfile.h>
#include <ss/ss_const.h>
#include <epm/epm.h>
#include <epm/signoff.h>
#include <bom/bom.h>
#include <ps/ps_tokens.h>
#include <tccore/grmtype.h>
#include <qry/qry.h>
#include <base_utils/Mem.h>
#include <user_exits/user_exits.h>
#include <lov/lov.h>
#include <cfm/cfm.h>
#include <tccore/grm.h>
#include <epm/epm_task_template_itk.h>
#include <textsrv/textserver.h>
#include <fclasses/tc_date.h>
#include <dispatcher/dispatcher_itk.h>
#include <res/res_itk.h>
#include <tccore/project.h>
#include <tccore/uom.h>
#include <form/form.h>
#include <base_utils/ResultCheck.hxx>
#include <base_utils/IFail.hxx>

// END   - All OOTB TC include

// START - C MVS

#include <stddef.h>
#include <time.h>
#include <sys/stat.h>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

using namespace std;

//Define ITK_EXE prior to include this file in order to use printf in executables. DLLs SHALL NOT USE PRINTF as there is no console
#ifdef ITK_EXE
	#define ECHO(...)  { printf(__VA_ARGS__); TC_write_syslog(__VA_ARGS__); }
	#define ECHOLN(...) { printf(__VA_ARGS__); printf("\n"); TC_write_syslog(__VA_ARGS__); TC_write_syslog("\n"); }
#else
	#define ECHO(...)  TC_write_syslog(__VA_ARGS__)
	#define ECHOLN(...) { TC_write_syslog(__VA_ARGS__); TC_write_syslog("\n"); }
#endif

#define ERR_ECHO(ex) \
    ERR_ECHO_GENERIC(ex.debugSrcFile, ex.debugSrcLine, ex.debugSrcFunc, ex.ifail(), ex.getMessage())

#define ERR_ECHO_NO_IFAIL(ERR,MSG) \
    ERR_ECHO_GENERIC( __FILE__, __LINE__, __FUNCTION__, ERR, MSG)

#define ERR_ECHO_GENERIC(F,L,FUNC,ERR,MSG) \
    ECHO("************* An error has occurred ***************\n"); \
    ECHO("\tFile\t\t%s\n\tLine\t\t%d\n\tFunc\t\t%s\n", F, L, FUNC); \
    ECHO("\tError code:\t%d\n\tError msg:\t%s\n", ERR, MSG == NULL ? "" : MSG);\
    ECHO("***************************************************\n\n")

//Use this predefined catch in functions that return an int (iFail)
#define SMN_CATCH catch (RichIFail &ex) { \
    ERR_ECHO(ex); \
    return ex.ifail(); \
} catch (int failCode) { \
    report_error( __FILE__, __LINE__, (char*)__FUNCTION__, failCode, false); \
    return failCode; \
} catch (string &errorMsg) { \
    ERR_ECHO_NO_IFAIL(-32000, errorMsg.c_str()); \
    return -32000; \
} catch (char *msg) { \
    ERR_ECHO_NO_IFAIL(-32000, msg); \
    return -32000; \
} catch (const char *msg) { \
    ERR_ECHO_NO_IFAIL(-32000, msg); \
    return -32000; \
} catch (exception& e) { \
	ERR_ECHO_NO_IFAIL(-32000, e.what()); \
	if (strcmp(e.what(), "Use askUserMessage ( ) or askSyslogMessage ( )")==0) \
		ECHO("******************** FOUND ************************\n"); \
	return -32000; \
} catch (...) { \
    ECHO("Unexpected exception at %s, %d, %s\n", __FILE__, __LINE__, __FUNCTION__); \
    return 1; \
}

#define SMN_THROW(Y) try { \
        ResultCheck rc = Y; \
    } catch (IFail &any) { \
        RichIFail err; \
        err.debugSrcFile=__FILE__; \
        err.debugSrcLine=__LINE__;\
        err.debugSrcFunc=__FUNCTION__;\
        err.iFail=any.ifail(); \
        err.msg = (char *)MEM_alloc(any.getMessage().length()+1);\
        sprintf(err.msg, "%s", any.getMessage().c_str());\
        throw err; \
    }

#define SMN_THROW_MSG(MSG) {\
    RichIFail err; \
    err.debugSrcFile=__FILE__; \
    err.debugSrcLine=__LINE__;\
    err.debugSrcFunc=__FUNCTION__;\
	err.msg=MEM_string_copy(string(MSG).c_str());\
    throw err; \
}

#define SAFE_MEM_FREE(X) { if (X!=NULL) { MEM_free(X); X=NULL; }}

int report_error(char *file, int line, char *call, int status, logical exit_on_error)
{
	if (status != ITK_ok)
	{
		int
			n_errors,
			*severities,
			*statuses;
		char
			**messages;

		EMH_ask_errors(&n_errors, (const int**)&severities, (const int**)&statuses, (const char***)&messages);
		ECHO("\nerror %d at line %d in %s\n", status, line, file);
		if (n_errors > 0) {
			ECHO("\t%s\n\n", messages[n_errors - 1]);
		}
		EMH_clear_errors();


		ECHO("%s\n", call);

		if (exit_on_error)
		{
			ECHO("\nExiting program!\n");
			exit(status);
		}
	}

	return status;
}

class RichIFail
{
public:
	char *debugSrcFile;
	int debugSrcLine;
	char *debugSrcFunc;
	int iFail;
	char *msg;

	RichIFail()
	{
		iFail = -32000;
		msg = NULL;
		debugSrcFile = NULL;
		debugSrcLine = 0;
		debugSrcFunc = NULL;
	}
	;

	int ifail() {
		return iFail;
	}

	const char * getMessage() {
		return msg;
	}
};

class DebugUtils {
public:
	static void pause() {
		if (getenv("VSCTCSCRIPTS")!=NULL) system("pause");
	}
};

#endif // !__CPPITKLIB_HXX__

