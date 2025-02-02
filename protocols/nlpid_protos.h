#ifndef NLPID_PROTOS_H
#define NLPID_PROTOS_H

#include "../protocol_handler.h"

#ifndef NLPID_NULLNS
#define	NLPID_NULLNS        0x00
#endif
#ifndef NLPID_Q933
#define NLPID_Q933          0x08  /* ANSI T1.617 Annex D or ITU-T Q.933 Annex A */
#endif
#ifndef NLPID_LMI
#define NLPID_LMI           0x09  /* The original, aka Cisco, aka Gang of Four */
#endif
#ifndef NLPID_SNAP
#define NLPID_SNAP          0x80
#endif
#ifndef NLPIID_CLNP
#define	NLPID_CLNP	        0x81  /* iso9577 */
#endif
#ifndef NLPID_ESIS
#define	NLPID_ESIS	        0x82  /* iso9577 */
#endif
#ifndef NLPID_ISIS
#define	NLPID_ISIS	        0x83  /* iso9577 */
#endif
#ifndef NLPID_CONS
#define NLPID_CONS          0x84
#endif
#ifndef NLPID_IDRP
#define NLPID_IDRP          0x85
#endif
#ifndef NLPID_MFR
#define NLPID_MFR           0xb1  /* FRF.15 */
#endif
#ifndef NLPID_SPB
#define NLPID_SPB           0xc1  /* IEEE 802.1aq/D4.5 */
#endif
#ifndef NLPID_IP
#define NLPID_IP            0xcc
#endif
#ifndef NLPID_PPP
#define NLPID_PPP           0xcf
#endif
#ifndef NLPID_X25_ESIS
#define NLPID_X25_ESIS      0x8a
#endif
#ifndef NLPID_IP6
#define NLPID_IP6           0x8e
#endif


extern protocol_handler nlpid_protos[];

#endif