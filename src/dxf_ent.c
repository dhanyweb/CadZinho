#include "dxf_ent.h"


int dxf_lwpline_get_pt(dxf_node * obj, dxf_node ** next,
double *pt1_x, double *pt1_y, double *pt1_z, double *bulge){
	
	dxf_node *current = NULL;
	static dxf_node *last = NULL;
	static double extru_x = 0.0, extru_y = 0.0, extru_z = 1.0, normal[3];
	
	/*flags*/
	int first = 0, pt1 = 0, ok = 0;
	static int pline_flag = 0, closed =0, init = 0;
	
	double px = 0.0, py = 0.0, pz = 0.0, bul = 0.0;
	static double last_x, last_y, last_z, curr_x, elev;
	
	double pt[3]; /*return values */
	
	if (*next == NULL){ /* parse object first time */
		pline_flag = 0; closed =0; init = 0;
		last = NULL;
		if(obj){
			if (obj->type == DXF_ENT){
				if (obj->obj.content){
					current = obj->obj.content->next;
					//printf("%s\n", obj->obj.name);
				}
			}
		}
		/* get general parameters of polyline */
		while (current){
			if (current->type == DXF_ATTR){ /* DXF attibute */
				switch (current->value.group){
					case 10:
						px = current->value.d_data;
						pt1 = 1; /* set flag */
						break;
					case 20:
						py = current->value.d_data;
						//pt1 = 1; /* set flag */
						break;
					case 30:
						pz = current->value.d_data;
						//pt1 = 1; /* set flag */
						break;
					case 42:
						bul = current->value.d_data;
						break;
					case 70:
						pline_flag = current->value.i_data;
						break;
					case 38:
						elev = current->value.d_data;
						break;
					case 210:
						extru_x = current->value.d_data;
						break;
					case 220:
						extru_y = current->value.d_data;
						break;
					case 230:
						extru_z = current->value.d_data;
						break;
				}
			}
			if (pt1){
				pt1 = 0;
				if (init == 0){
					init = 1;
					curr_x = px;
					last = current;
				}
				else{
					*next = current;
					break;
				}
			}
			current = current->next; /* go to the next in the list */
			//*next = current->next;
		}
		if (init){
			if (pline_flag & 1){
				closed = 1;
				/* if closed, the last vertex is first */
				last_x = curr_x;
				last_y = py;
				last_z = pz;
			}
			else {
				closed = 0;
				last = NULL;
			}
			
			/* to convert OCS to WCS */
			normal[0] = extru_x;
			normal[1] = extru_y;
			normal[2] = extru_z;			
			
			pt[0] = curr_x;
			pt[1] = py;
			pt[2] = pz;
			*bulge = bul;
			ok = 1;
			
		}
	}
	else if ((init) && (*next != last)){ /* continue search in next point */
		current = *next;
		while (current){
			if (current->type == DXF_ATTR){ /* DXF attibute */
				switch (current->value.group){
					case 10:
						px = current->value.d_data;
						pt1 = 1; /* set flag */
						break;
					case 20:
						py = current->value.d_data;
						//pt1 = 1; /* set flag */
						break;
					case 30:
						pz = current->value.d_data;
						//pt1 = 1; /* set flag */
						break;
					case 42:
						bul = current->value.d_data;
						break;
				}
			}
			if (pt1){
				pt1 = 0;
				if (first == 0){
					first = 1;
					curr_x = px;
				}
				else{
					*next = current;
					break;
				}
			}
			current = current->next; /* go to the next in the list */
		}
		if (first){
			
			pt[0] = curr_x;
			pt[1] = py;
			pt[2] = pz;
			*bulge = bul;
			ok = 1;
			if (current == NULL){
				if (closed){
					*next = last;
				}
				else {
					*next = NULL;
					pline_flag = 0; closed =0; init = 0;
					last = NULL;
				}
			}
		}
		else if (closed){
			*next = last;
		}
		else {
			*next = NULL;
			pline_flag = 0; closed =0; init = 0;
			last = NULL;
		}
	}
	else { /* last vertex */
		pt[0] = last_x;
		pt[1] = last_y;
		pt[2] = last_z;
		*bulge = 0.0;
		ok = 1;
		*next = NULL;
		pline_flag = 0; closed =0; init = 0;
		last = NULL;
	}
	
	if (ok == 0){
		*next = NULL;
		pline_flag = 0; closed =0; init = 0;
		last = NULL;
	}
	/* convert OCS to WCS */
	else {
		mod_axis(pt, normal , 0.0);
		/* update return values */
		*pt1_x = pt[0];
		*pt1_y = pt[1];
		*pt1_z = pt[2];
	}
	
	return ok;
}

