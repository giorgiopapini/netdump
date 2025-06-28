#include "protocol_handler.h"

#include <pcap/pcap.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "command_handler.h"
#include "utils/hashmap.h"
#include "utils/hierachy.h"
#include "utils/timestamp.h"
#include "utils/colors.h"
#include "protocols/proto_tables_handler.h"
#include "status_handler.h"

#define INLINE_SEPARATOR 		" | "
#define SPACE_SEPARATOR 		"\n"
#define UNKNOWN_PROTO_LABEL 	"Unknown"


int _is_output_arg_valid(const char *raw_val);
void _update_hierarchy(hierarchy_node *root, raw_array *proto_chain);

int _is_output_arg_valid(const char *raw_val) {
	if (NULL == raw_val) return 1;  /* will be set as std output by default */
	if (
		0 != strcmp(raw_val, OUTPUT_ARG_VAL_STD) &&
		0 != strcmp(raw_val, OUTPUT_ARG_VAL_RAW) &&
		0 != strcmp(raw_val, OUTPUT_ARG_VAL_ART)
	) return 0;
	return 1;
}

void _update_hierarchy(hierarchy_node *root, raw_array *proto_chain) {
	size_t i, j;
	hierarchy_node *curr_layer = root;
	hierarchy_node *curr_h_node = NULL;
	proto_chain_node *curr_c_node = NULL;
	int found = 0;

	CHECK_NULL_RET(root);
	if (NULL == root->children) root->children = create_raw_array(DEFAULT_CHILDREN_SIZE); 
	CHECK_NULL_RET(proto_chain);
	CHECK_NULL_RET(proto_chain->values);

	for (i = 0; i < proto_chain->len; i ++) {
		curr_c_node = ((proto_chain_node *)proto_chain->values[i]);
		for (j = 0; j < curr_layer->children->len; j ++) {
			curr_h_node = ((hierarchy_node *)curr_layer->children->values[j]);
			if (curr_c_node->proto_num == curr_h_node->proto_num) {
				found = 1;
				break;
			}
		}

		if (!found) {
			curr_h_node = create_hierarchy_node(curr_c_node->proto_name, curr_c_node->proto_num, curr_c_node->bytes, 1);
			insert(curr_layer->children, curr_h_node);
		}
		else {
			curr_h_node->tot_bytes += curr_c_node->bytes;
			curr_h_node->tot_packets ++;
		}

		root->tot_bytes += curr_c_node->bytes;
		curr_layer = curr_h_node;
	}
	root->tot_packets ++;
}

proto_chain_node *create_proto_chain_node(int proto_num, const char *proto_name, size_t bytes) {
	proto_chain_node *new_node = malloc(sizeof *new_node);
	CHECK_NULL_EXIT(new_node);

	new_node->bytes = bytes;
	new_node->proto_name = strdup(proto_name);
	new_node->proto_num = proto_num;
	return new_node;
}

void destroy_proto_chain_node(void *node) {
	CHECK_NULL_RET(node);
	if (NULL != ((proto_chain_node *)node)->proto_name) free(((proto_chain_node *)node)->proto_name);
	free(node);
}

output_format get_output_format(command *cmd) {
	char *out_format = get_raw_val(cmd, OUTPUT_FORMAT_ARG);
	if (NULL == out_format) return OUTPUT_FORMAT_BASIC;

	if (is_command(cmd, ANALYZE_COMMAND)) return OUTPUT_FORMAT_BASIC;
	else {
		if (0 == strcmp(out_format, OUTPUT_ARG_VAL_STD)) return OUTPUT_FORMAT_BASIC;
		else if (0 == strcmp(out_format, OUTPUT_ARG_VAL_RAW)) return OUTPUT_FORMAT_RAW;
		else if (0 == strcmp(out_format, OUTPUT_ARG_VAL_ART)) return OUTPUT_FORMAT_ACII_ART;
	}
	return OUTPUT_FORMAT_BASIC;
}

protocol_handler *get_protocol_handler(int target_proto, hashmap *proto_table) {
	hashmap_entry *entry = get_entry(proto_table, target_proto);
	if (NULL == entry) return NULL;
	
	return (protocol_handler *)entry->value;
}

void print_separator(command *cmd) {
	char *out_format = get_raw_val(cmd, OUTPUT_FORMAT_ARG);
	if (NULL == out_format) {
		printf(INLINE_SEPARATOR);
		return;
	}

	if (is_command(cmd, ANALYZE_COMMAND)) printf(INLINE_SEPARATOR);
	else {
		if (0 == strcmp(out_format, OUTPUT_ARG_VAL_STD)) printf(INLINE_SEPARATOR);
		else if (0 == strcmp(out_format, OUTPUT_ARG_VAL_RAW)) printf(SPACE_SEPARATOR);
		else if (0 == strcmp(out_format, OUTPUT_ARG_VAL_ART)) printf(SPACE_SEPARATOR);
	}
}

