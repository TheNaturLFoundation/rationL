#include "datatypes/linked_list.h"

#include <stdlib.h>

#include "utils/memory_utils.h"

LinkedList *list_create()
{
    LinkedList *list = SAFEMALLOC(sizeof(LinkedList));
    list->data = NULL;
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
    newList->data = data;
    newList->next = NULL;
    newList->previous = list;
    list->next = newList;
    return 1;
}

LinkedList *list_pop_at(LinkedList *list, int position)
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
    for (size_t curr_pos = 0; curr->next != NULL && curr_pos < position;
         curr = curr->next, curr_pos++)
        ;
    curr->previous->next = curr->next;
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

LinkedList *list_get(LinkedList *list, int position)
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
    while (list != NULL)
    {
        LinkedList *next = list->next;
        free(list->data);
        free(list);
        list = next;
    }
    return 1;
}
