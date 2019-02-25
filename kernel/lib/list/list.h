#ifndef __LIST_H
#define __LIST_H

#include"stdint.h"

#define offset(struct_type,member) (int)(&(((struct_type *) 0)->member))

#define elem2entry(struct_type,struct_member_name,elem_ptr) (struct_type *)((int)elem_ptr - offset(struct_type,struct_member_name))

struct _node {
    struct _node * prev;
    struct _node * next;
};

struct _list {
    struct _node head;
    struct _node tail;
};

void _list_init(struct _list *);

void _list_add_front(struct _list *,struct _node *);

void _list_add_back(struct _list *,struct _node *);

struct _node * _list_pop_front(struct _list *);

struct _node * _list_pop_back(struct _list *);

struct _node * _list_front(struct _list *);

struct _node * _list_back(struct _list *);

uint8_t _list_empty(struct _list *);

uint8_t _list_find(struct _list *,struct _node *);

#endif

