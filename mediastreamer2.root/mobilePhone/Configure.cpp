#include "StdAfx.h"
#include "Configure.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <errno.h>
//#include <sys/types.h>
//#include <sys/stat.h>
//#include "unistd.h"
#define MAX_LEN 1024

CConfigure::CConfigure(void)
{
}
CConfigure::CConfigure(const char *filename){

	file=NULL;
	//filename=NULL;
	sections=NULL;

	if (filename!=NULL){
		this->filename=_strdup(filename);
		file=fopen(this->filename,"r+");
		if (file!=NULL){
			lp_config_parse();
			fclose(file);
			/* make existing configuration files non-group/world-accessible */
			/*if (chmod(filename, S_IREAD | S_IWRITE) == -1)
			ms_warning("unable to correct permissions on "
			"configuration file: %s",
			strerror(errno));*/
			file=NULL;
			modified=0;
		}
	}
}

CConfigure::~CConfigure(void)
{
	if(file!=NULL)
		delete file;
	if(filename!=NULL)
		delete filename;
	if(sections!=NULL)
		delete sections;
}

ListNode * CConfigure::list_node_append(ListNode *head,ListNode *elem){
	ListNode *e=head;
	while(e->_next!=NULL) e=e->_next;
	e->_next=elem;
	elem->_prev=e;
	return head;
}

ListNode * CConfigure::list_node_remove(ListNode *head, ListNode *elem){
	ListNode *before,*after;
	before=elem->_prev;
	after=elem->_next;
	if (before!=NULL) before->_next=after;
	if (after!=NULL) after->_prev=before;
	elem->_prev=NULL;
	elem->_next=NULL;
	if (head==elem) return after;
	return head;
}

void CConfigure::list_node_foreach(ListNode *head, ListNodeForEachFunc func){
	for (;head!=NULL;head=head->_next){
		func(head);
	}
}

LpItem * CConfigure::lp_item_new(const char *key, const char *value){
	LpItem *item=lp_new0(LpItem,1);
	item->key=_strdup(key);
	item->value=_strdup(value);
	return item;
}

LpSection *CConfigure::lp_section_new(const char *name){
	LpSection *sec=lp_new0(LpSection,1);
	sec->name=_strdup(name);
	return sec;
}

void CConfigure::lp_item_destroy(void *pitem){
	LpItem *item=(LpItem*)pitem;
	free(item->key);
	free(item->value);
	free(item);
}

void CConfigure::lp_section_destroy(LpSection *sec){
	free(sec->name);
	ms_list_for_each(sec->items,&CConfigure::lp_item_destroy);
	ms_list_free(sec->items);
	free(sec);
}

void CConfigure::lp_section_add_item(LpSection *sec,LpItem *item){
	sec->items=ms_list_append(sec->items,(void *)item);
}

void CConfigure::lp_config_add_section( LpSection *section){
	sections=ms_list_append(sections,(void *)section);
}

void CConfigure::lp_config_remove_section( LpSection *section){
	sections=ms_list_remove(sections,(void *)section);
	lp_section_destroy(section);
}

bool_t CConfigure::is_first_char(const char *start, const char *pos){
	const char *p;
	for(p=start;p<pos;p++){
		if (*p!=' ') return FALSE;
	}
	return TRUE;
}

void CConfigure::lp_config_parse(){
	char tmp[MAX_LEN];
	LpSection *cur=NULL;

	if (file==NULL) return;

	while(fgets(tmp,MAX_LEN,file)!=NULL){
		char *pos1,*pos2;
		pos1=strchr(tmp,'[');
		if (pos1!=NULL && is_first_char(tmp,pos1) ){
			pos2=strchr(pos1,']');
			if (pos2!=NULL){
				int nbs;
				char secname[MAX_LEN];
				secname[0]='\0';
				/* found section */
				*pos2='\0';
				nbs = sscanf(pos1+1,"%s",secname);
				if (nbs == 1 ){
					if (strlen(secname)>0){
						cur=lp_section_new(secname);
						lp_config_add_section(cur);
					}
				}else{
					OutputDebugString(_T("parse error!"));
				}
			}
		}else {
			pos1=strchr(tmp,'=');
			if (pos1!=NULL){
				char key[MAX_LEN];
				key[0]='\0';

				*pos1='\0';
				if (sscanf(tmp,"%s",key)>0){

					pos1++;
					pos2=strchr(pos1,'\n');
					if (pos2==NULL) pos2=pos1+strlen(pos1);
					else {
						*pos2='\0'; /*replace the '\n' */
						pos2--;
					}
					/* remove ending white spaces */
					for (; pos2>pos1 && *pos2==' ';pos2--) *pos2='\0';
					if (pos2-pos1>=0){
						/* found a pair key,value */
						if (cur!=NULL){
							lp_section_add_item(cur,lp_item_new(key,pos1));
							/*printf("Found %s %s=%s\n",cur->name,key,pos1);*/
						}else{
							OutputDebugString(_T("found key,item but no sections"));
						}
					}
				}
			}
		}
	}
}


