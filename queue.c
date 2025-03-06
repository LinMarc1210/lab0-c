#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

/* Notice: sometimes, Cppcheck would find the potential NULL pointer bugs,
 * but some of them cannot occur. You can suppress them by adding the
 * following line.
 *   cppcheck-suppress nullPointer
 */

/* Create an empty queue */
struct list_head *q_new()
{
    struct list_head *head =
        (struct list_head *) malloc(sizeof(struct list_head));

    if (!head)
        return NULL;

    head->next = head;
    head->prev = head;
    return head;
}

/* Free all storage used by queue */
void q_free(struct list_head *head)
{
    if (!head)
        return;

    struct list_head *pos, *next;
    list_for_each_safe (pos, next, head) {
        element_t *entry = list_entry(pos, element_t, list);
        q_release_element(entry);
    }
    free(head);
}

/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    if (!head)
        return false;
    element_t *new = (element_t *) malloc(sizeof(element_t));
    if (!new)
        return false;
    new->value = strdup(s);
    if (!new->value) {
        free(new);
        return false;
    }
    list_add(&new->list, head);
    return true;
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (!head)
        return false;
    element_t *new = (element_t *) malloc(sizeof(element_t));
    if (!new)
        return false;
    new->value = strdup(s);
    if (!new->value) {
        free(new);
        return false;
    }
    list_add_tail(&new->list, head);
    return true;
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;
    element_t *entry = list_entry(head->next, element_t, list);
    if (sp) {
        sp = strncpy(sp, entry->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }
    list_del(head->next);

    return entry;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;
    element_t *entry = list_entry(head->prev, element_t, list);
    if (sp) {
        sp = strncpy(sp, entry->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }
    list_del(head->prev);

    return entry;
}

/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    if (!head || list_empty(head)) {
        return 0;
    }

    int size = 0;
    struct list_head *pos;
    list_for_each (pos, head) {
        size++;
    }
    return size;
}

/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    // https://leetcode.com/problems/delete-the-middle-node-of-a-linked-list/
    if (!head || list_empty(head))
        return false;
    struct list_head *pos;
    int index = 0;
    list_for_each (pos, head) {
        if (index == q_size(head) / 2) {
            break;
        }
        index++;
    }
    element_t *entry = list_entry(pos, element_t, list);
    list_del(pos);
    q_release_element(entry);

    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/
    if (!head || list_empty(head)) {
        return false;
    }
    if (list_is_singular(head)) {
        return true;
    }

    bool duplicate = false;

    struct list_head *pos, *next;
    list_for_each_safe (pos, next, head) {
        element_t *pos_entry = list_entry(pos, element_t, list);
        const element_t *next_entry =
            (next == head) ? NULL : list_entry(next, element_t, list);
        if (next_entry && strcmp(pos_entry->value, next_entry->value) == 0) {
            duplicate = true;
            list_del(pos);
            q_release_element(pos_entry);
        } else if (duplicate) {
            duplicate = false;
            list_del(pos);
            q_release_element(pos_entry);
        }
    }
    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    // https://leetcode.com/problems/swap-nodes-in-pairs/
    if (!head || list_empty(head) || list_is_singular(head)) {
        return;
    }

    struct list_head *front, *back, *safe;
    for (front = (head)->next, back = front->next, safe = back->next;
         front != (head) && back != (head);
         front = safe, back = front->next, safe = back->next) {
        struct list_head *front_prev = front->prev;
        struct list_head *back_next = back->next;

        front->next = back_next;
        front->prev = back;
        back->next = front;
        back->prev = front_prev;
        front_prev->next = back;
        back_next->prev = front;
    }
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    if (!head || list_empty(head)) {
        return;
    }

    struct list_head *pos = head;
    do {
        struct list_head *tmp = pos->next;
        pos->next = pos->prev;
        pos->prev = tmp;
        pos = tmp;
    } while (pos != head);
}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    // https://leetcode.com/problems/reverse-nodes-in-k-group/
    if (!head || list_empty(head) || list_is_singular(head)) {
        return;
    }

    int counter = 0;
    struct list_head *start = head;
    while (true) {
        struct list_head *end;
        for (end = start->next; end != head && counter < k;
             end = end->next, counter++)
            ;
        if (counter < k) {
            break;
        }
        struct list_head *pos = start->next;
        while (pos != end) {
            struct list_head *tmp = pos->next;
            if (pos->prev != start)
                pos->next = pos->prev;
            else
                pos->next = end;
            if (tmp != end)
                pos->prev = tmp;
            else
                pos->prev = start;
            pos = tmp;
        }
        struct list_head *end_prev = end->prev;
        struct list_head *start_next = start->next;
        end->prev = start_next;
        start->next = end_prev;

        start = end->prev;
        counter = 0;
    }
}

