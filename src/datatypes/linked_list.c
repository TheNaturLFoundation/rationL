#include <stdlib.h>
#include <string.h>
#include "datatypes/linked_list.h"
#include "utils/memory_utils.h"


LinkedList *list_create(size_t size)
{
    LinkedList *list = SAFEMALLOC(sizeof(LinkedList));
    list->data = NULL;
    list->size = size;
    list->next = NULL;
    list->previous = NULL;
    return list;
}

int list_push_back(LinkedList *list, void *data)
{
    if (list == NULL)
        return 0;
    while (list->next != NULL)
        list = list->next;
    LinkedList *newList = SAFEMALLOC(sizeof(LinkedList));
    newList->data = SAFEMALLOC(list->size);
    memcpy(newList->data, data, list->size);
    newList->next = NULL;
    newList->previous = list;
    newList->size = list->size;
    list->next = newList;
    return 1;
}

int list_push_front(LinkedList *list, void *data)
{
    if (list == NULL)
        return 0;
    LinkedList *newList = SAFEMALLOC(sizeof(LinkedList));
    newList->data = SAFEMALLOC(list->size);
    memcpy(newList->data, data, list->size);
    newList->next = list->next;
    if (list->next)
        list->next->previous = newList;
    newList->previous = list;
    newList->size = list->size;
    list->next = newList;
    return 1;
}

LinkedList *list_pop_at(LinkedList *list, ssize_t position)
{
    if (position < -1 || list == NULL)
        return NULL;
    LinkedList *curr = list;
    if (position == -1)
    {
        for (; curr->next != NULL; curr = curr->next)
            ;
        curr->next = NULL;
        curr->previous->next = NULL;
        curr->previous = NULL;
        return curr;
    }
    curr = curr->next;
    for (ssize_t curr_pos = 0; curr->next != NULL && curr_pos < position;
         curr = curr->next, curr_pos++)
        ;
    curr->previous->next = curr->next;
    if(curr->next)
        curr->next->previous = curr->previous;
    curr->next = NULL;
    curr->previous = NULL;
    return curr;
}

LinkedList *list_pop(LinkedList *list)
{
    return list_pop_at(list, -1);
}

LinkedList *list_pop_front(LinkedList *list)
{
    return list_pop_at(list, 0);
}

LinkedList *list_get(LinkedList *list, ssize_t position)
{
    if (position < -1)
        return NULL;
    if (position == -1)
    {
        while (list->next != NULL)
            list = list->next;
        return list;
    }
    list = list->next;
    while (list != NULL && position != 0)
    {
        list = list->next;
        position -= 1;
    }
    return list;
}

int list_free(LinkedList *list)
{
    if (list == NULL)
        return 0;
    LinkedList *start = list;
    list = list->next;
    free(start);
    while (list != NULL)
    {
        LinkedList *next = list->next;
        free(list->data);
        free(list);
        list = next;
    }
    return 1;
}

int list_free_from(LinkedList *list)
{
    if (list == NULL)
        return 0;
    while (list != NULL)
    {
        LinkedList *next = list->next;
        free(list->data);
        free(list);
        list = next;
    }
    return 1;
}

void *list_pop_value(LinkedList* list)
{
    LinkedList *popped = list_pop(list);
    void *data = popped->data;
    free(popped);
    return data;
}

void *list_pop_front_value(LinkedList* list)
{
    LinkedList *popped = list_pop_front(list);
    void *data = popped->data;
    free(popped);
    return data;
}

void *list_pop_at_value(LinkedList* list, ssize_t position)
{
    LinkedList *popped = list_pop_at(list, position);
    void *data = popped->data;
    free(popped);
    return data;
}

void *list_get_value(LinkedList* list, ssize_t position)
{
    LinkedList *l = list_get(list, position);
    return l->data;
}

int list_empty(LinkedList* list)
{
    return list == NULL || list->next == NULL;
}
