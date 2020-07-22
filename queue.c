#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "harness.h"
#include "queue.h"

/*
 * Create a linked list element.
 * Return NULL if fails.
 */
list_ele_t *__create_list_ele(char *s)
{
    if (!s)
        return NULL;

    list_ele_t *newele;
    char *newvalue;
    size_t l;

    newele = malloc(sizeof(list_ele_t));
    l = strlen(s);
    newvalue = malloc((l + 1) * sizeof(char));
    if (!newele || !newvalue) {
        free(newele);
        free(newvalue);
        return NULL;
    }

    newele->value = newvalue;
    newele->next = NULL;
    strncpy(newvalue, s, l);
    newvalue[l] = '\0';
    return newele;
}

/*
 * Create empty queue.
 * Return NULL if could not allocate space.
 */
queue_t *q_new()
{
    queue_t *q = malloc(sizeof(queue_t));
    if (!q)
        return NULL;
    q->head = NULL;
    q->tail = NULL;
    q->size = 0;
    return q;
}

/* Free all storage used by queue */
void q_free(queue_t *q)
{
    if (!q)
        return;
    /* Free the list elements and the strings */
    list_ele_t *next = q->head;
    while (next) {
        list_ele_t *ele = next;
        next = ele->next;
        free(ele->value);
        free(ele);
    }
    /* Free queue structure */
    q->head = q->tail = NULL;
    free(q);
}

/*
 * Attempt to insert element at head of queue.
 * Return true if successful.
 * Return false if q is NULL or could not allocate space.
 * Argument s points to the string to be stored.
 * The function must explicitly allocate space and copy the string into it.
 */
bool q_insert_head(queue_t *q, char *s)
{
    if (!q)
        return false;

    list_ele_t *newh;
    newh = __create_list_ele(s);
    if (!newh)
        return false;

    if (q->size == 0) {
        q->head = q->tail = newh;
    } else {
        newh->next = q->head;
        q->head = newh;
    }
    q->size += 1;
    return true;
}

/*
 * Attempt to insert element at tail of queue.
 * Return true if successful.
 * Return false if q is NULL or could not allocate space.
 * Argument s points to the string to be stored.
 * The function must explicitly allocate space and copy the string into it.
 */
bool q_insert_tail(queue_t *q, char *s)
{
    if (!q)
        return false;

    list_ele_t *newt;
    newt = __create_list_ele(s);
    if (!newt)
        return false;

    if (q->size == 0) {
        q->head = q->tail = newt;
    } else {
        q->tail->next = newt;
        q->tail = newt;
    }
    q->size += 1;
    return true;
}

/*
 * Attempt to remove element from head of queue.
 * Return true if successful.
 * Return false if queue is NULL or empty.
 * If sp is non-NULL and an element is removed, copy the removed string to *sp
 * (up to a maximum of bufsize-1 characters, plus a null terminator.)
 * The space used by the list element and the string should be freed.
 */
bool q_remove_head(queue_t *q, char *sp, size_t bufsize)
{
    if (!q || !q->head || !sp)
        return false;

    strncpy(sp, q->head->value, bufsize - 1);
    sp[bufsize - 1] = '\0';
    list_ele_t *oldh = q->head;
    q->head = q->head->next;
    free(oldh->value);
    free(oldh);
    q->size -= 1;
    return true;
}

/*
 * Return number of elements in queue.
 * Return 0 if q is NULL or empty
 */
int q_size(queue_t *q)
{
    return q ? q->size : 0;
}

/*
 * Reverse elements in queue
 * No effect if q is NULL or empty
 * This function should not allocate or free any list elements
 * (e.g., by calling q_insert_head, q_insert_tail, or q_remove_head).
 * It should rearrange the existing ones.
 */
void q_reverse(queue_t *q)
{
    if (!q || !q->head)
        return;

    list_ele_t *curr;
    list_ele_t *prev;
    list_ele_t *next;
    curr = q->head->next;
    prev = q->head;
    q->head->next = NULL;
    while (curr) {
        next = curr->next;
        curr->next = prev;
        prev = curr;
        curr = next;
    }
    q->tail = q->head;
    q->head = prev;
}

/*
 * Merge two sorted lists into one sorted list.
 * Return the head of the new list.
 */
list_ele_t *merge(list_ele_t *l1, list_ele_t *l2)
{
    list_ele_t *head;
    list_ele_t *tail;

    /* Determine the head. */
    if (strcmp(l1->value, l2->value) < 0) {
        /* l1->value is smaller. */
        head = tail = l1;
        l1 = l1->next;
    } else {
        head = tail = l2;
        l2 = l2->next;
    }

    /* Connect elements after the head. */
    while (l1 && l2) {
        if (strcmp(l1->value, l2->value) < 0) {
            /* l1->value is smaller. */
            tail->next = l1;
            tail = l1;
            l1 = l1->next;
        } else {
            tail->next = l2;
            tail = l2;
            l2 = l2->next;
        }
    }

    /* Connect the remaining list. */
    if (l1)
        tail->next = l1;
    else if (l2)
        tail->next = l2;

    return head;
}

/*
 * Split l0 into l1 (the preceding half) and l2 (the succeeding half)
 * with no elements being copied.
 */
void split(list_ele_t *l0, list_ele_t **l1, list_ele_t **l2)
{
    list_ele_t *fast = l0->next;
    list_ele_t *slow = l0;

    while (fast && fast->next) {
        slow = slow->next;
        fast = fast->next->next;
    }

    fast = slow->next;
    slow->next = NULL;

    *l1 = l0;
    *l2 = fast;
}

/*
 * Sort elements of a list in ascending order.
 * Return the head of the sorted list.
 */
list_ele_t *merge_sort(list_ele_t *head)
{
    if (!head || !head->next)
        return head;

    list_ele_t *l1;
    list_ele_t *l2;

    split(head, &l1, &l2);

    l1 = merge_sort(l1);
    l2 = merge_sort(l2);

    return merge(l1, l2);
}

/*
 * Sort elements of queue in ascending order
 * No effect if q is NULL or empty. In addition, if q has only one
 * element, do nothing.
 */
void q_sort(queue_t *q)
{
    if (!q || !q->head || !q->head->next)
        return;

    q->head = merge_sort(q->head);
    q->tail = q->head;
    while (q->tail->next) {
        q->tail = q->tail->next;
    }
}
