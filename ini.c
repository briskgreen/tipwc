#include "ini.h"

#define INI_ERROR_RETURN(code) \
{\
	ini_errno=code;\
	return NULL;\
}

void ini_build_node(INI_NODE *node,SBUF *buf);

INI *ini_parse(const char *string)
{
	INI *ini;
	SBUF *str;
	SBUF *temp;
	SBUF *node;
	SBUF *value;
	char *p;
	int len;
	int i;

	ini=ini_new();
	if(!ini) INI_ERROR_RETURN(INI_NO_MEM);
	str=sbuf_new(512);
	if(!str) INI_ERROR_RETURN(INI_SBUF_NO_MEM);
	temp=sbuf_new(512);
	if(!temp)
	{
		sbuf_free(str);
		INI_ERROR_RETURN(INI_SBUF_NO_MEM);
	}
	value=sbuf_new(512);
	if(!value)
	{
		sbuf_free(str);
		sbuf_free(temp);
		INI_ERROR_RETURN(INI_SBUF_NO_MEM);
	}
	node=sbuf_new(512);
	if(!node)
	{
		sbuf_free(str);
		sbuf_free(temp);
		sbuf_free(value);
		INI_ERROR_RETURN(INI_SBUF_NO_MEM);
	}

	while(p=strstr(string,"\n"))
	{
		len=p-string;
		if(len == 0)
		{
			++string;
			continue;
		}
		sbuf_nappend(temp,len,"%s",string);

		switch(temp->buf[0])
		{
			case '[':
				sbuf_resert(node);
				for(i=1;temp->buf[i] != ']';++i)
					sbuf_append_chr(node,temp->buf[i]);

				ini_table_add(ini,node->buf);
				break;
			case '#':
			case ';':
				break;
			default:
				{
					char *p;
					int len;
					INI_NODE *n;

					p=strchr(temp->buf,'=');
					len=p-temp->buf;
					sbuf_nappend(str,len,"%s",temp->buf);
					++p;
					//sbuf_resert(value);
					sbuf_nappend(value,temp->len-len,"%s",p);
					n=ini_node_new(str->buf,value->buf);
					ini_node_add(ini->table,node->buf,n);
				}
		}

		sbuf_resert(temp);
		//sbuf_resert(node);
		sbuf_resert(str);
		sbuf_resert(value);
		string+=len+1;
	}

	sbuf_free(temp);
	sbuf_free(str);
	sbuf_free(node);
	sbuf_free(value);

	return ini;
}

INI *ini_parse_with_filename(const char *filename)
{
	INI *ini;
	char *string;
	size_t len;
	FILE *fp;

	if(!(fp=fopen(filename,"rb")))
	{
		ini_errno=INI_OPEN_FILE;
		return NULL;
	}
	fseek(fp,0L,SEEK_END);
	len=ftell(fp);
	rewind(fp);

	string=(char *)malloc(sizeof(char)*(len+1));
	if(string == NULL)
	{
		ini_errno=INI_STRING_NO_MEM;
		fclose(fp);

		return NULL;
	}
	if(fread(string,sizeof(char)*len,1,fp) != 1)
	{
		ini_errno=INI_READ_FILE;
		fclose(fp);
		free(string);

		return NULL;
	}
	string[len]='\0';
	fclose(fp);
	ini=ini_parse(string);
	free(string);

	return ini;
}

int ini_build(INI *ini,const char **string)
{
	INI_TABLE *table;
	SBUF *str;

	if(!ini || !ini->table) return INI_NO;
	table=ini->table;
	str=sbuf_new(1024);
	if(!str) return INI_SBUF_NO_MEM;
	while(table)
	{
		sbuf_append(str,"[%s]\n",table->name);
		ini_build_node(table->node,str);
		sbuf_append_chr(str,'\n');

		table=table->next;
	}

	*string=str->buf;
	free(str);

	return INI_OK;
}

int ini_build_to_file(INI *ini,const char *filename)
{
	int status;
	FILE *fp;
	const char *res;

	if(!(fp=fopen(filename,"wb")))
		return INI_CREATE_FILE;

	status=ini_build(ini,&res);
	if(status != INI_OK)
	{
		fclose(fp);
		return status;
	}
	fwrite(res,sizeof(char)*strlen(res),1,fp);
	free((void *)res);
	fclose(fp);

	return INI_OK;
}

void ini_build_node(INI_NODE *node,SBUF *buf)
{
	while(node)
	{
		sbuf_append(buf,"%s = %s\n",node->key,node->value);
		node=node->next;
	}
}

int ini_get_table_num(INI *ini,const char *name)
{
	INI_TABLE *table;

	table=ini->table;
	while(table)
	{
		if(strcmp(table->name,name) == 0)
			return table->count;

		table=table->next;
	}

	ini_errno=INI_NO;
	return -1;
}
