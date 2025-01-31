#ifndef PPP_PROTOS_H
#define PPP_PROTOS_H

#include "../protocol_handler.h"

#ifndef PPP_IP
#define PPP_IP		    0x0021	/* Raw IP */
#endif
#ifndef PPP_OSI
#define PPP_OSI		    0x0023	/* OSI Network Layer */
#endif
#ifndef PPP_NS
#define PPP_NS		    0x0025	/* Xerox NS IDP */
#endif
#ifndef PPP_DECNET
#define PPP_DECNET	    0x0027	/* DECnet Phase IV */
#endif
#ifndef PPP_APPLE
#define PPP_APPLE	    0x0029	/* Appletalk */
#endif
#ifndef PPP_IPX
#define PPP_IPX		    0x002b	/* Novell IPX */
#endif
#ifndef PPP_VJC
#define PPP_VJC		    0x002d	/* Van Jacobson Compressed TCP/IP */
#endif
#ifndef PPP_VJNC
#define PPP_VJNC	    0x002f	/* Van Jacobson Uncompressed TCP/IP */
#endif
#ifndef PPP_BRPDU
#define PPP_BRPDU	    0x0031	/* Bridging PDU */
#endif
#ifndef PPP_STII
#define PPP_STII	    0x0033	/* Stream Protocol (ST-II) */
#endif
#ifndef PPP_VINES
#define PPP_VINES	    0x0035	/* Banyan Vines */
#endif
#ifndef PPP_ML
#define PPP_ML		    0x003d  /* Multi-Link PPP */
#endif
#ifndef PPP_IPV6
#define PPP_IPV6	    0x0057	/* IPv6 */
#endif
#ifndef PPP_COMP
#define PPP_COMP	    0x00fd	/* Compressed Datagram */
#endif

#ifndef PPP_HELLO
#define PPP_HELLO	    0x0201	/* 802.1d Hello Packets */
#endif
#ifndef PPP_LUXCOM
#define PPP_LUXCOM	    0x0231	/* Luxcom */
#endif
#ifndef PPP_SNS
#define PPP_SNS		    0x0233	/* Sigma Network Systems */
#endif
#ifndef PPP_MPLS_UCAST
#define PPP_MPLS_UCAST  0x0281  /* rfc 3032 */
#endif
#ifndef PPP_MPLS_MCAST
#define PPP_MPLS_MCAST  0x0283  /* rfc 3022 */
#endif

#ifndef PPP_IPCP
#define PPP_IPCP	    0x8021	/* IP Control Protocol */
#endif
#ifndef PPP_OSICP
#define PPP_OSICP	    0x8023	/* OSI Network Layer Control Protocol */
#endif
#ifndef PPP_NSCP
#define PPP_NSCP	    0x8025	/* Xerox NS IDP Control Protocol */
#endif
#ifndef PPP_DECNETCP
#define PPP_DECNETCP	0x8027	/* DECnet Control Protocol */
#endif
#ifndef PPP_APPLECP
#define PPP_APPLECP	    0x8029	/* Appletalk Control Protocol */
#endif
#ifndef PPP_IPXCP
#define PPP_IPXCP	    0x802b	/* Novell IPX Control Protocol */
#endif
#ifndef PPP_STIICP
#define PPP_STIICP	    0x8033	/* Stream Protocol Control Protocol */
#endif
#ifndef PPP_VINESCP
#define PPP_VINESCP	    0x8035	/* Banyan Vines Control Protocol */
#endif
#ifndef PPP_IPV6CP
#define PPP_IPV6CP	    0x8057	/* IPv6 Control Protocol */
#endif
#ifndef PPP_CCP
#define PPP_CCP		    0x80fd	/* Compress Control Protocol */
#endif
#ifndef PPP_MPLSCP
#define PPP_MPLSCP      0x8281  /* rfc 3022 */
#endif

#ifndef PPP_LCP
#define PPP_LCP		    0xc021	/* Link Control Protocol */
#endif
#ifndef PPP_PAP
#define PPP_PAP		    0xc023	/* Password Authentication Protocol */
#endif
#ifndef PPP_LQM
#define PPP_LQM		    0xc025	/* Link Quality Monitoring */
#endif
#ifndef PPP_SPAP
#define PPP_SPAP        0xc027
#endif
#ifndef PPP_CHAP
#define PPP_CHAP	    0xc223	/* Challenge Handshake Authentication Protocol */
#endif
#ifndef PPP_BACP
#define PPP_BACP	    0xc02b	/* Bandwidth Allocation Control Protocol */
#endif
#ifndef PPP_BAP
#define PPP_BAP		    0xc02d	/* BAP */
#endif
#ifndef PPP_MPCP
#define PPP_MPCP		0xc03d	/* Multi-Link */
#endif
#ifndef PPP_SPAP_OLD
#define PPP_SPAP_OLD    0xc123
#endif
#ifndef PPP_EAP
#define PPP_EAP         0xc227
#endif


extern protocol_handler ppp_protos[];

#endif