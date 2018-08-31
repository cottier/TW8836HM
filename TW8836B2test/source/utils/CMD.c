
typedef void(*HOOKPROCESS)(void);

typedef struct{
	unsigned char num;
	unsigned char len;
	char* cmdstr;
	HOOKPROCESS funptr;
}CMD_T