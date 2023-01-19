#ifndef _LINKEDLIST_H_
#define _LINKEDLIST_H_

#include "ics.h"

typedef struct node_t {
    event_t        val;
    struct node_t  *next;
} node_t;

typedef struct node_line {
    char fline[MAX_LINE_LEN];
    struct node_line    *next;
} node_line;

void merge_sort(node_t** headRef);
node_t *sorted_merge(node_t*, node_t*);
void front_back_split(node_t*, node_t**, node_t**);

node_t* get_tail(node_t *);

void deallocate_mem_line_list(node_line *);

node_line *new_node1(char* fline);
node_t *new_node(event_t val);

node_line *add_front1(node_line *, node_line *);
node_t *add_front(node_t *, node_t *);

node_line *add_end1(node_line *, node_line *);
node_t *add_end(node_t *, node_t *);

node_line *peek_front1(node_line *);
node_t *peek_front(node_t *);

node_line *remove_front1(node_line *);
node_t *remove_front(node_t *);

void    apply1(node_line *, void(*fn)(node_line *, void *), void *arg);
void    apply(node_t *, void(*fn)(node_t *, void *), void *arg);

#endif
