#pragma once
#include "mediastreamer2/mediastream.h"
#include "mediastreamer2/msvolume.h"
#include "mediastreamer2/msequalizer.h"
#define lp_new0(type,n)	(type*)calloc(sizeof(type),n)
#define LISTNODE(_struct_)	\
	struct _struct_ *_prev;\
	struct _struct_ *_next;

typedef struct _ListNode{
	LISTNODE(_ListNode)
} ListNode;

typedef void (*ListNodeForEachFunc)(ListNode *);

#define LIST_PREPEND(e1,e2) (  (e2)->_prev=NULL,(e2)->_next=(e1),(e1)->_prev=(e2),(e2) )
#define LIST_APPEND(head,elem) ((head)==0 ? (elem) : (list_node_append((ListNode*)(head),(ListNode*)(elem)), (head)) ) 
#define LIST_REMOVE(head,elem) 

/* returns void */
#define LIST_FOREACH(head) list_node_foreach((ListNode*)head)

typedef struct _LpItem{
	char *key;
	char *value;
} LpItem;

typedef struct _LpSection{
	char *name;
	MSList *items;
} LpSection;

class CConfigure
{
public:
	CConfigure(void);
	CConfigure(const char *filename);
	~CConfigure(void);
public:
	FILE *file;
	char *filename;
	MSList *sections;
	int modified;

public:
ListNode * list_node_append(ListNode *head,ListNode *elem);
ListNode * list_node_remove(ListNode *head, ListNode *elem);
void list_node_foreach(ListNode *head, ListNodeForEachFunc func);
LpItem * lp_item_new(const char *key, const char *value);
LpSection *lp_section_new(const char *name);
static void lp_item_destroy(void *pitem);
static void lp_section_destroy(LpSection *sec);

void lp_section_add_item(LpSection *sec,LpItem *item);
void lp_config_add_section(LpSection *section);
void lp_config_remove_section(LpSection *section);
bool_t is_first_char(const char *start, const char *pos);
void lp_config_parse();
//LpConfig * lp_config_new(const char *filename);
void lp_item_set_value(LpItem *item, const char *value);
void lp_config_destroy();
LpSection *lp_config_find_section( const char *name);
LpItem *lp_section_find_item(LpSection *sec, const char *name);
void lp_section_remove_item(LpSection *sec, LpItem *item);
char *lp_config_get_string( const char *section, const char *key,  char *default_string);
int lp_config_get_int(const char *section, const char *key, int default_value);
float lp_config_get_float(const char *section, const char *key, float default_value);
void lp_config_set_string(const char *section, const char *key, const char *value);
void lp_config_set_int(const char *section, const char *key, int value);
static void lp_item_write(LpItem *item, FILE *file);
static void lp_section_write(LpSection *sec, FILE *file);
int lp_config_sync();
int lp_config_has_section(const char *section);
void lp_config_clean_section(const char *section);
int lp_config_needs_commit();
};
