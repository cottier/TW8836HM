#include "typedefs.h"
#include "hmshell.h"

long SHELL_RegisterCommand(const shell_command_context_t* command_context)
{
    long result = 0;

    return result;
}

void SHELL_Init(p_shell_context_t context,\
								send_data_cb_t sendfun,\
								recv_data_cb_t recvfun,\
								printf_data_cb_t shellprintf,\
								char* prompt)
{
	context->send_data_func = sendfun;
  context->recv_data_func = recvfun;
  context->printf_data_func = shellprintf;
	context->prompt = prompt;
//	SHELL_RegisterCommand(&xHelpCommand);
//  SHELL_RegisterCommand(&xExitCommand);
	context->printf_data_func("shell init is ok");
}
void SHELL_Main(p_shell_context_t context )
{
	static BYTE ch;
	//while(1)
	{
		ch = getchar
		if(context->exit == 0)
		{
			break;
		}
		else 
		{
			context->printf_data_func("%c",ch);
		}
	}
}

static uint8_t GetChar(p_shell_context_t context)
{
    uint8_t ch;
    context->recv_data_func(&ch, 1U);
    return ch;
}