int should_print_pkt(command *cmd, protocol_layer layer) {
	int show_datalink = NULL != get_arg(cmd, DATALINK_HDR_ARG);
	int show_network = NULL == get_arg(cmd, NETWORK_HDR_ARG);
	int show_transport = NULL != get_arg(cmd, TRANSPORT_HDR_ARG);
	int show_application = NULL != get_arg(cmd, APPLICATION_HDR_ARG);

	switch (layer) {
		case PROTOCOL_LAYER_DATALINK:		return show_datalink; break;
		case PROTOCOL_LAYER_TRANSPORT:		return show_transport; break;
		case PROTOCOL_LAYER_NETWORK:		return show_network; break;
		case PROTOCOL_LAYER_APPLICATION:	return show_application; break;
		default: break;
	}
	return 1;
}

void dissect(
	command *cmd,
	raw_array *proto_chain,
	uint8_t *pkt, 
	size_t pkt_len, 
	int proto_id, 
	int proto_table_num,
	shared_libs *libs,
	custom_dissectors *custom_diss,
	int proto_shown
) {
	protocol_handler *handler = NULL;
	protocol_info proto_info;
	output_format out_format = OUTPUT_FORMAT_NONE;  /* layer specific */
	output_func_t out_func = NULL;
	hashmap *proto_hashmap = NULL;
	size_t tot_bytes;

	if (0 <= proto_table_num && PROTO_TABLE_COUNT >= proto_table_num)
		proto_hashmap = get_proto_table_from_id((proto_table_id)proto_table_num);
	
	if (custom_diss->len > 0) handler = get_custom_protocol_handler(custom_diss, proto_id, proto_hashmap, libs);
	if (NULL == handler) handler = get_protocol_handler(proto_id, proto_hashmap);
	if (NULL == handler) return;

	if (NULL == handler->dissect_proto) {
		if (OUTPUT_FORMAT_RAW != get_output_format(cmd)) return;
		if (pkt_len > 0) {
			if (proto_shown > 0) print_separator(cmd);
			printf(CYAN "(%s) " RESET_COLOR, UNKNOWN_PROTO_LABEL);
			
			out_func = select_output_func(get_output_format(cmd), NULL, NULL);
			if (NULL != out_func) out_func(pkt, pkt_len, 0);
			proto_shown ++;
		}
		return;
	}
	
	if (should_print_pkt(cmd, handler->layer)) {  /* else -> default output format is OUTPUT_NONE */
		out_format = get_output_format(cmd);
		if (proto_shown > 0) print_separator(cmd);
		proto_shown ++;
		
		if (NULL == get_arg(cmd, NO_PROTOCOL_NAME_ARG)) {
			printf(CYAN "(%s) " RESET_COLOR, handler->protocol_name);
			if (OUTPUT_FORMAT_ACII_ART == out_format) printf("\n");  /* (PROTO_NAME) \n "Actual ASCII art" */
		}
	}

	proto_info = handler->dissect_proto(pkt, pkt_len);
	out_func = select_output_func(out_format, proto_info.print_protocol_func, proto_info.visualize_protocol_func);
	if (NULL != out_func) out_func(pkt, pkt_len, proto_info.hdr_len);
	
	if (0 == proto_info.hdr_len) tot_bytes = pkt_len;
	else tot_bytes = proto_info.hdr_len;
	insert(proto_chain, create_proto_chain_node(handler->protocol, handler->protocol_name, tot_bytes));

	if (NO_ENCAP_PROTO_TABLE != proto_info.encap_proto_table_num && pkt_len > proto_info.hdr_len) {
		dissect(
			cmd,
			proto_chain,
			(pkt + proto_info.hdr_len),
			(pkt_len - proto_info.hdr_len),
			proto_info.encap_protocol,
			proto_info.encap_proto_table_num,
			libs,
			custom_diss,
			proto_shown
		);
	}
}

void dissect_packet(command *cmd, packet *pkt, shared_libs *libs, custom_dissectors *custom_diss, hierarchy_node *root) {
	raw_array *proto_chain = create_raw_array(DEFAULT_PROTO_CHAIN_DEPTH);
	const char *raw_output_val = get_raw_val(cmd, OUTPUT_FORMAT_ARG);
	const output_format fmt = get_output_format(cmd);

	CHECK_NULL_EXIT(pkt);
	CHECK_NULL_EXIT(pkt->header);

	if (!_is_output_arg_valid(raw_output_val)) {
		raise_error(UNRECOGNIZED_OUTPUT_ERROR, 0, NULL, raw_output_val);
		return;
	}

	if (NULL == get_arg(cmd, NO_TIMESTAMP_ARG)) print_timestamp(pkt->header->ts);
	if (NULL != get_arg(cmd, PACKET_NUM_ARG)) printf(GREEN "(#%ld) " RESET_COLOR, pkt->num);
	/* if "ascii_art" than it adds a bit of spacing */
	if (OUTPUT_FORMAT_ACII_ART == fmt || OUTPUT_FORMAT_RAW == fmt) printf("\n\n");

	dissect(
		cmd,
		proto_chain,
		pkt->bytes,
		(size_t)pkt->header->caplen,
		pkt->datalink_type,
		DLT_PROTOS,
		libs,
		custom_diss,
		0
	);
	printf("\n");

	/* execute this in another thread or process (ALSO THE PROTO_CHAIN DEALLOCATION) */
	if (NULL != root) _update_hierarchy(root, proto_chain);
	reset_arr(proto_chain, destroy_proto_chain_node);
	free(proto_chain);
}