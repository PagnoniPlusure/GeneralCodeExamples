#include "MyHandler.h"

extern DLLAPI int MyHandler(EPM_action_message_t msg)
{
    char *name = NULL;

    EPM_ask_name2(msg.task, &name);

    if (name != NULL)
        printf("name: %s \n", name);

    return ITK_ok;
}
