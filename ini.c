#include "ini.h"

#define INI_ERROR_RETURN(code) \
{\
	ini_errno=code;\
	return NULL;\
}

#define INI_POP *buf
#define INI_ADVANCE ++buf
#define INI_PUSH --buf

void ini_build_node(INI_NODE *node,SBUF *buf);
int _ini_parse_line(INI *ini,char *buf,SBUF *node,SBUF *str,SBUF *value);
int _ini_get(SBUF *sbuf,char *buf,int len);

INI *ini_parse(const char *string)
{
	INI *ini;
	SBUF *str;
	SBUF *temp;
	SBUF *node;
	SBUF *value;
	char *p;
	int len;

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
		if(_ini_parse_line(ini,temp->buf,node,str,value) != INI_OK)
		{
			ini_free(ini);
			break;
		}

		sbuf_reset(temp);
		sbuf_reset(str);
		sbuf_reset(value);
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

void ini_build_free(const char *string)
{
	free((void *)string);
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
	ini_build_free(res);
	fclose(fp);

	return INI_OK;
}

void ini_build_node(INI_NODE *node,SBUF *buf)
{
	while(node)
	{
		if(node->value == NULL)
			sbuf_append(buf,"%s =\n",node->key);
		else
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

int _ini_parse_line(INI *ini,char *buf,SBUF *node,SBUF *str,SBUF *value)
{
	char *p;
	INI_NODE *n;

	while(isspace(INI_POP))
		INI_ADVANCE;

	while(INI_POP)
	{
		switch(INI_POP)
		{
			case '[':
				sbuf_reset(node);
				p=strchr(buf,']');
				if(p == NULL)
				{
					ini_errno=INI_PARSE_NODE_ERROR;
					return INI_NO;
				}
				sbuf_nappend(node,p-buf-1,"%s",buf+1);
				if(ini_table_add(ini,node->buf) != INI_OK) return INI_NO;
				return INI_OK;
			case ';':
				return INI_OK;
			default:
				p=strchr(buf,'=');
				if(p == NULL || p == buf)
				{
					ini_errno=INI_PARSE_KEY_ERROR;
					return INI_NO;
				}
				if(_ini_get(str,buf,p-buf) != INI_OK)
				{
					ini_errno=INI_PARSE_KEY_ERROR;
					return INI_NO;
				}
				if(_ini_get(value,p+1,-1) != INI_OK)
					n=ini_node_new(str->buf,NULL);
				else
					n=ini_node_new(str->buf,value->buf);
				if(n == NULL) return INI_NO;
				if(ini_node_add(ini->table,node->buf,n) != INI_OK) return INI_NO;

				return INI_OK;
		}
	}

	return INI_OK;
}

int _ini_get(SBUF *sbuf,char *buf,int len)
{
	if(len == -1)
		len=strlen(buf);
	while(isspace(buf[len-1]))
		--len;
	buf[len]='\0';

	while(isspace(INI_POP))
		INI_ADVANCE;
	if(!INI_POP) return INI_NO;

	while(INI_POP)
	{
		switch(INI_POP)
		{
			case '\\':
				INI_ADVANCE;
				if(!INI_POP) return INI_NO;
				switch(INI_POP)
				{
					case 'n':
						sbuf_append_chr(sbuf,'\n');
						break;
					case 'r':
						sbuf_append_chr(sbuf,'\r');
						break;
					case 't':
						sbuf_append_chr(sbuf,'\t');
						break;
					default:
						sbuf_append_chr(sbuf,INI_POP);
				}
				break;
			default:
				sbuf_append_chr(sbuf,INI_POP);
				break;
		}

		INI_ADVANCE;
	}

	return INI_OK;
}