int dxf_pline_get_pt(dxf_node * obj, dxf_node ** next,
double *pt1_x, double *pt1_y, double *pt1_z, double *bulge){
	
	dxf_node *current = NULL;
	static dxf_node *last = NULL;
	static double extru_x = 0.0, extru_y = 0.0, extru_z = 1.0, normal[3];
	
	/*flags*/
	int first = 0, pt1 = 0, ok = 0;
	static int pline_flag = 0, closed =0, init = 0;
	
	double px = 0.0, py = 0.0, pz = 0.0, bul = 0.0;
	static double last_x, last_y, last_z, elev;
	
	double pt[3]; /*return values */
	
	if (*next == NULL){ /* parse object first time */
		pline_flag = 0; closed =0; init = 0;
		last = NULL;
		if(obj){
			if (obj->type == DXF_ENT){
				if (obj->obj.content){
					current = obj->obj.content->next;
				}
			}
		}
		/* get general parameters of polyline */
		while (current){
			if (current->type == DXF_ATTR){
				switch (current->value.group){
					case 70:
						pline_flag = current->value.i_data;
						break;
					case 38:
						elev = current->value.d_data;
						break;
					case 210:
						extru_x = current->value.d_data;
						break;
					case 220:
						extru_y = current->value.d_data;
						break;
					case 230:
						extru_z = current->value.d_data;
						break;
				}
			}
			else if (current->type == DXF_ENT){
				if (strcmp(current->obj.name, "VERTEX") == 0 ){
					pt1 = 1; /* found vertex */
				}
			}
			if (pt1){
				pt1 = 0;
				if (init == 0){
					/* get current vertex parameters */
					dxf_node *curr_vtx = current->obj.content;
					while(curr_vtx){
						if (curr_vtx->type == DXF_ATTR){ 
							switch (curr_vtx->value.group){
								case 10:
									px = curr_vtx->value.d_data;
									break;
								case 20:
									py = curr_vtx->value.d_data;
									break;
								case 30:
									pz = curr_vtx->value.d_data;
									break;
								case 42:
									bul = curr_vtx->value.d_data;
									break;
							}
						}
						curr_vtx = curr_vtx->next;
					}
					
					init = 1;
					last = current;
				}
				else{
					*next = current;
					break;
				}
			}
			current = current->next; /* go to the next in the list */
		}
		if (init){
			if (pline_flag & 1){
				closed = 1;
				/* if closed, the last vertex is first */
				last_x = px;
				last_y = py;
				last_z = pz;
			}
			else {
				closed = 0;
				last = NULL;
			}
			
			/* to convert OCS to WCS */
			normal[0] = extru_x;
			normal[1] = extru_y;
			normal[2] = extru_z;			
			
			pt[0] = px;
			pt[1] = py;
			pt[2] = pz;
			*bulge = bul;
			ok = 1;
			
		}
	}
	else if ((init) && (*next != last)){ /* continue search in next point */
		dxf_node *curr_vtx = NULL;
		current = *next;
		if (*next) curr_vtx = current->obj.content;
		while(curr_vtx){
			if (curr_vtx->type == DXF_ATTR){ /* DXF attibute */
				switch (curr_vtx->value.group){
					case 10:
						px = curr_vtx->value.d_data;
						break;
					case 20:
						py = curr_vtx->value.d_data;
						break;
					case 30:
						pz = curr_vtx->value.d_data;
						break;
					case 42:
						bul = curr_vtx->value.d_data;
						break;
				}
			}
			curr_vtx = curr_vtx->next;
		}
		
		/* get next vertex */
		*next = NULL;
		if (current->next){
			if (current->next->type == DXF_ENT){
				if (strcmp(current->next->obj.name, "VERTEX") == 0 ){
					*next = current->next;
				}
			}
		}
		
		pt[0] = px;
		pt[1] = py;
		pt[2] = pz;
		*bulge = bul;
		ok = 1;
		if (*next == NULL){
			if (closed){
				*next = last;
			}
			else {
				*next = NULL;
				pline_flag = 0; closed =0; init = 0;
				last = NULL;
			}
		}
	}
	else { /* last vertex */
		pt[0] = last_x;
		pt[1] = last_y;
		pt[2] = last_z;
		*bulge = 0.0;
		ok = 1;
		*next = NULL;
		pline_flag = 0; closed =0; init = 0;
		last = NULL;
	}
	
	if (ok == 0){
		*next = NULL;
		pline_flag = 0; closed =0; init = 0;
		last = NULL;
	}
	/* convert OCS to WCS */
	else {
		mod_axis(pt, normal , 0.0);
		/* update return values */
		*pt1_x = pt[0];
		*pt1_y = pt[1];
		*pt1_z = pt[2];
	}
	
	return ok;
}

