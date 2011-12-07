/* network.h */

/* All includes needed for the network */

#include <ip.h>
#include <ether.h>
#include <ipreass.h>
#include <icmp.h>
#include <udp.h>
#include <tcp.h>
#include <tcpfsm.h>
#include <tcpstat.h>
#include <tcptimer.h>
#include <tcb.h>
#include <net.h>
#include <dgram.h>
#include <arp.h>
#include <fserver.h>
#include <rfile.h>
#include <domain.h>
#include <netif.h>
#include <route.h>
#include <rip.h>
#include <daemon.h>
#include <snmpvars.h>
#include <bootp.h>

/* Declarations data conversion and checksum routines */

extern	short		cksum();	/* 1s comp of 16-bit 2s comp sum*/

extern	short		hs2net();	/* host to network short	*/
extern	short		net2hs();	/* network to host short	*/
extern	long		hl2net();	/* host-to-network long		*/
extern	long		net2hl();	/* network to host long		*/
extern	long		hl2vax();	/* pdp11-to-vax long		*/
extern	long		vax2hl();	/* vax-to-pdp11 long		*/

/* network macros */
#define hi8(x)   (unsigned char)  (((long) (x) >> 16) & 0x00ff)
#define low16(x) (unsigned short) ((long) (x) & 0xffff)

#define	BYTE(x, y)	(x[y]&0xff)

/* for talking to VAXen */
#define h2vax(x) (unsigned) ((unsigned) ((x)>>8) + (unsigned)((x)<<8))
#define vax2h(x) (unsigned) ((unsigned) ((x)>>8) + (unsigned)((x)<<8))

