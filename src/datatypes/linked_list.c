#include <stdlib.h>

#include "datatypes/linked_list.h"

LinkedList* create_list()
{
    LinkedList *list = SAFEMALLOC(sizeof(LinkedList));
    list->data = SENTINEL_DATA;
    list->next = NULL;
}

void push_back(LinkedList* list, int newData)
{
    
}