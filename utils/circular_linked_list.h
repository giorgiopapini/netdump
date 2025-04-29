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
    size_t len;
} circular_list;

node *create_node(void *content);
int compare_nodes(node *n1, node *n2);
void push_node(circular_list *list, node *new_node, size_t max_len, void (*deallocate_content_func)(void *));
void destroy_node(node *curr, void (*deallocate_content)(void *));
void destroy_list(circular_list *list, void (*deallocate_content)(void *));

#endif