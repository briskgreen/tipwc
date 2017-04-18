#include "ini_error.h"

int ini_errno;

void ini_perror(const char *msg)
{
	if(msg == NULL)
		printf("%s\n",ini_error(ini_errno));
	else
		printf("%s: %s\n",ini_error(ini_errno),msg);
}

char *ini_error(int errno)
{
	static char *msg;

	switch(errno)
	{
		case INI_OK:
			msg="成功!";
			break;
		case INI_NO:
			msg="失败!";
			break;
		case INI_TABLE_NO_MEM:
			msg="为表开辟内存空间出错!";
			break;
		case INI_TABLE_NAME:
			msg="为表中名称开辟内存空间出错!";
			break;
		case INI_NO_MEM:
			msg="为INI结构开辟内存空间出错!";
			break;
		case INI_NODE_NO_MEM:
			msg="为节点开辟内存空间出错!";
			break;
		case INI_NODE_KEY:
			msg="为节点中键开辟内存空间出错!";
			break;
		case INI_NODE_VALUE:
			msg="为节点中值开辟内存空间出错!";
			break;
		case INI_NODE_NOT_FOUND:
			msg="未发现参数!";
			break;
		case INI_SBUF_NO_MEM:
			msg="开辟内存出错!";
			break;
		case INI_CREATE_FILE:
			msg="创建文件出错!";
			break;
		case INI_OPEN_FILE:
			msg="打开文件出错!";
			break;
		case INI_STRING_NO_MEM:
			msg="为文件开辟缓冲区出错!";
			break;
		case INI_READ_FILE:
			msg="读取文件出错!";
			break;
		case INI_PARSE_NODE_ERROR:
			msg="错误的节点!";
			break;
		case INI_PARSE_KEY_ERROR:
			msg="错误的键!";
			break;
		case INI_TABLE_NOT_FOUND:
			msg="未发现节点!";
			break;
		default:
			msg="未知错误!";
	}

	return msg;
}
