#ifndef IP_PROTOS_H
#define IP_PROTOS_H

#include "../utils/hashmap.h"

#ifndef IPPROTO_HOPOPTS
#define IPPROTO_HOPOPTS		0		/* IPv6 hop-by-hop options */
#endif
#ifndef IPPROTO_ICMP
#define	IPPROTO_ICMP		1		/* control message protocol */
#endif
#ifndef IPPROTO_IGMP
#define	IPPROTO_IGMP		2		/* group mgmt protocol */
#endif
#ifndef IPPROTO_IPV4
#define IPPROTO_IPV4		4
#endif
#ifndef IPPROTO_TCP
#define	IPPROTO_TCP		6		/* tcp */
#endif
#ifndef IPPROTO_EGP
#define	IPPROTO_EGP		8		/* exterior gateway protocol */
#endif
#ifndef IPPROTO_PIGP
#define IPPROTO_PIGP		9
#endif
#ifndef IPPROTO_UDP
#define	IPPROTO_UDP		17		/* user datagram protocol */
#endif
#ifndef IPPROTO_DCCP
#define	IPPROTO_DCCP		33		/* datagram congestion control protocol */
#endif
#ifndef IPPROTO_IPV6
#define IPPROTO_IPV6		41
#endif
#ifndef IPPROTO_ROUTING
#define IPPROTO_ROUTING		43		/* IPv6 routing header */
#endif
#ifndef IPPROTO_FRAGMENT
#define IPPROTO_FRAGMENT	44		/* IPv6 fragmentation header */
#endif
#ifndef IPPROTO_RSVP
#define IPPROTO_RSVP		46		/* resource reservation */
#endif
#ifndef IPPROTO_GRE
#define	IPPROTO_GRE		47		/* General Routing Encap. */
#endif
#ifndef IPPROTO_ESP
#define	IPPROTO_ESP		50		/* SIPP Encap Sec. Payload */
#endif
#ifndef IPPROTO_AH
#define	IPPROTO_AH		51		/* SIPP Auth Header */
#endif
#ifndef IPPROTO_NHRP
#define IPPROTO_NHRP		54		/* Next Hop Resolution */
#endif
#ifndef IPPROTO_MOBILE
#define IPPROTO_MOBILE		55
#endif
#ifndef IPPROTO_ICMPV6
#define IPPROTO_ICMPV6		58		/* ICMPv6 */
#endif
#ifndef IPPROTO_NONE
#define IPPROTO_NONE		59		/* IPv6 no next header */
#endif
#ifndef IPPROTO_DSTOPTS
#define IPPROTO_DSTOPTS		60		/* IPv6 destination options */
#endif
#ifndef IPPROTO_MOBILITY_OLD
#define IPPROTO_MOBILITY_OLD	62
#endif
#ifndef IPPROTO_ND
#define	IPPROTO_ND		77		/* Sun net disk proto (temp.) */
#endif
#ifndef IPPROTO_EIGRP
#define	IPPROTO_EIGRP		88		/* Cisco/GXS IGRP */
#endif
#ifndef IPPROTO_OSPF
#define IPPROTO_OSPF		89
#endif
#ifndef IPPROTO_PIM
#define IPPROTO_PIM		103
#endif
#ifndef IPPROTO_IPCOMP
#define IPPROTO_IPCOMP		108
#endif
#ifndef IPPROTO_VRRP
#define IPPROTO_VRRP		112 /* See also CARP. */
#endif
#ifndef IPPROTO_PGM
#define IPPROTO_PGM             113
#endif
#ifndef IPPROTO_SCTP
#define IPPROTO_SCTP		132
#endif
#ifndef IPPROTO_MOBILITY
#define IPPROTO_MOBILITY	135
#endif

/* around 256 ip_protos */
#define IP_PROTOS_BUCKETS_NUM   512


extern hashmap *ip_protos;

void load_ip_protos(void);

#endif