void merge_two(struct list_head *target, struct list_head *head, bool descend)
{
    if (!target || !head) {
        return;
    }

    LIST_HEAD(pos);

    while (!list_empty(head) && !list_empty(target)) {
        element_t *l_entry = list_first_entry(target, element_t, list);
        element_t *r_entry = list_first_entry(head, element_t, list);
        if ((!descend && strcmp(l_entry->value, r_entry->value) <= 0) ||
            (descend && strcmp(l_entry->value, r_entry->value) >= 0)) {
            list_move_tail(&l_entry->list, &pos);
        } else {
            list_move_tail(&r_entry->list, &pos);
        }
    }

    if (!list_empty(target))
        list_splice_tail_init(target, &pos);
    if (!list_empty(head))
        list_splice_tail_init(head, &pos);

    list_splice(&pos, head);
    return;
}

/* Sort elements of queue in ascending/descending order */
void q_sort(struct list_head *head, bool descend)
{
    if (!head || list_empty(head) || list_is_singular(head)) {
        return;
    }

    int count = q_size(head) / 2;
    struct list_head *slow = head, *fast = head->next;
    for (; count; slow = slow->next, fast = fast->next->next, count--)
        ;

    LIST_HEAD(left);
    list_cut_position(&left, head, slow);

    q_sort(&left, descend);
    q_sort(head, descend);
    merge_two(&left, head, descend);
}


/* Remove every node which has a node with a strictly less value anywhere to
 * the right side of it */
int q_ascend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    if (!head || list_empty(head) || list_is_singular(head))
        return q_size(head);

    struct list_head *pos, *pos_safe;
    list_for_each_safe (pos, pos_safe, head) {
        struct list_head *right = pos->next;
        element_t *pos_entry = list_entry(pos, element_t, list);

        while (right != head) {
            const element_t *right_entry = list_entry(right, element_t, list);
            if (strcmp(pos_entry->value, right_entry->value) > 0) {
                list_del(pos);
                q_release_element(pos_entry);
                break;
            }
            right = right->next;
        }
    }
    return q_size(head);
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    if (!head || list_empty(head) || list_is_singular(head))
        return q_size(head);

    struct list_head *pos, *pos_safe;
    list_for_each_safe (pos, pos_safe, head) {
        struct list_head *right = pos->next;
        element_t *pos_entry = list_entry(pos, element_t, list);

        while (right != head) {
            const element_t *right_entry = list_entry(right, element_t, list);
            if (strcmp(pos_entry->value, right_entry->value) < 0) {
                list_del(pos);
                q_release_element(pos_entry);
                break;
            }
            right = right->next;
        }
    }
    return q_size(head);
}

/* Merge all the queues into one sorted queue, which is in ascending/descending
 * order */
int q_merge(struct list_head *head, bool descend)
{
    // https://leetcode.com/problems/merge-k-sorted-lists/
    if (!head || list_empty(head)) {
        return 0;
    }

    queue_contex_t *entry = NULL, *safe = NULL;
    queue_contex_t *target = list_first_entry(head, queue_contex_t, chain);

    if (list_is_singular(head)) {
        return q_size(target->q);
    }

    list_for_each_entry_safe (entry, safe, head, chain) {
        if (entry != target && entry->q && !list_empty(entry->q)) {
            list_splice_tail(entry->q, target->q);
            INIT_LIST_HEAD(entry->q);
        }
    }

    q_sort(target->q, descend);

    return q_size(target->q);
}
