#include <stdio.h>
#include <arpa/inet.h>
#include <string.h>

#include "ether.h"
#include "../../ethertypes.h"
#include "../../utils/lookup_table.h"
#include "../../utils/visualizer.h"
#include "../../utils/formats.h"

lookup_table ethers = {
	{ ETHERTYPE_IP,				"IPv4" },
    { ETHERTYPE_MPLS,			"MPLS unicast" },
    { ETHERTYPE_MPLS_MULTI,		"MPLS multicast" },
    { ETHERTYPE_IPV6,			"IPv6" },
    { ETHERTYPE_8021Q,			"802.1Q" },
    { ETHERTYPE_8021Q9100,		"802.1Q-9100" },
    { ETHERTYPE_8021QinQ,		"802.1Q-QinQ" },
    { ETHERTYPE_8021Q9200,		"802.1Q-9200" },
    { ETHERTYPE_MACSEC,			"802.1AE MACsec" },
    { ETHERTYPE_VMAN,			"VMAN" },
    { ETHERTYPE_PUP,        	"PUP" },
    { ETHERTYPE_ARP,            "ARP"},
    { ETHERTYPE_RARP,           "Reverse ARP"},
    { ETHERTYPE_NS,             "NS" },
    { ETHERTYPE_SPRITE,         "Sprite" },
    { ETHERTYPE_TRAIL,          "Trail" },
    { ETHERTYPE_MOPDL,          "MOP DL" },
    { ETHERTYPE_MOPRC,          "MOP RC" },
    { ETHERTYPE_DN,             "DN" },
    { ETHERTYPE_LAT,            "LAT" },
    { ETHERTYPE_SCA,            "SCA" },
    { ETHERTYPE_TEB,            "TEB" },
    { ETHERTYPE_LANBRIDGE,      "Lanbridge" },
    { ETHERTYPE_DECDNS,         "DEC DNS" },
    { ETHERTYPE_DECDTS,         "DEC DTS" },
    { ETHERTYPE_VEXP,           "VEXP" },
    { ETHERTYPE_VPROD,          "VPROD" },
    { ETHERTYPE_ATALK,          "Appletalk" },
    { ETHERTYPE_AARP,           "Appletalk ARP" },
    { ETHERTYPE_IPX,            "IPX" },
    { ETHERTYPE_PPP,            "PPP" },
    { ETHERTYPE_MPCP,           "MPCP" },
    { ETHERTYPE_SLOW,           "Slow Protocols" },
    { ETHERTYPE_PPPOED,         "PPPoE D" },
    { ETHERTYPE_PPPOES,         "PPPoE S" },
    { ETHERTYPE_EAPOL,          "EAPOL" },
    { ETHERTYPE_REALTEK,        "Realtek protocols" },
    { ETHERTYPE_MS_NLB_HB,      "MS NLB heartbeat" },
    { ETHERTYPE_JUMBO,          "Jumbo" },
    { ETHERTYPE_NSH,            "NSH" },
    { ETHERTYPE_LOOPBACK,       "Loopback" },
    { ETHERTYPE_ISO,            "OSI" },
    { ETHERTYPE_GRE_ISO,        "GRE-OSI" },
    { ETHERTYPE_CFM_OLD,        "CFM (old)" },
    { ETHERTYPE_CFM,            "CFM" },
    { ETHERTYPE_IEEE1905_1,     "IEEE1905.1" },
    { ETHERTYPE_LLDP,           "LLDP" },
    { ETHERTYPE_TIPC,           "TIPC"},
    { ETHERTYPE_GEONET_OLD,     "GeoNet (old)"},
    { ETHERTYPE_GEONET,         "GeoNet"},
    { ETHERTYPE_CALM_FAST,      "CALM FAST"},
    { ETHERTYPE_AOE,            "AoE" },
    { ETHERTYPE_PTP,            "PTP" },
    { ETHERTYPE_ARISTA,         "Arista Vendor Specific Protocol" },
    { 0,                        NULL }
};


size_t ether_hdr_len(const uint8_t *pkt) { return ETHER_LEN; }

void print_ether_hdr(const uint8_t *pkt) {
	uint16_t ethertype = ntohs(ETHERTYPE(pkt));
	const char *protocol_name = get_value(ethers, ethertype);
    
	/* ========================= printing src (MAC) > dest (MAC) ========================= */
    print_mac(SRC_ADDR(pkt));
    printf(" > ");
	print_mac(DEST_ADDR(pkt));
    /* =================================================================================== */

	/* =============================== printing ethertype ================================ */
	printf(", ethertype: 0x%04x", ethertype);
	if (NULL != protocol_name) printf(" (%s)", protocol_name); 
    /* =================================================================================== */
}

void visualize_ether_hdr(const uint8_t *pkt) {
    const char *protocol_name = get_value(ethers, ntohs(ETHERTYPE(pkt)));
    protocol_name = NULL == protocol_name ? UNKNOWN : protocol_name;
    char dest_addr[MAC_ADDR_STR_LEN];
    char src_addr[MAC_ADDR_STR_LEN];
    char ethertype[strlen(protocol_name) + 10];  /* proto (0xXXXX)'\0' */

    snprintf(dest_addr, sizeof(dest_addr), MAC_ADDR_FORMAT, MAC_TO_STR(DEST_ADDR(pkt)));
    snprintf(src_addr, sizeof(src_addr), MAC_ADDR_FORMAT, MAC_TO_STR(SRC_ADDR(pkt)));
    snprintf(ethertype, sizeof(ethertype), "%s (0x%04x)", protocol_name, ntohs(ETHERTYPE(pkt)));

    start_printing();
    print_hdr_info(ETHER_HEADER_LABEL, NULL);
    print_field(DEST_ADDR_LABEL, dest_addr, 0);
    print_field(SRC_ADDR_LABEL, src_addr, 0);
    print_field(ETHERTYPE_LABEL, ethertype, 0);
    end_printing();
}