#ifndef CIRCULAR_LINKED_LIST
#define CIRCULAR_LINKED_LIST

typedef struct node {
    void *content;
    struct node *next;
    struct node *prev;
} node;

typedef struct circular_list {
    struct node *head;
    struct node *curr;
    int len;
} circular_list;

node *create_node(void *content);
int compare_nodes(node *n1, node *n2);
void push_node(circular_list *list, node *new_node, int max_len, void (*deallocate_content_func)(void *));
void destroy_node(node *curr, void (*deallocate_content)(void *));

#endif