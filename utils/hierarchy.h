#ifndef HIERARCHY_H
#define HIERARCHY_H

#include <stddef.h>

#include "../utils/raw_array.h"

#define DEFAULT_HIERARCHY_NODE_NAME         "ROOT"
#define DEFAULT_CHILDREN_SIZE               1


typedef struct hierarchy_node {
    int proto_num;
    char *proto_name;
    size_t tot_bytes;
    size_t tot_packets;
    raw_array *children;  /* array of hierarchy node pointers */
} hierarchy_node;


hierarchy_node *create_hierarchy_node(const char *proto_name, int proto_num, size_t tot_bytes, size_t tot_packets);
void add_hierarchy_node(hierarchy_node *parent, hierarchy_node *new_node);
void reset_hierarchy(hierarchy_node *root);
void destroy_hierarchy_node(void *node);  /* void pointer because it has to be passed to reset_arr */

#endif