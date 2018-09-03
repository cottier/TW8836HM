#ifndef __HMSHELL_H__
#define __HMSHELL_H__

#define KEY_ESC (0x1BU)
#define KEY_DEL (0x7FU)

#define uint8_t      BYTE //unsigned char 
#define uint16_t     WORD //unsigned int
#define uint32_t     DWORD//unsinged long

#define SHELL_BUFFER_SIZE 64
#define SHELL_MAX_CMD (6U)
#define SHELL_HIST_MAX    3
#define SHELL_HIST_CMD    6

typedef BYTE(*get_char_cb_t)(void);
typedef void(*send_data_cb_t)(char*,uint8_t);
typedef void(*recv_data_cb_t)(char*,uint8_t);
typedef void(*printf_data_cb_t)(const char* , ...);
typedef enum _fun_key_status
{
    KEYSHELL_Nomal = 0,
    KEYSHELL_Special,
    KEYSHELL_Function,
}fun_key_status_t;
typedef struct _shell_context_struct
{
    char*   prompt;
    fun_key_status_t STAT;
//buffer
    char line[SHELL_BUFFER_SIZE];
    uint8_t cmd_num;
    uint8_t l_pos;
    uint8_t c_pos;
//send&receive Function.
    send_data_cb_t send_data_func;
    recv_data_cb_t recv_data_func;
    printf_data_cb_t printf_data_func;
//histroy
    char hist_buf[SHELL_HIST_MAX][SHELL_BUFFER_SIZE];
    uint8_t hist_current;
    uint8_t hist_count;
    uint8_t exit;
}shell_context_struct,*p_shell_context_t;

typedef long( *cmd_function_t)(p_shell_context ,uint32_t ,char** );
typedef struct _shell_command_context
{
		const char* pcCommand;
		char* pcHelpString;
		const cmd_function_t pFuncCallBack;
	  uint8_t cExpectedNumberOfParameters;
}shell_command_context_t;

typedef struct _shell_command_context_list
{
		const shell_command_context_t* CommandList[SHELL_MAX_CMD];
		uint8_t numberOfCommandInList;
}shell_command_context_list_t;

void SHELL_Init(p_shell_context_t ,send_data_cb_t,recv_data_cb_t,printf_data_cb_t,char*);
void SHELL_Main(p_shell_context_t );

#endif