#ifndef IPPROTO_H
#define IPPROTO_H

#define IPPROTO_HOPOPTS		0		/* IPv6 hop-by-hop options */
#define	IPPROTO_ICMP		1		/* control message protocol */
#define	IPPROTO_IGMP		2		/* group mgmt protocol */
#define IPPROTO_IPV4		4
#define	IPPROTO_TCP		6		/* tcp */
#define	IPPROTO_EGP		8		/* exterior gateway protocol */
#define IPPROTO_PIGP		9
#define	IPPROTO_UDP		17		/* user datagram protocol */
#define	IPPROTO_DCCP		33		/* datagram congestion control protocol */
#define IPPROTO_IPV6		41
#define IPPROTO_ROUTING		43		/* IPv6 routing header */
#define IPPROTO_FRAGMENT	44		/* IPv6 fragmentation header */
#define IPPROTO_RSVP		46		/* resource reservation */
#define	IPPROTO_GRE		47		/* General Routing Encap. */
#define	IPPROTO_ESP		50		/* SIPP Encap Sec. Payload */
#define	IPPROTO_AH		51		/* SIPP Auth Header */
#define IPPROTO_NHRP		54		/* Next Hop Resolution */
#define IPPROTO_MOBILE		55
#define IPPROTO_ICMPV6		58		/* ICMPv6 */
#define IPPROTO_NONE		59		/* IPv6 no next header */
#define IPPROTO_DSTOPTS		60		/* IPv6 destination options */

#define	IPPROTO_ND		77		/* Sun net disk proto (temp.) */
#define	IPPROTO_EIGRP		88		/* Cisco/GXS IGRP */
#define IPPROTO_OSPF		89
#define IPPROTO_PIM		103
#define IPPROTO_IPCOMP		108
#define IPPROTO_VRRP		112 /* See also CARP. */
#define IPPROTO_PGM             113
#define IPPROTO_SCTP		132
#define IPPROTO_MOBILITY	135

extern lookup_table ipprotos;

#endif