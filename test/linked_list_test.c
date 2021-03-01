#include "datatypes/linked_list.h"
#include <criterion/criterion.h>
#include <criterion/internal/assert.h>

Test(linked_list, create_list)
{
    LinkedList* list = create_linked_list();
    cr_assert_null(list->data);
    free(list);
}

Test(linked_list, push_back)
{
    LinkedList* list = create_linked_list();
    int *a = malloc(sizeof(int));
    push_back(list, a);
    int *data = (list->next->data);
    cr_assert_eq(*data, *a);
    free(data);
}
