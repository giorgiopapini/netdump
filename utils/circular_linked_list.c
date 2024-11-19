#include <stdlib.h>

#include "circular_linked_list.h"
#include "../status_handler.h"


node *create_node(void *content) {
    node *new_node = (node *)malloc(sizeof(node));
    if (NULL == new_node) raise_error(NULL_POINTER, 1, NULL, "new_node", __FILE__);

    new_node->content = content;
    new_node->next = NULL;
    new_node->prev = NULL;
    return new_node;
}

void push_node(circular_list *list, node *new_node, int max_len, void (*deallocate_content_func)(void *)) {
    node *tmp = list->head;

    if (NULL == list->head) {
        new_node->next = new_node;
        new_node->prev = new_node;
        list->head = new_node;
        list->len ++;
    } 
    else {
        while (tmp->next != list->head) tmp = tmp->next;
        tmp->next = new_node;
        new_node->prev = tmp;

        if ((list->len + 1) > max_len) { /* if out of bounds, deallocate head and set to head the immediately next node */
            tmp = list->head->next;
            destroy_node(list->head, deallocate_content_func);
            list->head = tmp;
        }
        else list->len ++;  /* otherwise do nothing, just increment the actual size of the list */

        new_node->next = list->head;
        list->head->prev = new_node;
    }

    list->curr = new_node;  /* when new new node inserted, curr pointer always points to the new node */
}

void destroy_node(node *curr, void (*deallocate_content)(void *)) {
    if (NULL == curr) raise_error(NULL_POINTER, 1, NULL, "node", __FILE__);
    else {
        if (NULL != deallocate_content) deallocate_content(curr->content);
        free(curr);
    }
}