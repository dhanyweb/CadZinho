#ifndef _DXF_LIB
#define _DXF_LIB

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "list.h"

#define DXF_MAX_LAYERS 1000
#define DXF_MAX_LTYPES 1000
#define DXF_MAX_FONTS 1000
#define DXF_MAX_CHARS 250
#define DXF_MAX_PAT 10
#define DXF_POOL_PAGES 1000

#if defined(_WIN32) || defined(WIN32) || defined(__CYGWIN__) || defined(__MINGW32__) || defined(__BORLANDC__)
#define PATH_SEPARATOR ';'
#define DIR_SEPARATOR '\\'
#else
#define PATH_SEPARATOR ':'
#define DIR_SEPARATOR '/'
#endif

struct sort_by_idx{
	long int idx;
	void *data;
};

/* supportable graphic entities */
enum dxf_graph {
	DXF_NONE 		= 0x0,
	DXF_LINE 		= 0x1,
	DXF_POINT 		= 0x2,
	DXF_CIRCLE 		= 0x4,
	DXF_ARC 			= 0x8,
	DXF_TRACE 		= 0x10,
	DXF_SOLID 		= 0x20,
	DXF_TEXT 		= 0x40,
	DXF_SHAPE 		= 0x80,
	DXF_INSERT 		= 0x100,
	DXF_ATTRIB 		= 0x200,
	DXF_POLYLINE 		= 0x400,
	DXF_VERTEX 		= 0x800,
	DXF_LWPOLYLINE 	= 0x1000,
	DXF_3DFACE 		= 0x2000,
	DXF_VIEWPORT 	= 0x4000,
	DXF_DIMENSION 	= 0x8000,
	DXF_ELLIPSE 		= 0x10000,
	DXF_MTEXT 		= 0x20000,
	DXF_BLK 			= 0x40000,
	DXF_ENDBLK 		= 0x80000,
	DXF_HATCH 		= 0x100000,
	DXF_DIMSTYLE 	= 0x200000,
	DXF_IMAGE 		= 0x400000,
	DXF_IMAGE_DEF 	= 0x800000
};

enum dxf_pool_action{
	ADD_DXF,
	ZERO_DXF,
	FREE_DXF
};

enum dxf_pool_life{
	DWG_LIFE = 0,
	FRAME_LIFE = 1,
	ONE_TIME = 2,
	PRG_LIFE = 3,
	SEL_LIFE = 4
};

struct Dxf_pool_slot{
	void *pool[DXF_POOL_PAGES];
	/* the pool is a vector of pages. The size of each page is out of this definition */
	int pos; /* current position in current page vector */
	int page; /* current page index */
	int size; /* number of pages available in slot */
};
typedef struct Dxf_pool_slot dxf_pool_slot;

struct Dxf_node{
	struct Dxf_node *master; /* entity to which it is contained */
	struct Dxf_node *next;    /* next node (double linked list) */
	struct Dxf_node *prev;    /* previous node (double linked list) */
	struct Dxf_node *end; /*last object in list*/
	
	/* defines whether it is an DXF entity (obj) or an attribute (value) */
	enum {DXF_ENT, DXF_ATTR} type;
	
	union{
		struct {
			/* == entity dxf especific */
			char name[DXF_MAX_CHARS]; /* standardized DXF name of entity */
			int layer;
			int pool;
			void * graphics; /* graphics information */
			
			struct Dxf_node *content; /* the content is a list */
		} obj;
		
		struct {
			/* ==group dxf especific */
			int group; /* standardized DXF group */
			/* the group defines the type of data, which can be: */
			enum {DXF_FLOAT, DXF_INT, DXF_STR} t_data;
			union {
				double d_data; /* a float number, */
				int i_data; /* a integer number, */
				char s_data[DXF_MAX_CHARS]; /* or a string. */
			};
		} value;
	};
}; 
typedef struct Dxf_node dxf_node;

struct Dxf_layer{
	char name[DXF_MAX_CHARS];
	int color;
	char ltype[DXF_MAX_CHARS];
	int line_w;
	int frozen;
	int lock;
	int off;
	int num_el;
	dxf_node *obj;
};
typedef struct Dxf_layer dxf_layer;

struct Dxf_ltype{
	char name[DXF_MAX_CHARS];
	char descr[DXF_MAX_CHARS];
	int size;
	double pat[DXF_MAX_PAT];
	double length;
	int num_el;
	dxf_node *obj;
};
typedef struct Dxf_ltype dxf_ltype;

struct Dxf_tstyle{
	char name[DXF_MAX_CHARS];
	char file[DXF_MAX_CHARS];
	char big_file[DXF_MAX_CHARS];
	char subst_file[DXF_MAX_CHARS];
	
	int flags1;
	int flags2;
	int num_el;
	
	double fixed_h;
	double width_f;
	double oblique;
	
	void *font;
	
	dxf_node *obj;
};
typedef struct Dxf_tstyle dxf_tstyle;

