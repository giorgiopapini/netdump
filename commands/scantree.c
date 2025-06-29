#include "scantree.h"

#include <stdio.h>
#include <string.h>

#include "../utils/formats.h"
#include "../utils/colors.h"

void _print_hierarchy_node(hierarchy_node *node, const hierarchy_node *root, const size_t tot_space);


void _print_hierarchy_node(hierarchy_node *node, const hierarchy_node *root, const size_t tot_space) {
    size_t i;
    size_t new_tot_space;

    if (node == NULL) return;

    for (i = 0; i < tot_space; i ++) printf(" ");

    printf(PREFIX_STR CYAN "%s" RESET_COLOR, node->proto_name);
    printf(
        ": {bytes: " YELLOW "%ld" RESET_COLOR " (" MAGENTA "%0.2f%%" RESET_COLOR "),",
        node->tot_bytes,
        (root->tot_bytes > 0) ? (double)node->tot_bytes / (double)root->tot_bytes * 100 : 0
    );
    printf(
        " pkts: " YELLOW "%ld" RESET_COLOR " (" MAGENTA "%0.2f%%" RESET_COLOR ")}",
        node->tot_packets,
        (root->tot_packets) > 0 ? (double)node->tot_packets / (double)root->tot_packets * 100 : 0
    );
    printf("\n");

    new_tot_space = tot_space + strlen(node->proto_name) + strlen(PREFIX_STR);
    
    if (NULL != node->children && NULL != node->children->values) {
        for (i = 0; i < node->children->len; i ++) {
            _print_hierarchy_node(node->children->values[i], root, new_tot_space);
        }
    }
}

void execute_scantree(hierarchy_node *root) {
    printf("All percentages are calculated relative to the root\n\n");
    _print_hierarchy_node(root, root, 0);
    printf("\n");
}