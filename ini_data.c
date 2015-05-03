#include "ini_data.h"

INI_TABLE *ini_table_new(const char *name)
{
	INI_TABLE *table;

	table=malloc(sizeof(INI_TABLE));
	if(table == NULL)
	{
		ini_errno=INI_TABLE_NO_MEM;
		return NULL;
	}

	table->name=strdup(name);
	if(table->name == NULL)
	{
		ini_errno=INI_TABLE_NAME;
		free(table);
		return NULL;
	}
	table->node=NULL;
	table->next=NULL;

	return table;
}

INI *ini_new(void)
{
	INI *ini;
	int i;

	ini=malloc(sizeof(INI));
	if(ini == NULL)
	{
		ini_errno=INI_NO_MEM;
		return NULL;
	}

	ini->count=0;
	ini->table=NULL;

	return ini;
}

INI_NODE *ini_node_new(const char *key,const char *value)
{
	INI_NODE *node;

	node=malloc(sizeof(INI_NODE));
	if(node == NULL)
	{
		ini_errno=INI_NODE_NO_MEM;
		return NULL;
	}

	node->key=strdup(key);
	if(!node->key)
	{
		ini_errno=INI_NODE_KEY;
		return NULL;
	}
	if(value == NULL)
		node->value=NULL;
	else
	{
		node->value=strdup(value);
		if(!node->value)
		{
			ini_errno=INI_NODE_VALUE;
			return NULL;
		}
	}
	node->next=NULL;

	return node;
}

int ini_node_add(INI_TABLE *table,const char *name,INI_NODE *node)
{
	while(table)
	{
		if(strcmp(table->name,name) == 0)
			break;

		table=table->next;
	}

	if(table == NULL)
		return INI_NODE_NOT_FOUND;

	if(table->node == NULL || table->node->key == NULL)
		table->node=node;
	else
	{
		INI_NODE *temp;

		temp=table->node;
		while(temp->next)
			temp=temp->next;

		temp->next=node;
	}

	++table->count;
	return INI_OK;
}

int ini_table_has_key(INI_TABLE *table,const char *name,const char *key)
{
	INI_NODE *node;

	while(table)
	{
		if(strcmp(table->name,name) == 0)
			break;

		table=table->next;
	}

	if(table == NULL) return INI_NO;
	node=table->node;
	if(!node)
		return INI_NO;
	while(node->next)
	{
		if(strcmp(node->key,key) == 0)
			return INI_OK;

		node=node->next;
	}

	return INI_NO;
}

int ini_has_table(INI *ini,const char *name)
{
	INI_TABLE *table=ini->table;

	while(table)
	{
		if(strcmp(table->name,name) == 0)
			return INI_OK;

		table=table->next;
	}

	return INI_NO;
}

void ini_node_del_with_key(INI_TABLE *table,const char *name,const char *key)
{
	INI_NODE *node;
	INI_NODE *temp;

	while(table)
	{
		if(strcmp(table->name,name) == 0)
			break;

		table=table->next;
	}

	if(!table) return;
	node=table->node;
	if(!node) return;
	if(node->next == NULL || strcmp(node->key,key) == 0)
	{
		temp=node;
		table->node=node->next;
		--table->count;
		free(temp->key);
		free(temp->value);
		free(temp);
		return;
	}

	while(node->next)
	{
		if(strcmp(node->next->key,key) == 0)
		{
			temp=node->next;
			node->next=node->next->next;
			--table->count;
			free(temp->key);
			free(temp->value);
			free(temp);
			return;
		}

		node=node->next;
	}
}

void ini_table_del_with_name(INI *ini,const char *name)
{
	INI_TABLE *table;
	INI_TABLE *temp;

	if(!ini) return;
	table=ini->table;
	if(!table) return;
	if(table->next == NULL || strcmp(table->name,name) == 0)
	{
		temp=table;
		ini->table=table->next;
		--ini->count;
		ini_node_free(temp->node);
		free(temp);
		return;
	}

	while(table->next)
	{
		if(strcmp(table->next->name,name) == 0)
		{
			temp=table->next;
			table->next=table->next->next;
			--ini->count;
			ini_node_free(temp->node);
			free(temp);
			return;
		}

		table=table->next;
	}
}

