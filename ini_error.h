#ifndef _INI_ERROR_H
#define _INI_ERROR_H

#include <stdio.h>
#include <stdlib.h>

extern int ini_errno;

#define INI_OK 0
#define INI_NO -1
#define INI_TABLE_NO_MEM -2
#define INI_TABLE_NAME -3
#define INI_NO_MEM -4
#define INI_NODE_NO_MEM -5
#define INI_NODE_KEY -6
#define INI_NODE_VALUE -7
#define INI_NODE_NOT_FOUND -8
#define INI_SBUF_NO_MEM -9
#define INI_CREATE_FILE -10
#define INI_OPEN_FILE -11
#define INI_STRING_NO_MEM -12
#define INI_READ_FILE -13
#define INI_PARSE_NODE_ERROR -14
#define INI_PARSE_KEY_ERROR -15
#define INI_TABLE_NOT_FOUND -16

void ini_perror(const char *msg);

char *ini_error(int errno);

#endif
