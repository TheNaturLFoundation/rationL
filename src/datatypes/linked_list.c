#include <stdlib.h>

#include "datatypes/linked_list.h"

/**
 * @brief Creates a linked list sentinel and allocates memory.
 * Allocates memory for a new sentinel and sets the data
 * to SENTINAL_DATA defined in linked_list.h. Also sets
 * the next to NULL. If you want to add an element to 
 * the list, use push_back on the list instead. Do not create
 * new elements of the list using this function.
 * @author Simon Scatton
 * @date 21/01/2020
 * @return A new LinkedList* sentinel.
 * @version 1.0.0 
 */
LinkedList* create_list()
{
    LinkedList *list = SAFEMALLOC(sizeof(LinkedList));
    list->data = SENTINEL_DATA;
    list->next = NULL;
    return list;
}

/**
 * @brief Creates a new list with newData as data and pushes it to
 * @brief the end of the list.
 * This functions allocates memory for a new element of the list and
 * pushes the data passed as parameter in this new element that is
 * then placed at the end of the list.
 * @author Simon Scatton
 * @date 21/01/2020
 * @return true(1) if the push_back was successful false(0) otherwise
 * @version 1.0.0
 */
int push_back(LinkedList *list, int newData)
{
    if(list == NULL)
        return 0;
    while (list->next != NULL)
        list = list->next;
    LinkedList *newList = SAFEMALLOC(sizeof(LinkedList));
    newList->data = newData;
    newList->next = NULL;
    list->next = newList;
    return 1;
}

/**
 * @brief Removes the last element of the linked list and returns it.
 * This functions removes the last element of the linked list and 
 * returns it, it also sets the new end pointer
 * of the list to NULL. If the list passed as argument is NULL
 * or a sentinel, then the pop operation will fail and the pop
 * function will return NULL. You will need to free yourself the
 * struct after a call to pop
 * @author Simon Scatton
 * @date 21/01/2020
 * @return A pointer to the end of the list if the list is not null
 * @return or a sentinel, NULL otherwise
 * @version 1.0.0
 */
LinkedList* pop(LinkedList *list)
{
    if(list == NULL || list->next == NULL)
        return 0;
    while(list->next != NULL && list->next->next != NULL)
        list = list->next;
    LinkedList *last = list->next;
    list->next = NULL;
    return last;
}