int ini_table_add(INI *ini,const char *name)
{
	INI_TABLE *table;
	INI_TABLE *table_node;

	table_node=ini_table_new(name);
	if(!table_node) return ini_errno;
	if(ini->table == NULL)
		ini->table=table_node;
	else
	{
		table=ini->table;

		while(table->next)
			table=table->next;

		table->next=table_node;
	}

	++ini->count;
	return INI_OK;
}

void ini_node_free(INI_NODE *node)
{
	INI_NODE *temp;

	if(node)
	{
		while(node)
		{
			temp=node;
			node=node->next;

			if(temp->key)
				free(temp->key);
			if(temp->value)
				free(temp->value);
			free(temp);
		}
	}
}

void ini_table_free(INI_TABLE *table)
{
	INI_TABLE *temp;

	if(table)
	{
		while(table)
		{
			temp=table;
			table=table->next;

			free(temp->name);
			ini_node_free(temp->node);
			free(temp);
		}
	}
}

void ini_free(INI *ini)
{
	if(ini)
	{
		if(ini->table)
			ini_table_free(ini->table);

		free(ini);
	}
}

char *ini_table_get_key_value(INI_TABLE *table,const char *name,const char *key)
{
	INI_NODE *node;

	while(table)
	{
		if(strcmp(table->name,name) == 0)
			break;

		table=table->next;
	}
	if(table == NULL) return NULL;
	node=table->node;
	while(node)
	{
		if(strcmp(node->key,key) == 0)
			return node->value;

		node=node->next;
	}

	return NULL;
}

int ini_table_set_name(INI_TABLE *table,const char *name,
		const char *new_name)
{
	if(!table) return INI_NO;

	while(table)
	{
		if(strcmp(table->name,name) == 0)
		{
			free(table->name);
			table->name=strdup(new_name);
			if(!table->name) return INI_TABLE_NO_MEM;

			return INI_OK;
		}

		table=table->next;
	}

	return INI_TABLE_NOT_FOUND;
}

int ini_node_set_key(INI_TABLE *table,const char *name,
		const char *key,const char *new_key)
{
	INI_NODE *node;

	if(!table) return INI_NO;
	while(table)
	{
		if(strcmp(table->name,name) == 0)
			break;

		table=table->next;
	}

	if(!table) return INI_TABLE_NOT_FOUND;
	node=table->node;
	if(!node) return INI_NODE_NOT_FOUND;
	while(node)
	{
		if(strcmp(node->key,key) == 0)
		{
			free(node->key);
			node->key=strdup(new_key);
			if(node->key == NULL) return INI_NODE_KEY;

			return INI_OK;
		}

		node=node->next;
	}

	return INI_NODE_NOT_FOUND;
}

int ini_node_set_value(INI_TABLE *table,const char *name,
		const char *key,const char *new_value)
{
	INI_NODE *node;

	if(!table) return INI_NO;
	while(table)
	{
		if(strcmp(table->name,name) == 0)
			break;

		table=table->next;
	}

	if(!table) return INI_TABLE_NOT_FOUND;
	node=table->node;
	if(!node) return INI_NODE_NOT_FOUND;
	while(node)
	{
		if(strcmp(node->key,key) == 0)
		{
			if(node->value)
				free(node->value);

			if(new_value == NULL)
			{
				node->value=NULL;
				return INI_OK;
			}

			node->value=strdup(new_value);
			if(!node->value) return INI_NODE_VALUE;

			return INI_OK;
		}

		node=node->next;
	}

	return INI_NODE_NOT_FOUND;
}

int ini_node_set(INI_TABLE *table,const char *name,
		const char *key,const char *new_key,
		const char *new_value)
{
	int status;

	status=ini_node_set_value(table,name,key,new_value);
	if(status != INI_OK) return status;
	return ini_node_set_key(table,name,key,new_key);
}
