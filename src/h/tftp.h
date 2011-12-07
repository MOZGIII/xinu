/* tftp.h */

/* TFTP Information protocol information obtained from RFC-783 		*/

#define TFTPPORT		69		/* default port number	*/
extern int tftpd();             /* tftp daemon          */
/* TFTP mode values							*/

#define	TFTP_NETASCII		0
#define	TFTP_OCTET		1
#define	TFTP_MAIL		2

/* Values for TFTP opcode field */

#define	TFTP_RRQ		1		/* Request to read	*/
#define	TFTP_WRQ		2		/* Request to write	*/
#define	TFTP_DATA		3		/* Data transfer	*/
#define	TFTP_ACK		4		/* Acknowledge		*/
#define	TFTP_ERR		5		/* Error packet		*/

/* Values for TFTP error codes */

#define	TFTP_NOTDEF		0		/* Error code not def	*/
#define	TFTP_FNOTF		1		/* File not found	*/
#define	TFTP_ACCESS		2		/* Access violation	*/
#define	TFTP_DSKFULL		3		/* Disk full		*/
#define	TFTP_ILLOP		4		/* Illegal operation	*/
#define	TFTP_BADTID		5		/* Bad transfer ID	*/
#define	TFTP_FEXIST		6		/* File already exists	*/
#define	TFTP_BADUSER		7		/* No such user		*/


/* TFTP Packet constants */

#define	TFTPHLEN	(sizeof(short) * 2)	/* Size of TFTP Header	*/
						/* opcode + short	*/
#define	TFTPDLEN		512		/* Max size of TFTP data*/
#define	TFTP_RETRY		5		/* # of xmit retries	*/
#define	TFTP_PRIO		5000		/* Priority of TFTPserv	*/

/* Structure for TFTP Read/Write requests */
struct	TFTP_req {
	short	opcode;			/* opcode = TFTP_WRQ or TFTP_RRQ*/
	char	file[1];		/* Anchor into file/mode string	*/
};

/* Structure for TFTP acknowledgment packets */
struct TFTP_ack {
	short	opcode;
	short	blknum;
};

/* Structure for TFTP data packets */
struct	TFTP_data {
	short	opcode;			/* TFTP Operation Code		*/
	short	blknum;			/* Block number/Error Code	*/
	char	data[TFTPDLEN];		/* Anchor into data		*/
};

/* Structure for TFTP error packets */
struct TFTP_err {
	short	opcode;			/* opcode = TFTP_ERR		*/
	short	errcode;		/* TFTP error number		*/
	char	errmsg[1];		/* Error string or NULL		*/
};


/* Definition of TFTP packet */
union	TFTP {
	struct	TFTP_req   t_req;
	struct	TFTP_ack   t_ack;
	struct	TFTP_data  t_data;
	struct	TFTP_err   t_err;
};
