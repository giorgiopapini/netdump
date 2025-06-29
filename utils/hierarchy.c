#include "hierarchy.h"

#include <stdlib.h>
#include <string.h>

#include "../status_handler.h"


hierarchy_node *create_hierarchy_node(const char *proto_name, int proto_num, size_t tot_bytes, size_t tot_packets) {
    hierarchy_node *new_node = malloc(sizeof *new_node);
    CHECK_NULL_EXIT(new_node);

    new_node->proto_num = proto_num;
    new_node->proto_name = strdup(proto_name);
    new_node->tot_bytes = tot_bytes;
    new_node->tot_packets = tot_packets;
    new_node->children = create_raw_array(DEFAULT_CHILDREN_SIZE);
    return new_node;
}

void reset_hierarchy(hierarchy_node *root) {
    size_t i;
    CHECK_NULL_EXIT(root);
    
    root->tot_bytes = 0;
    root->tot_packets = 0;
    if (NULL != root->children) {
        if (NULL != root->children->values) {
            for (i = 0; i < root->children->len; i ++) {
                destroy_hierarchy_node(root->children->values[i]);
            }
            free(root->children->values);
        }
        free(root->children);
    }
    root->children = NULL;
}

void destroy_hierarchy_node(void *node) {
    hierarchy_node *r_node = (hierarchy_node *)node;
    CHECK_NULL_RET(node);

    if (NULL != r_node->proto_name) free(r_node->proto_name);
    if (NULL != r_node->children) {
        reset_arr(r_node->children, destroy_hierarchy_node);
        free(r_node->children);
    }
    r_node->proto_name = NULL;
    r_node->children = NULL;
    
    free(r_node);
}