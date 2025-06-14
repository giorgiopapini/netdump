#ifndef ETHERTYPES_H
#define ETHERTYPES_H

#include "../utils/hashmap.h"

#ifndef ETHERTYPE_GRE_ISO
#define ETHERTYPE_GRE_ISO       0x00FE  /* not really an ethertype only used in GRE */
#endif
#ifndef ETHERTYPE_PUP
#define	ETHERTYPE_PUP		0x0200	/* PUP protocol */
#endif
#ifndef ETHERTYPE_IP
#define	ETHERTYPE_IP		0x0800	/* IP protocol */
#endif
#ifndef ETHERTYPE_ARP
#define ETHERTYPE_ARP		0x0806	/* Addr. resolution protocol */
#endif
#ifndef ETHERTYPE_RARP
#define ETHERTYPE_RARP  	0x8035	/* reverse Addr. resolution protocol */
#endif
#ifndef ETHERTYPE_NS
#define ETHERTYPE_NS		0x0600
#endif
#ifndef	ETHERTYPE_SPRITE
#define	ETHERTYPE_SPRITE	0x0500
#endif
#ifndef ETHERTYPE_TRAIL
#define ETHERTYPE_TRAIL		0x1000
#endif
#ifndef	ETHERTYPE_MOPDL
#define	ETHERTYPE_MOPDL		0x6001
#endif
#ifndef	ETHERTYPE_MOPRC
#define	ETHERTYPE_MOPRC		0x6002
#endif
#ifndef	ETHERTYPE_DN
#define	ETHERTYPE_DN		0x6003
#endif
#ifndef	ETHERTYPE_LAT
#define	ETHERTYPE_LAT		0x6004
#endif
#ifndef ETHERTYPE_SCA
#define ETHERTYPE_SCA		0x6007
#endif
#ifndef ETHERTYPE_TEB
#define ETHERTYPE_TEB		0x6558
#endif
#ifndef	ETHERTYPE_LANBRIDGE
#define	ETHERTYPE_LANBRIDGE	0x8038
#endif
#ifndef	ETHERTYPE_DECDNS
#define	ETHERTYPE_DECDNS	0x803c
#endif
#ifndef	ETHERTYPE_DECDTS
#define	ETHERTYPE_DECDTS	0x803e
#endif
#ifndef	ETHERTYPE_VEXP
#define	ETHERTYPE_VEXP		0x805b
#endif
#ifndef	ETHERTYPE_VPROD
#define	ETHERTYPE_VPROD		0x805c
#endif
#ifndef ETHERTYPE_ATALK
#define ETHERTYPE_ATALK		0x809b
#endif
#ifndef ETHERTYPE_AARP
#define ETHERTYPE_AARP		0x80f3
#endif
#ifndef	ETHERTYPE_TIPC
#define	ETHERTYPE_TIPC		0x88ca
#endif
#ifndef	ETHERTYPE_8021Q
#define	ETHERTYPE_8021Q		0x8100
#endif

