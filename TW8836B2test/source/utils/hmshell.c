#include "typedefs.h"
#include "hmshell.h"

int32_t SHELL_RegisterCommand(const shell_command_context_t *command_context)
{
    int32_t result = 0;

    /* If have room  in command list */
    if (g_RegisteredCommands.numberOfCommandInList < SHELL_MAX_CMD)
    {
        g_RegisteredCommands.CommandList[g_RegisteredCommands.numberOfCommandInList++] = command_context;
    }
    else
    {
        result = -1;
    }
    return result;
}

void SHELL_Init(p_shell_context_t context,\
								send_data_cb_t sendfun,\
								recv_data_cb_t recvfun,\
                printf_data_cb_t shellprinf,\
                char* prompt)
{
	context->send_data_func = sendfun;
  context->recv_data_func = recvfun;
  context->printf_data_func = shellprintf;
	context->prompt = prompt
	SHELL_RegisterCommand(&xHelpCommand);
  SHELL_RegisterCommand(&xExitCommand);
	context->printf_data_func("shell init is ok");
}