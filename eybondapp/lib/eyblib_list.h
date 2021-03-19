/**************Copyright(C) 2015-2026 Shenzhen Eybond Co., Ltd.***************
  *@brief   : eyblib_list.h
  *@notes   : 2020.08.04 MWY
*******************************************************************************/
#ifndef __EYBLIB_LIST_H_
#define	__EYBLIB_LIST_H_

#include "eyblib_typedef.h"

typedef struct node_t
{
	struct node_t *front;
	struct node_t *next;
	u8_t   payload[];
}Node_t;

typedef struct
{
    s16_t   count;  
    u16_t   rsv;
	Node_t	*node;
}ListHandler_t;

typedef u8_t (*cmpFun)(void *src, void *dest);	//
typedef u8_t (*processorFun)(void *listPaycoad, void *chagePoint);			//

void    list_init(ListHandler_t *head);
int 	list_topInsert(ListHandler_t *head, void *payload);
int 	list_bottomInsert(ListHandler_t *head, void *payload);
int 	list_ConfInsert(ListHandler_t *head, cmpFun cmp, void *payload);
void 	*list_nodeApply(mcu_t payloadSize);
void    *list_nextData(ListHandler_t *head, void *payload); // get next node data
void 	*list_find(ListHandler_t *head, cmpFun cmp, void *conVal);

void    list_trans(ListHandler_t *head, processorFun fun, void *optPoint);
void 	list_nodeDelete(ListHandler_t *head, void *payload);
void	list_delete(ListHandler_t *head);

#endif//__EYBLIB_LIST_H_
/******************************************************************************/

