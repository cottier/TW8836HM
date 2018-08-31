#include <stdio.h>

static BYTE LedControl(p_shell_context_t context,WORD argc,char **argv);

/*
 * fun:SHELL_SendDataCallback(BYTE *buf,WORD len)
 *    :*buf 缓冲区代发送
 *    :len  发送长度
 */
void SHELL_SendDataCallback(BYTE *buf,WORD len)
{
  while(len--)
  {
    PUTCHAR(*(buf++));
  }
}
/*
 * fun:SHELL_ReceiveDataCallback(BYTE *buf,WORD len)
 *    :*buf 缓冲区接受数据
 *    :len  接收数据长度
 */
void SHELL_ReceiveDataCallBack(BYTE* buf,WORD len)
{
  while(len--)
  {
    *(buf++) = READCHAR();
  }
}
static const shell_command_context_t xLedCommand = { "led",\
                                                     "\r\nsldjflkdsf",\
                                                     LedControl,\
                                                     2};
static BYTE LedControl(p_shell_context_t context,WORD argc,char** argv)
{

}
BYTE main(void)
{
  shell_context_struct user_context;
  SHELL_Init(&user_context,SHELL_SendDataCallback,SHELL_ReceiveDataCallback,SHELL_Printf,"SHELL>>");
  SHELL_RegisterCommand(&xLedCommand);
  SHELL_Main(&user_context);
  while(1)
  {
    ;
  }
}