list_node *  gui_dwg_sel_filter(dxf_drawing *drawing, enum dxf_graph filter, int pool_idx){
	/* scan the drawing and return a list of entities according selection filter, only visible ents */
	dxf_node *current = NULL;
	list_node *list = NULL;
	
	/* verify structures */
	if (!drawing || (drawing->ents == NULL) || (drawing->main_struct == NULL)){
		/* fail */
		return NULL;
	}
	
	/* init the serch */
	current = drawing->ents->obj.content->next; /* or from begin */
	
	/* sweep entities section */
	while (current != NULL){
		if (current->type == DXF_ENT){ /* look for DXF entity */
			enum dxf_graph ent_type = dxf_ident_ent_type (current);
			
			/*verify if entity layer is on and thaw */
			if ((!drawing->layers[current->obj.layer].off) && 
				(!drawing->layers[current->obj.layer].frozen) ){
				/* and if  is a compatible entity */
				if (ent_type & filter){
					if (!list) list = list_new(NULL, pool_idx);
					/* append to list*/
					list_node * new_node = list_new(current, pool_idx);
					list_push(list, new_node);
				}
				else if (ent_type == DXF_INSERT && (filter & DXF_ATTRIB) ){
					int num_attr = 0;
					dxf_node *attr = NULL;
					
					num_attr = 0;
					while (attr = dxf_find_obj_i(current, "ATTRIB", num_attr)){
						/* add attribute entity to list */
						if (!list) list = list_new(NULL, pool_idx);
						/* append to list*/
						list_node * new_node = list_new(attr, pool_idx);
						list_push(list, new_node);
						
						num_attr++;
					}
				}
				
			}
		}
		current = current->next;
	}
	
	return list;
}

int dxf_layer_get(dxf_drawing *drawing, dxf_node * ent){
	/* Return the layer index of drawing's layer vector */
	/*if search fails, return -1 */
	
	/* verify structures */
	if (!drawing || (drawing->ents == NULL) || (drawing->main_struct == NULL)) 
		return -1;
	if (!ent) return -1;
	
	int ok = -1;
	char layer[DXF_MAX_CHARS + 1] = "0"; /* default layer is "0" */
	dxf_node *tmp = NULL;
	/* try to find entity's layer information */
	if(tmp = dxf_find_attr2(ent, 8))
		strncpy (layer, tmp->value.s_data, DXF_MAX_CHARS);
	/* try to find layer index */
	if (strlen(layer) > 0) ok = dxf_lay_idx (drawing, layer);
	else ok = dxf_lay_idx (drawing, "0");
	
	return ok;
}

int dxf_ltype_get(dxf_drawing *drawing, dxf_node * ent){
	/* Return the line type index of drawing's ltype vector */
	/*if search fails, return -1 */
	
	/* verify structures */
	if (!drawing || (drawing->ents == NULL) || (drawing->main_struct == NULL)) 
		return -1;
	if (!ent) return -1;
	
	int ok = -1;
	char ltyp[DXF_MAX_CHARS + 1] = "ByLayer"; /* default line type is "ByLayer" */
	dxf_node *tmp = NULL;
	/* try to find entity's line type information */
	if(tmp = dxf_find_attr2(ent, 6))
		strncpy (ltyp, tmp->value.s_data, DXF_MAX_CHARS);
	/* try to find line type index */
	if (strlen(ltyp) > 0) ok = dxf_ltype_idx (drawing, ltyp);
	else ok = dxf_ltype_idx  (drawing, "ByLayer");
	
	return ok;
}

int dxf_tstyle_get(dxf_drawing *drawing, dxf_node * ent){
	/* Return the text style index of drawing's vector */
	/*if search fails, return -1 */
	
	/* verify structures */
	if (!drawing || (drawing->ents == NULL) || (drawing->main_struct == NULL)) 
		return -1;
	if (!ent) return -1;
	
	int ok = -1;
	char tsty[DXF_MAX_CHARS + 1] = "Standard"; /* default style is "Standard" */
	dxf_node *tmp = NULL;
	/* try to find entity's style information */
	if(tmp = dxf_find_attr2(ent, 7))
		strncpy (tsty, tmp->value.s_data, DXF_MAX_CHARS);
	/* try to find style index */
	if (strlen(tsty) > 0) ok = dxf_tstyle_idx (drawing, tsty);
	else ok = dxf_tstyle_idx (drawing, "Standard");
	
	return ok;
}