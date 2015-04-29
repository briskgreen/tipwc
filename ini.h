#ifndef _INI_H
#define _INI_H

#include "ini_data.h"
#include <sbuf.h>

//从字符串中解析
INI *ini_parse(const char *string);

//从文件中解析
INI *ini_parse_with_filename(const char *filename);

//使用INI结构创建一个ini配置文件
int ini_build(INI *ini,const char **string);

//使用INI结构创建一个配置文件到磁盘
int ini_build_to_file(INI *ini,const char *filename);

#endif
