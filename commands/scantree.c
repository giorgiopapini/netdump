#include "scantree.h"

#include <stdio.h>
#include <string.h>

#include "../utils/formats.h"
#include "../utils/colors.h"

void _print_hierarchy_node(hierarchy_node *node, size_t tot_space);


void _print_hierarchy_node(hierarchy_node *node, size_t tot_space) {
    size_t i;
    size_t new_tot_space;

    if (node == NULL) return;

    for (i = 0; i < tot_space; i ++) printf(" ");

    printf(PREFIX_STR CYAN "%s" RESET_COLOR, node->proto_name);
    printf(
        ": {bytes: " YELLOW "%ld" RESET_COLOR ", pkts: " YELLOW "%ld" RESET_COLOR "}\n",
        node->tot_bytes,
        node->tot_packets
    );
    new_tot_space = tot_space + strlen(node->proto_name) + strlen(PREFIX_STR);
    
    if (NULL != node->children && NULL != node->children->values) {
        for (i = 0; i < node->children->len; i ++) {
            _print_hierarchy_node(node->children->values[i], new_tot_space);
        }
    }
}

void execute_scantree(hierarchy_node *root) {
    printf("\n");
    _print_hierarchy_node(root, 0);
    printf("\n");
}