/* see:
        https://en.wikipedia.org/wiki/IEEE_802.1Q
    and https://en.wikipedia.org/wiki/QinQ
*/
#ifndef	ETHERTYPE_8021Q9100
#define	ETHERTYPE_8021Q9100	0x9100
#endif
#ifndef	ETHERTYPE_8021Q9200
#define	ETHERTYPE_8021Q9200	0x9200
#endif
#ifndef	ETHERTYPE_8021QinQ
#define	ETHERTYPE_8021QinQ      0x88a8
#endif
#ifndef ETHERTYPE_MACSEC
#define ETHERTYPE_MACSEC	0x88e5
#endif
#ifndef ETHERTYPE_IPX
#define ETHERTYPE_IPX		0x8137
#endif
#ifndef ETHERTYPE_IPV6
#define ETHERTYPE_IPV6		0x86dd
#endif
#ifndef ETHERTYPE_PPP
#define	ETHERTYPE_PPP		0x880b
#endif
#ifndef ETHERTYPE_MPCP
#define	ETHERTYPE_MPCP		0x8808
#endif
#ifndef ETHERTYPE_SLOW
#define	ETHERTYPE_SLOW		0x8809
#endif
#ifndef	ETHERTYPE_MPLS
#define	ETHERTYPE_MPLS		0x8847
#endif
#ifndef	ETHERTYPE_MPLS_MULTI
#define	ETHERTYPE_MPLS_MULTI	0x8848
#endif
#ifndef ETHERTYPE_PPPOED
#define ETHERTYPE_PPPOED	0x8863
#endif
#ifndef ETHERTYPE_PPPOES
#define ETHERTYPE_PPPOES	0x8864
#endif
#ifndef ETHERTYPE_NSH
#define ETHERTYPE_NSH		0x894F
#endif
#ifndef ETHERTYPE_PPPOED2
#define ETHERTYPE_PPPOED2	0x3c12
#endif
#ifndef ETHERTYPE_PPPOES2
#define ETHERTYPE_PPPOES2	0x3c13
#endif
#ifndef ETHERTYPE_MS_NLB_HB
#define ETHERTYPE_MS_NLB_HB	0x886f /* MS Network Load Balancing Heartbeat */
#endif
#ifndef ETHERTYPE_JUMBO
#define ETHERTYPE_JUMBO         0x8870
#endif
#ifndef ETHERTYPE_LLDP
#define ETHERTYPE_LLDP          0x88cc
#endif
#ifndef ETHERTYPE_EAPOL
#define ETHERTYPE_EAPOL		0x888e
#endif
#ifndef ETHERTYPE_REALTEK
#define ETHERTYPE_REALTEK	0x8899	/* Realtek layer 2 protocols and switch tags */
#endif
#ifndef ETHERTYPE_AOE
#define ETHERTYPE_AOE		0x88a2
#endif
#ifndef ETHERTYPE_PTP
#define ETHERTYPE_PTP		0x88f7
#endif
#ifndef	ETHERTYPE_LOOPBACK
#define	ETHERTYPE_LOOPBACK	0x9000
#endif
#ifndef	ETHERTYPE_VMAN
#define	ETHERTYPE_VMAN	        0x9100 /* Extreme VMAN Protocol */
#endif
#ifndef	ETHERTYPE_CFM_OLD
#define	ETHERTYPE_CFM_OLD       0xabcd /* 802.1ag depreciated */
#endif
#ifndef	ETHERTYPE_CFM
#define	ETHERTYPE_CFM           0x8902 /* 802.1ag */
#endif
#ifndef	ETHERTYPE_IEEE1905_1
#define	ETHERTYPE_IEEE1905_1    0x893a /* IEEE 1905.1 */
#endif
#ifndef	ETHERTYPE_ISO
#define	ETHERTYPE_ISO           0xfefe  /* nonstandard - used in Cisco HDLC encapsulation */
#endif
#ifndef	ETHERTYPE_CALM_FAST
#define	ETHERTYPE_CALM_FAST     0x1111  /* ISO CALM FAST */
#endif
#ifndef	ETHERTYPE_GEONET_OLD
#define	ETHERTYPE_GEONET_OLD    0x0707  /* ETSI GeoNetworking (before Jan 2013) */
#endif
#ifndef	ETHERTYPE_GEONET
#define	ETHERTYPE_GEONET        0x8947  /* ETSI GeoNetworking (Official IEEE registration from Jan 2013) */
#endif
#ifndef	ETHERTYPE_ARISTA
#define	ETHERTYPE_ARISTA        0xd28b /*  Arista Networks vendor specific EtherType */
#endif

/* around (400–600) ethertypes */
#define ETHERTYPES_BUCKETS_NUM  512


extern hashmap *ethertypes;

void load_ethertypes(void);

#endif