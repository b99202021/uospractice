// the list operation should be atomic ????
// Try for protecting the list operations

#include "list.h"
#include "int.h"

void _list_init(struct _list * list_ptr){
    int intr = intr_status();
    list_ptr->head.prev = 0;
    list_ptr->head.next = &(list_ptr->tail);
    list_ptr->tail.prev = &(list_ptr->head);
    list_ptr->tail.next = 0;
    if(intr) intopen();
}

void _list_add_front(struct _list * list_ptr,struct _node * node_ptr){
    int intr = intr_status();
    node_ptr->prev = &(list_ptr->head);
    node_ptr->next = list_ptr->head.next;
    list_ptr->head.next->prev = node_ptr;
    list_ptr->head.next = node_ptr;
    if(intr) intopen();
}

void _list_add_back(struct _list * list_ptr,struct _node * node_ptr){
    int intr = intr_status();
    node_ptr->next = &(list_ptr->tail);
    node_ptr->prev = list_ptr->tail.prev;
    list_ptr->tail.prev->next = node_ptr;
    list_ptr->tail.prev = node_ptr;
    if(intr) intopen();
}

struct _node * _list_pop_front(struct _list * list_ptr){
    int intr = intr_status();
    if(_list_empty(list_ptr)) return 0;
    struct _node * pop_ptr = list_ptr->head.next;
    list_ptr->head.next = list_ptr->head.next->next;
    list_ptr->head.next->prev = &(list_ptr->head);
    if(intr) intopen();
    return pop_ptr;
}

struct _node * _list_front(struct _list* list_ptr){
    if(_list_empty(list_ptr)) return 0;
    return list_ptr->head.next;
}

struct _node * _list_pop_back(struct _list * list_ptr){
    int intr = intr_status();
    if(_list_empty(list_ptr)) return 0;
    struct _node * pop_ptr = list_ptr->tail.prev;
    list_ptr->tail.prev = list_ptr->tail.prev->prev;
    list_ptr->tail.prev->next = &(list_ptr->tail);
    if(intr) intopen();
    return pop_ptr;
}

struct _node * _list_back(struct _list * list_ptr){
    if(_list_empty(list_ptr)) return 0;
    return list_ptr->tail.prev;
}

uint8_t _list_empty(struct _list * list_ptr){
    if(list_ptr->head.next == &(list_ptr->tail)) return 1;
    else return 0;
}

uint8_t _list_find(struct _list * list_ptr,struct _node * node_ptr){
    int intr = intr_status();
    struct _node * nptr = list_ptr->head.next;
    while(nptr++ != &(list_ptr->tail)){
        if(nptr == node_ptr) return 1;
    }
    if(intr) intopen();
    return 0;
}


