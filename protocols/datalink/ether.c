#include <stdio.h>
#include <arpa/inet.h>

#include "ether.h"
#include "../../ethertypes.h"
#include "../../utils/raw_array.h"
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
    { ETHERTYPE_REVARP,         "Reverse ARP"},
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
    { ETHERTYPE_ARISTA,         "Arista Vendor Specific Protocol" }
};

void print_ether_hdr(const uint8_t *pkt) {
    ether_hdr *ether_header = (ether_hdr *)pkt;
	uint16_t ethertype = ntohs(ether_header->ethertype);
	const char *protocol_name = get_value(ethers, ethertype, (sizeof(ethers) / sizeof(ethers[0])));
    
	/* ========================= printing src (MAC) > dest (MAC) ========================= */
	for (int i = 0; i < 5; i ++) {      /* the last 8 bits are printed after without the trailing ':' */
		printf("%02x:", ether_header->src_addr[i] & 0xff);
	}
    printf("%02x", ether_header->src_addr[5]);
    printf(" > ");
	for (int i = 0; i < 5; i ++) {      /* the last 8 bits are printed after without the trailing ':' */
		printf("%02x:", ether_header->dest_addr[i] & 0xff);
	}
    printf("%02x", ether_header->dest_addr[5] & 0xff);
    /* =================================================================================== */

	/* =============================== printing ethertype ================================ */
	printf(", ethertype: 0x%04x", ethertype);
	if (NULL != protocol_name) printf(" (%s)", protocol_name); 
    /* =================================================================================== */
}

void visualize_ether_hdr(const uint8_t *pkt) {
    ether_hdr *ether_header = (ether_hdr *)pkt;
    char dest_addr[18];
    char src_addr[18];
    char ethertype[7];

    snprintf(dest_addr, sizeof(dest_addr), MAC_FORMAT, MAC_TO_STR(ether_header->dest_addr));
    snprintf(src_addr, sizeof(src_addr), MAC_FORMAT, MAC_TO_STR(ether_header->src_addr));
    snprintf(ethertype, sizeof(ethertype), "0x%04x", ntohs(ether_header->ethertype));

    start_printing();
    print_hdr_info(ETHER_HEADER_LABEL, NULL);
    print_field(DEST_ADDR_LABEL, dest_addr, 0);
    print_field(SRC_ADDR_LABEL, src_addr, 0);
    print_field(ETHERTYPE_LABEL, ethertype, 0);
    end_printing();
}