void CConfigure::lp_item_set_value(LpItem *item, const char *value){
	free(item->value);
	item->value=_strdup(value);
}


void CConfigure::lp_config_destroy(){
	if (filename!=NULL) free(filename);
	ms_list_for_each(sections,(void (*)(void*))lp_section_destroy);
	ms_list_free(sections);
	free(this);
}

LpSection *CConfigure::lp_config_find_section(const char *name){
	LpSection *sec;
	MSList *elem;
	/*printf("Looking for section %s\n",name);*/
	for (elem=sections;elem!=NULL;elem=ms_list_next(elem)){
		sec=(LpSection*)elem->data;
		if (strcmp(sec->name,name)==0){
			/*printf("Section %s found\n",name);*/
			return sec;
		}
	}
	return NULL;
}

LpItem *CConfigure::lp_section_find_item(LpSection *sec, const char *name){
	MSList *elem;
	LpItem *item;
	/*printf("Looking for item %s\n",name);*/
	for (elem=sec->items;elem!=NULL;elem=ms_list_next(elem)){
		item=(LpItem*)elem->data;
		if (strcmp(item->key,name)==0) {
			/*printf("Item %s found\n",name);*/
			return item;
		}
	}
	return NULL;
}

void CConfigure::lp_section_remove_item(LpSection *sec, LpItem *item){
	sec->items=ms_list_remove(sec->items,(void *)item);
	lp_item_destroy(item);
}

char* CConfigure::lp_config_get_string(const char *section, const char *key,  char *default_string){
	LpSection *sec;
	LpItem *item;
	sec=lp_config_find_section(section);
	if (sec!=NULL){
		item=lp_section_find_item(sec,key);
		if (item!=NULL) return item->value;
	}
	return default_string;
}

int CConfigure::lp_config_get_int(const char *section, const char *key, int default_value){
    char *str=lp_config_get_string(section,key,NULL);
	if (str!=NULL) return atoi(str);
	else return default_value;
}

float CConfigure::lp_config_get_float(const char *section, const char *key, float default_value){
	const char *str=lp_config_get_string(section,key,NULL);
	float ret=default_value;
	if (str==NULL) return default_value;
	sscanf(str,"%f",&ret);
	return ret;
}

void CConfigure::lp_config_set_string(const char *section, const char *key, const char *value){
	LpItem *item;
	LpSection *sec=lp_config_find_section(section);
	if (sec!=NULL){
		item=lp_section_find_item(sec,key);
		if (item!=NULL){
			if (value!=NULL)
				lp_item_set_value(item,value);
			else lp_section_remove_item(sec,item);
		}else{
			if (value!=NULL)
				lp_section_add_item(sec,lp_item_new(key,value));
		}
	}else if (value!=NULL){
		sec=lp_section_new(section);
		lp_config_add_section(sec);
		lp_section_add_item(sec,lp_item_new(key,value));
	}
	modified++;
}

void CConfigure::lp_config_set_int(const char *section, const char *key, int value){
	char tmp[30];
	snprintf(tmp,30,"%i",value);
	lp_config_set_string(section,key,tmp);
	modified++;
}

void CConfigure::lp_item_write(LpItem *item, FILE *file){
	fprintf(file,"%s=%s\n",item->key,item->value);
}

void CConfigure::lp_section_write(LpSection *sec, FILE *file){
	fprintf(file,"[%s]\n",sec->name);
	ms_list_for_each2(sec->items,(void (*)(void*, void*))lp_item_write,(void *)file);
	fprintf(file,"\n");
}

int CConfigure::lp_config_sync(){
	FILE *file;
	if (filename==NULL) return -1;
#ifndef WIN32
	/* don't create group/world-accessible files */
	(void) umask(S_IRWXG | S_IRWXO);
#endif
	file=fopen(filename,"w");
	if (file==NULL){
		OutputDebugString(_T("Could not write %s !"));
		return -1;
	}
	ms_list_for_each2(sections,(void (*)(void *,void*))lp_section_write,(void *)file);
	fclose(file);
	modified=0;
	return 0;
}

int CConfigure::lp_config_has_section(const char *section){
	if (lp_config_find_section(section)!=NULL) return 1;
	return 0;
}

void CConfigure::lp_config_clean_section(const char *section){
	LpSection *sec=lp_config_find_section(section);
	if (sec!=NULL){
		lp_config_remove_section(sec);
	}
	modified++;
}

int CConfigure::lp_config_needs_commit(){
	return modified>0;
}
