#pragma once
#include "utils/memory_utils.h"

/**
 * @struct LinkedList
 * @brief Basic single linked list.
 * @author Simon Scatton
 * @date 01/03/2020
 * @version 1.0.1
 */
typedef struct LinkedList
{
    void* data;
    struct LinkedList *previous;
    struct LinkedList *next;
} LinkedList;


/**
 * Allocates memory for a new sentinel and sets the data
 * to NULL. Also sets the next to NULL. If you want to add
 * an element to the list, use push_back on the list instead.
 * Do not create new elements of the list using this function.
 * @author Simon Scatton
 * @date 21/01/2020
 * @return A new LinkedList* sentinel.
 * @version 1.0.0
 */

LinkedList* list_create();


/**
 * Allocates memory for a new element of the list and
 * pushes the data passed as parameter in this new element
 * that is then placed at the end of the list.
 * @param list The list you wish to push_back to.
 * @param newData The data to be pushed back.
 * @author Simon Scatton
 * @date 01/03/2020
 * @return true(1) if the push_back was successful false(0) otherwise
 * @version 1.0.1
 */
int list_push_back(LinkedList *list, void *data);

/**
 * Removes the last element of the linked list and returns it.
 * It also sets the new end pointer of the list to NULL.
 * If the list passed as argument is NULL or a sentinel, then
 * the pop operation will fail and the pop function will return
 * NULL. You will need to free yourself the list and its content
 * after a call to pop using the list_free function. The next and
 * previous values of the list will be set to NULL. If you wish to
 * get a pointer to the end of the list see list_get instead.
 * @author Simon Scatton
 * @date 21/01/2020
 * @return A pointer to the end of the list if the list is not null
 * @return or a sentinel, NULL otherwise
 * @version 1.0.0
 */
LinkedList* list_pop(LinkedList *list);

/**
 * Gets the element at position in the list. If the position is
 * longer than the list or a negative number different of 1 it will
 * return NULL. If you wish to get the last element of the list pass
 * -1 as position. This functions does not modify the list.
 * @returns A pointer to the list in position position
 * @author Simon Scatton
 * @date 01/03/2021
 * @version 1.0.0
 **/
LinkedList* list_get(LinkedList *list, int position);


/**
 * Frees the list and the data pointers that are contained in
 * the data field. This function can be used to free an element
 * or the entire list. If used on a list with more than 1 element
 * it will free the entire list, if used at a specific pointer in
 * the list it will free the list from the pointer passed as
 * parameter to the end of the list.
 * @return 1 if the free is succesful 0 otherwise
 * @author Simon Scatton
 * @version 1.0.0
 * @date 01/03/2021
 * */

int list_free(LinkedList *list);
