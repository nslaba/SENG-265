/*
 * linkedlist.c
 *
 * Based on the implementation approach described in "The Practice 
 * of Programming" by Kernighan and Pike (Addison-Wesley, 1999).
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ics.h"
#include "emalloc.h"
#include "listy.h"

///////////////implementation of mergesort//////////////////////
void merge_sort(node_t** headRef)
{
    node_t *head = *headRef;
    node_t *a, *b;

    //best case: length is 0 or 1
    if ((head == NULL) || (head->next == NULL)) {
        return;
    }
    //split head into a & b sublists
    front_back_split(head, &a, &b);

    //recursively sort the sublists
    merge_sort(&a);
    merge_sort(&b);

    //merge the two lists together
    *headRef = sorted_merge(a,b);

}


void front_back_split(node_t *source, node_t **front_ref, node_t **back_ref)
{
    node_t *fast, *slow;
    slow = source;
    fast = source->next;

    // slow will be positioned before mid point
    while (fast != NULL) {
        fast = fast->next;
        if (fast != NULL) {
            slow = slow->next;
            fast = fast->next;
        }
    }

    *front_ref = source;
    *back_ref = slow->next;
    slow->next = NULL;

}

node_t *sorted_merge(node_t *a, node_t *b) {
    node_t *final = NULL;

    //best cases:
    if (a==NULL) return b;
    else if (b==NULL) return a;

    if (difftime(a->val.dtstart, b->val.dtstart)>=0) {
        final = b;
        final-> next = sorted_merge(a, b->next);
    } else {
        final = a;
        final->next = sorted_merge(a->next, b);
    }
    return final;
}
////////////////////////////////////////////////////////////////
node_t* get_tail(node_t *head) {
    node_t *curr = NULL;
    if (head != NULL) {
        for (curr = head; curr->next !=NULL; curr = curr->next) {
            continue;
        }
    }
    return curr;
}


void deallocate_mem_line_list(node_line *head) {
    node_line *curr = NULL;
    while (head != NULL)
    {   
        curr = head;
        head = head->next; 
        free(curr);
    }
}

node_line *new_node1(char* fline) {
    //assert( val != NULL);

    node_line *temp = (node_line *)emalloc(sizeof(node_line));

    strcpy(temp->fline, fline);
    temp->next = NULL;
    
    return temp;
}

node_t *new_node(event_t val) {
    //assert( val != NULL);

    node_t *temp = (node_t *)emalloc(sizeof(node_t));

    temp->val = val;
    temp->next = NULL;

    return temp;
}

node_line *add_front1(node_line *list, node_line *new) {
    new->next = list;
    return new;
}


node_t *add_front(node_t *list, node_t *new) {
    new->next = list;
    return new;
}

node_line *add_end1(node_line *list, node_line *new) {
    node_line *curr;

    if (list == NULL) {
        new->next = NULL;
        //list = new;
        return new;
    }
    for (curr = list; curr->next != NULL; curr = curr->next);
    curr->next = new;
    new->next = NULL;
    return list;
}


node_t *add_end(node_t *list, node_t *new) {
    node_t *curr;

    if (list == NULL) {
        new->next = NULL;
        return new;
    }

    for (curr = list; curr->next != NULL; curr = curr->next);
    curr->next = new;
    new->next = NULL;
    return list;
}

node_line *peek_front1(node_line *list) {
    return list;
}

node_t *peek_front(node_t *list) {
    return list;
}
node_line *remove_front1(node_line *list) {
    if (list == NULL) {
        return NULL;
    }
    return list->next;
}

node_t *remove_front(node_t *list) {
    if (list == NULL) {
        return NULL;
    }

    return list->next;
}

void apply1(node_line *list,
            void (*fn)(node_line *list, void *),
            void *arg)
{
    for ( ; list != NULL; list = list->next) {
        (*fn)(list, arg);
    }
}

void apply(node_t *list,
           void (*fn)(node_t *list, void *),
           void *arg)
{
    for ( ; list != NULL; list = list->next) {
        (*fn)(list, arg);
    }
}
