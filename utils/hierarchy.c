#include "hierachy.h"

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

void destroy_hierarchy_node(void *node) {
    hierarchy_node *r_node = (hierarchy_node *)node;
    CHECK_NULL_RET(node);

    if (NULL != r_node->proto_name) free(r_node->proto_name);
    if (NULL != r_node->children) {
        reset_arr(r_node->children, destroy_hierarchy_node);
        free(r_node->children);
    }
    free(r_node);
}