/** \file
    File to implement a double linked list.
*/

#ifndef _LIST_H_
#define _LIST_H_

#include <stddef.h>

/// List data structure used to link an node to a list.
typedef struct _list_iterator{
    struct _list_iterator *prev; ///< To previous node.
    struct _list_iterator *next; ///< To next node.
} list_iterator;

/// Go through all the nodes in a list.
#define list_for_each(iterator, head) \
    for (iterator = (head)->next; iterator != (head); \
        iterator = iterator->next)

/// Get the node by the link item inside the node.
#define list_entry(iterator, type, member) \
    ((type *)((char *)(list_iterator*)iterator - offsetof(type, member)))

/// Init a list.
/// \param[in] head : List to be initialized.
static void list_init(list_iterator *head)
{
    head->prev = head;
    head->next = head;
}

/// Insert a new node into the head of the list.
/// \param[in] head : Head of the list.
/// \param[in] entry : The new node to be inserted.
static void list_add(list_iterator *head, list_iterator *entry)
{
    head->next->prev = entry;
    entry->next = head->next;
    entry->prev = head;
    head->next = entry;
}

/// Remove a node from the list.
/// \param[in] entry : The node to be removed.
static void list_del(list_iterator *entry)
{
    entry->prev->next = entry->next;
    entry->next->prev = entry->prev;
    entry->next = NULL;
    entry->prev = NULL;
}

/// Determinate whether the list is empty.
/// \param[in] head : Head of the list.
/// \retval true : The list is empty.
/// \retval false : The list is not empty.
static int list_is_empty(list_iterator *head)
{
    return (head->next == head);
}

/// Merge list2 to list1.
/// \param[in] head1 : Head of list one;
/// \param[in] head2 : Head of list two;
static void list_merge(list_iterator *head1, list_iterator *head2)
{
    if (list_is_empty(head2)) return;
    head1->next->prev = head2->prev;
    head2->prev->next = head1->next;
    head1->next = head2->next;
    head2->next->prev = head1;
    head2->next = NULL;
    head2->prev = NULL;
    return;
}

#endif
