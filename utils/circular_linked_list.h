#ifndef CIRCULAR_LINKED_LIST
#define CIRCULAR_LINKED_LIST

typedef struct node {
    void *content;
    struct node *next;
    struct node *prev;
} node;

typedef struct circular_list {
    struct node *head;
    int len;
} circular_list;

node *create_node(void *content);
void push_node(circular_list *list, node *new_node, int max_len);

void destroy_node(node *curr, void (*deallocate_content)(void *));

#endif