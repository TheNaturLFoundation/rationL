#include "datatypes/linked_list.h"
#include <criterion/criterion.h>
#include <stdio.h>

Test(linked_list, create_list)
{
    LinkedList* list = LinkedList(int);
    cr_assert_null(list->data);
    cr_assert_null(list->next);
    cr_assert_null(list->previous);
    list_free(list);
}

Test(linked_list, free_on_null)
{
    LinkedList* list = LinkedList(int);
    list_free(list);
    list = NULL;
    list_free(list);
}

Test(linked_list, push_back_1)
{
    LinkedList* list = LinkedList(int);
    int a = 1;
    list_push_back(list, &a);
    int *data = (list->next->data);
    cr_assert_eq(*data, a);
    cr_assert_eq(list->next->previous, list);
    cr_assert_null(list->next->next);
    list_free(list);
}

Test(linked_list, push_front)
{
    LinkedList* list = LinkedList(int);
    int a = 1;
    int b = 2;
    list_push_front(list, &b);
    list_push_front(list, &a);

    int *data = (list->next->data);
    cr_assert_eq(*data, a);
    cr_assert_eq(list->next->previous, list);

    data = (list->next->next->data);
    cr_assert_eq(*data, b);
    cr_assert_eq(list->next->next->previous, list->next);
    cr_assert_null(list->next->next->next);

    list_free(list);
}

Test(linked_list, push_and_pop)
{
    LinkedList *list = LinkedList(int);
    int a = 1;
    list_push_back(list, &a);
    LinkedList *data_a = list_pop(list);
    cr_assert_eq(*(int*)(data_a->data), a);
    cr_assert_null(data_a->next);
    cr_assert_null(data_a->previous);
    cr_assert_null(list->next);
    list_free(list);
    list_free_from(data_a);
}


Test(linked_list, pop_at_first)
{
    LinkedList *list = LinkedList(int);
    int a = 1;
    list_push_back(list, &a);
    LinkedList *head = list_pop_front(list);
    cr_assert_eq(*(int *)head->data, 1);
    cr_assert_eq(head->next, NULL);
    cr_assert_eq(head->previous, NULL);
    list_free_from(head);
    list_push_back(list, &a);
    LinkedList *head2 = list_pop_front(list);
    cr_assert_eq(*(int *)head2->data, 1);
    list_free_from(head2);
    list_free(list);
}

Test(linked_list, pop_at_second)
{
    LinkedList *list = LinkedList(int);
    int a = 1;
    int b = 2;
    list_push_back(list, &a);
    list_push_back(list, &b);
    LinkedList *head = list_pop_at(list, 1);
    cr_assert_eq(*(int *)head->data, 2);
    cr_assert_eq(*(int *)list->next->data, 1);
    list_free(list);
    list_free_from(head);
 }

Test(linked_list, list_pop_get_neg_index)
{
    LinkedList *list = LinkedList(int);
    int a = 2;
    list_push_back(list, &a);
    LinkedList *val = list_pop_at(list, -5);
    cr_assert_null(val);
    val = list_get(list, -2);
    cr_assert_null(val);
    list_free(list);
}

Test(linked_list, push_push_pop)
{
    LinkedList *list = LinkedList(int);
    int a = 1;
    int b = 2;
    list_push_back(list, &a);
    list_push_back(list, &b);
    cr_assert_eq(*(int*)list->next->data, a);
    cr_assert_eq(*(int*)list->next->next->data, b);
    cr_assert_eq(list->next->next->previous, list->next);
    LinkedList *blist = list_pop(list);
    cr_assert_null(list->next->next);
    list_free_from(blist);
    list_free(list);
}

Test(linked_list, get_0)
{
    LinkedList *list = LinkedList(int);
    int a = 1;
    list_push_back(list, &a);
    LinkedList *first = list_get(list, 0);
    cr_assert_eq(*(int*)(first->data), a);
    cr_assert_eq(list->next, first);
    list_free(list);
}

Test(linked_list, get_middle)
{
    LinkedList *list = NULL;
    int a = 1;
    int b = 2;
    int c = 3;
    cr_assert_eq(0, list_push_back(list, &a));
    list = LinkedList(int);
    cr_assert_eq(list_push_back(list, &a), 1);
    list_push_back(list, &b);
    list_push_back(list, &c);
    LinkedList *l = list_get(list, 1);
    cr_assert_eq(*(int*)l->data, 2);
    list_free(list);
}

Test(linked_list, get_last)
{
    LinkedList *list = LinkedList(int);
    int a = 1;
    list_push_back(list, &a);
    int b = 2;
    list_push_back(list, &b);
    LinkedList *last = list_get(list, -1);
    cr_assert_eq(*(int*)(last->data), b);
    list_free(list);
}

Test(linked_list, list_pop_value)
{
    LinkedList *list = LinkedList(int);
    list_push_back(list, &(int){4});
    list_push_back(list, &(int){2});
    list_push_back(list, &(int){0});
    int* a = list_pop_value(list);
    cr_assert_eq(*a, 0);
    free(a);
    int* b = list_pop_value(list);
    cr_assert_eq(*b, 2);
    free(b);
    int* c = list_pop_value(list);
    cr_assert_eq(*c, 4);
    free(c);
    list_free(list);
}

Test(linked_list, list_pop_front_value)
{
    LinkedList *list = LinkedList(int);
    list_push_back(list, &(int){4});
    list_push_back(list, &(int){2});
    list_push_back(list, &(int){0});
    int* a = list_pop_front_value(list);
    cr_assert_eq(*a, 4);
    free(a);
    int* b = list_pop_front_value(list);
    cr_assert_eq(*b, 2);
    free(b);
    int* c = list_pop_front_value(list);
    cr_assert_eq(*c, 0);
    free(c);
    list_free(list);
}

Test(linked_list, list_pop_at_value)
{
    LinkedList *list = LinkedList(int);
    list_push_back(list, &(int){4});
    list_push_back(list, &(int){2});
    list_push_back(list, &(int){0});
    int* a = list_pop_at_value(list, 1);
    cr_assert_eq(*a, 2);
    free(a);
    int* b = list_pop_at_value(list, 1);
    cr_assert_eq(*b, 0);
    free(b);
    int* c = list_pop_at_value(list, 0);
    cr_assert_eq(*c, 4);
    free(c);
    list_free(list);
}

Test(linked_list, list_get_at_value)
{
    LinkedList *list = LinkedList(int);
    list_push_back(list, &(int){4});
    list_push_back(list, &(int){2});
    list_push_back(list, &(int){0});
    int* a = list_get_value(list, 1);
    cr_assert_eq(*a, 2);
    int* b = list_get_value(list, 1);
    cr_assert_eq(*b, 2);
    int* c = list_get_value(list, 0);
    cr_assert_eq(*c, 4);
    int* d = list_get_value(list, 2);
    cr_assert_eq(*d, 0);
    list_free(list);
}

Test(linked_list, foreach)
{
    LinkedList *list = LinkedList(int);
    list_push_back(list, &(int){4});
    list_push_back(list, &(int){2});
    list_push_back(list, &(int){0});
    size_t i = 0;
    list_foreach(int, value, list)
    {
        switch(i)
        {
            case 0:
                cr_assert_eq(value, 4);
                break;
            case 1:
                cr_assert_eq(value, 2);
                break;
            case 2:
                cr_assert_eq(value, 0);
                break;
        }
        i++;
    }
    list_free(list);
}
