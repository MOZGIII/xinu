/* netif.h - NIGET */

#define	NI_MAXHWA 14			/* max size of any hardware	*/
                                /*  (physical) net address  */
struct  hwa {                   /* a hardware address       */
    int     ha_len;             /* length of this address   */
	char	ha_addr[NI_MAXHWA];	/* actual bytes of the address	*/
};

#define NI_INQSZ    30          /* interface input queue size   */
#define NETNLEN     30          /* length of network name   */

#define NI_LOCAL    0           /* index of local interface */
#define NI_PRIMARY  1           /* index of primary interface   */

/* interface states */

#define	NIS_UP		0x1
#define	NIS_DOWN	0x2
#define	NIS_TESTING	0x3

/* Definitions of network interface structure (one per interface)	*/

struct  netif {                 /* info about one net interface */
    char    ni_num;             /* handy index of this interface */
    char    ni_name[NETNLEN];   /* domain name of this interface*/
    char    ni_state;           /* interface states: NIS_ above */
    IPaddr  ni_ip;              /* IP address for this interface*/
    IPaddr  ni_gateway;         /* Gateway */
    IPaddr  ni_nserver;         /* Name server */
    IPaddr  ni_tserver;         /* Time server */
    IPaddr  ni_net;             /* network IP address       */
    IPaddr  ni_subnet;          /* subnetwork IP address    */
    IPaddr  ni_mask;            /* IP subnet mask for interface */
    IPaddr  ni_brc;             /* IP broadcast address     */
    IPaddr  ni_nbrc;            /* IP net broadcast address */
    int     ni_mtu;             /* max transfer unit (bytes)    */
    int     ni_hwtype;          /* hardware type (for ARP)  */
    struct  hwa ni_hwa;         /* hardware address of interface*/
    struct  hwa ni_hwb;         /* hardware broadcast address   */
    Bool    ni_ivalid;          /* is ni_ip valid?      */
    Bool    ni_nvalid;          /* is ni_name valid?        */
    Bool    ni_svalid;          /* is ni_subnet valid?      */
    int     ni_dev;             /* the Xinu device descriptor   */
    int     ni_ipinq;           /* IP input queue       */
    int     ni_outp;            /* port for outgoing packets */
	/* Interface MIB */
    char    *ni_descr;          /* text description of hardware */
    int     ni_mtype;           /* MIB interface type       */
    long    ni_speed;           /* bits per second      */
	char	ni_admstate;		/* administrative status (NIS_*)*/
	long	ni_lastchange;		/* last state change (1/100 sec)*/
    long    ni_ioctets;         /* # of octets received     */
    long    ni_iucast;          /* # of unicast received    */
    long    ni_inucast;         /* # of non-unicast received    */
	long	ni_idiscard;		/* # dropped - output queue full*/
    long    ni_ierrors;         /* # input packet errors    */
	long	ni_iunkproto;		/* # in packets for unk. protos */
    long    ni_ooctets;         /* # of octets sent     */
    long    ni_oucast;          /* # of unicast sent        */
    long    ni_onucast;         /* # of non-unicast sent    */
	long	ni_odiscard;		/* # output packets discarded	*/
    long    ni_oerrors;         /* # output packet errors   */
    long    ni_oqlen;           /* output queue length      */
};

#define NIGET(ifn)  ((struct ep *)deq(nif[ifn].ni_ipinq))

#define NIF Neth+1              /* # of interfaces + Noth +1 for local*/

extern char *deq(int);
extern struct netif	nif[];
