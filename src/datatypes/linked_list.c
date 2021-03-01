#include <stdlib.h>

#include "datatypes/linked_list.h"

LinkedList* list_create()
{
    LinkedList *list = SAFEMALLOC(sizeof(LinkedList));
    list->data = NULL;
    list->next = NULL;
    list->previous = NULL;
    return list;
}

int list_push_back(LinkedList *list, void *data)
{
    if(list == NULL)
        return 0;
    while (list->next != NULL)
        list = list->next;
    LinkedList *newList = SAFEMALLOC(sizeof(LinkedList));
    newList->data = data;
    newList->next = NULL;
    newList->previous = list;
    list->next = newList;
    return 1;
}

LinkedList* list_pop(LinkedList *list)
{
    if(list == NULL || list->next == NULL)
        return NULL;
    while(list->next != NULL && list->next->next != NULL)
        list = list->next;
    LinkedList *last = list->next;
    last->previous = NULL;
    list->next = NULL;
    return last;
}