struct Dxf_drawing{
	int pool; /* memory pool location */
	/* DXF main sections */
	dxf_node 	
	*head,  /* header with drawing variables */
	*tabs,  /* tables - see next */
	*blks, /* blocks definitions */
	*ents, /* entities - grahics elements */
	*objs; /* objects - non grahics elements */
	
	/* DXF tables */
	dxf_node 
	*t_ltype, /* line types */ 
	*t_layer,  /* layers */
	*t_style,  /* text styles, text fonts */
	*t_view,  /* views */
	*t_ucs,   /* UCS - coordinate systems */
	*t_vport,  /* viewports (useful in layout mode) */
	*t_dimst, /* dimension styles*/
	*blks_rec, /* blocks records */
	*t_appid; /* third part application indentifier */
	
	/* complete structure access */
	dxf_node *main_struct;
	
	dxf_node *hand_seed; /* handle generator */
	
	dxf_layer layers[DXF_MAX_LAYERS];
	int num_layers;
	
	dxf_ltype ltypes[DXF_MAX_LTYPES];
	int num_ltypes;
	
	dxf_tstyle text_styles[DXF_MAX_FONTS];
	int num_tstyles;
	
	void *font_list;
	void *dflt_font;
	
	void *img_list;
	
	int version;
	
};
typedef struct Dxf_drawing dxf_drawing;

/* functions*/
void str_upp(char *str);

char * trimwhitespace(char *str);

char *get_filename(char *path);

char *get_dir(char *path);

char *get_ext(char *path);

void strip_ext(char *filename);

int file_exists(char *fname);

int utf8_to_codepoint(char *utf8_s, int *uni_c);

int codepoint_to_utf8(int uni_c, char utf8_s[5]);

int str_utf2cp(char *str, int *cp, int max);

void dxf_ent_print2 (dxf_node *ent);

void dxf_ent_print_f (dxf_node *ent, char *path);

dxf_node * dxf_obj_new (char *name, int pool);

int dxf_ident_attr_type (int group);

int dxf_ident_ent_type (dxf_node *obj);

dxf_node * dxf_attr_new (int group, int type, void *value, int pool);

//vector_p dxf_find_attr(dxf_node * obj, int attr);

dxf_node * dxf_find_attr2(dxf_node * obj, int attr);

dxf_node * dxf_find_attr_i(dxf_node * obj, int attr, int idx);

dxf_node * dxf_find_attr_i2(dxf_node * start, dxf_node * end, int attr, int idx);

int dxf_count_attr(dxf_node * obj, int attr);

//vector_p dxf_find_obj(dxf_node * obj, char *name);

dxf_node * dxf_find_obj_i(dxf_node * obj, char *name, int idx);

//vector_p dxf_find_obj_descr(dxf_node * obj, char *name, char *descr);

dxf_node * dxf_find_obj_descr2(dxf_node * obj, char *name, char *descr);

void dxf_layer_assemb (dxf_drawing *drawing);

void dxf_ltype_assemb (dxf_drawing *drawing);

void dxf_tstyles_assemb (dxf_drawing *drawing);

int dxf_lay_idx (dxf_drawing *drawing, char *name);

int dxf_ltype_idx (dxf_drawing *drawing, char *name);

int dxf_tstyle_idx (dxf_drawing *drawing, char *name);

int dxf_layer_get(dxf_drawing *drawing, dxf_node * obj);

int dxf_save (char *path, dxf_drawing *drawing);

char * dxf_load_file(char *path, long *fsize);

int dxf_read (dxf_drawing *drawing, char *buf, long fsize, int *prog);

dxf_node * dxf_find_obj2(dxf_node * obj, char *name);

void * dxf_mem_pool(enum dxf_pool_action action, int idx);

void dxf_append(dxf_node *master, dxf_node *new_node);

int dxf_find_head_var(dxf_node *obj, char *var, dxf_node **start, dxf_node **end);

dxf_drawing *dxf_drawing_new(int pool);

int dxf_drawing_clear (dxf_drawing *drawing);

int dxf_obj_subst(dxf_node *orig, dxf_node *repl);

int dxf_obj_append(dxf_node *master, dxf_node *obj);

int dxf_obj_detach(dxf_node *obj);

int dxf_attr_append(dxf_node *master, int group, void *value, int pool);

int dxf_attr_insert_before(dxf_node *attr, int group, void *value, int pool);

dxf_node * dxf_attr_insert_after(dxf_node *attr, int group, void *value, int pool);

int dxf_attr_change(dxf_node *master, int group, void *value);

int dxf_attr_change_i(dxf_node *master, int group, void *value, int idx);

int dxf_find_ext_appid(dxf_node *obj, char *appid, dxf_node **start, dxf_node **end);

int dxf_ext_append(dxf_node *master, char *appid, int group, void *value, int pool);

int ent_handle(dxf_drawing *drawing, dxf_node *element);

dxf_node *dxf_find_handle(dxf_node *source, long int handle);

void drawing_ent_append(dxf_drawing *drawing, dxf_node *element);

list_node * dxf_ents_list(dxf_drawing *drawing, int pool_idx);

#endif