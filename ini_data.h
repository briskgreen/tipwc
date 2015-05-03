#ifndef _INI_DATA_H
#define _INI_DATA_H

#include "ini_error.h"
#include <string.h>

//节点数据
typedef struct ini_node
{
	char *key;
	char *value;

	struct ini_node *next;
}INI_NODE;

//表
typedef struct ini_table
{
	char *name; //表名称
	int count; //表内节点数
	INI_NODE *node; //表内节点

	struct ini_table *next; //下一个表
}INI_TABLE;

typedef struct
{
	int count; //表数量
	INI_TABLE *table; //表
}INI;

/*
 * 创建一个表
 * name 该表的名称
 * 创建成功时返回一个INI_TABLE指针
 * 出错时返回NULL并设置错误代码
 */
INI_TABLE *ini_table_new(const char *name);

/*
 * 创建一个ini配置文件结构
 * 成功时返回一个INI结构指针
 * 出错时返回NULL并设置错误代码
 */
INI *ini_new(void);

//创建新节点
INI_NODE *ini_node_new(const char *key,const char *value);

/*
 * 向table为name的表中添加一个结点
 * 成功时返回INI_OK
 * 失败时返回错误代码
 */
int ini_node_add(INI_TABLE *table,const char *name,INI_NODE *node);

/*
 * 从table为name的表中查询是否有键为key的节点
 * 查询成功时返回INI_OK
 * 没有查询到结果时返回INI_NO
 */
int ini_table_has_key(INI_TABLE *table,const char *name,const char *key);

//查询表
int ini_has_table(INI *ini,const char *name);

//从table为name的表中删除键为key的节点
void ini_node_del_with_key(INI_TABLE *table,const char *name,const char *key);

//删除一个表
void ini_table_del_with_name(INI *ini,const char *name);

//添加一个表
int ini_table_add(INI *ini,const char *name);

//释放节点资源
void ini_node_free(INI_NODE *node);

//释放表资源
void ini_table_free(INI_TABLE *table);

//释放资源
void ini_free(INI *ini);

//获取一个表name中节点key的值
char *ini_table_get_key_value(INI_TABLE *table,const char *name,const char *key);

//修改表名
int ini_table_set_name(INI_TABLE *table,const char *name,
		const char *new_name);

//修改键名
int ini_node_set_key(INI_TABLE *table,const char *name,
		const char *key,const char *new_key);

//修改键值
int ini_node_set_value(INI_TABLE *table,const char *name,
		const char *key,const char *new_value);

//修改整个键值对
int ini_node_set(INI_TABLE *table,const char *name,
		const char *key,const char *new_key,
		const char *new_value);

#endif
