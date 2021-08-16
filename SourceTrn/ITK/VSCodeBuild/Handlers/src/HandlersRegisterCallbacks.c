#include "HandlerRegisterCallbacks.h"

extern DLLAPI int Handlers_gs_shell_init_module(int *decision, va_list args)
{
    int iStatus = ITK_ok;

    printf("Custom Handlers Library\n\n");

    iStatus = EPM_register_action_handler("MyHandler", "", MyHandler);

    if (iStatus == ITK_ok)
        printf("Can not register Handler\n\n");

    return ITK_ok;
}

extern DLLAPI int Handlers_register_callbacks()
{
    CUSTOM_register_exit("Handlers", "USER_gs_shell_init_module", (CUSTOM_EXIT_ftn_t)Handlers_gs_shell_init_module);

    return ITK_ok;
}
