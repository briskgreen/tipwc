#ifndef _INI_H
#define _INI_H

#include "ini_data.h"
#include <ctype.h>
#include <sbuf.h>

//获取一个节点中的一个值
#define ini_get_key_value(ini,name,key) (ini_table_get_key_value(ini->table,name,key))

//从字符串中解析
INI *ini_parse(const char *string);

//从文件中解析
INI *ini_parse_with_filename(const char *filename);

//使用INI结构创建一个ini配置文件
int ini_build(INI *ini,const char **string);

//释放
void ini_build_free(const char *string);

//使用INI结构创建一个配置文件到磁盘
int ini_build_to_file(INI *ini,const char *filename);

//获取一个节点中键的数量
int ini_get_talbe_num(INI *ini,const char *name);

#endif
