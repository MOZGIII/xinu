/* ec.h - 3C503 controller definitions */

#define EC0VEC  0x0d /*0x0d*/                /* vector */
#define EC0PORT 0x300               /* port */
#define EC0SMB  "0xd800"            /* shared memory base segment */

#define ADDROM      EC0PORT
#define SMBASE      0xd800          /* shared mem base */

/* 3c503 Gate Array */

#define GA (EC0PORT + 0x400)        /* address of Gate Array */
#define NS EC0PORT                  /* address of NS8390 */

/* Shared memory management parameters */

#define STTPG    (0x20)      /* First page of TX buffer */
#define STRTPG   (0x26)      /* First page of RX buffer */
#define STOPPG   (0x40)      /* Last page +1 of RX ring */

/* 3c503 Gate Array Registers */
#define PSTR  (GA+0)                /* Page Start must equal PSTART*/
#define PSPR  (GA+1)                /* Page stop must equal PSTOP */
#define DQTR  (GA+2)                /* DMA Timer Register */
#define BCFR  (GA+3)                /* I/O Base Jumper Read Register */
#define PCFR  (GA+4)                /* Mem Base Jumper Read Register */
#define GACFR (GA+5)                /* Configuration Register */
#define CTRL  (GA+6)                /* Control Register */
#define STREG (GA+7)                /* Status Register */
#define IDCFR (GA+8)                /* Int/DMA Configuration Register */
#define DAMSB (GA+9)                /* MSB of DMA Address Register */
#define DALSB (GA+10)               /* LSB of DMA Address Register */
#define VPTR2 (GA+11)               /* Vector Pointer Register 2 */
#define VPTR1 (GA+12)               /* Vector Pointer Register 1 */
#define VPTR0 (GA+13)               /* Vector Pointer Register 0 */
#define RFMSB (GA+14)               /* MSB of Register File (FIFO) */
#define RFLSB (GA+15)               /* LSB of Register File (FIFO) */

/* 3C503 GA CNTL Register Bits */

#define RST   (0x01)                /* Reset GA and NS8390 */
#define XSEL  (0x02)                /* Xcvr Select */
#define EALO  (0x04)                /* Window the Ethernet Address PROM */
#define DBSEL (0x20)                /* Configure FIFO */
#define DDIR  (0x40)                /* DMA Direction */
#define START (0x80)                /* Start GA DMA controller */

/* 3C503 GA STREG Bits */

#define DPRDY (0x80)                /* FIFO Data Port Ready */
#define UFLW  (0x40)                /* FIFO Read when Empty */
#define OFLW  (0x20)                /* Write to Full FIFO */
#define DTC   (0x10)                /* DMA Terminal Count */
#define DIP   (0x08)                /* DMA In Progress */

/* 3C503 GACFR Values */

#define NORM   (0x49)               /* Enable 8K shared mem, no DMA TC int */
#define IRQOFF (0xc9)               /* NORM and 8390 IRQ disabled */

/* End of 3C503 parameter definitions */

/* 8390 LAN Controller (page0) */
#define CMDR    (NS+0)
#define CLDA0   (NS+1)
#define PSTART  (NS+1)
#define CLDA1   (NS+2)
#define PSTOP   (NS+2)
#define BNRY    (NS+3)
#define TSR     (NS+4)
#define TPSR    (NS+4)
#define NCR     (NS+5)
#define TBCR0   (NS+5)
#define FIFO    (NS+6)
#define TBCR1   (NS+6)
#define ISR     (NS+7)
#define CRDA0   (NS+8)
#define RSAR0   (NS+8)
#define CRDA1   (NS+9)
#define RSAR1   (NS+9)
#define RBCR0   (NS+10)
#define RBCR1   (NS+11)
#define RSR     (NS+12)
#define RCR     (NS+12)
#define CNTR0   (NS+13)
#define TCR     (NS+13)
#define CNTR1   (NS+14)
#define DCR     (NS+14)
#define CNTR2   (NS+15)
#define IMR     (NS+15)

/* 8390 LAN Controller (page 1) */

#define PAR0    (NS+1)
#define PAR1    (NS+2)
#define PAR2    (NS+3)
#define PAR3    (NS+4)
#define PAR4    (NS+5)
#define PAR5    (NS+6)
#define CURR    (NS+7)
#define MAR0    (NS+8)
#define MAR1    (NS+9)
#define MAR2    (NS+10)
#define MAR3    (NS+11)
#define MAR4    (NS+12)
#define MAR5    (NS+13)
#define MAR6    (NS+14)
#define MAR7    (NS+15)

/* msr */
#define	MSK_RESET	0x80
#define	MSK_ENASH	0x40
#define	MSK_DECOD	0x3f

/* cmsr */
#define MSK_RD2     (0x20)
#define	MSK_STP		(0x01 | MSK_RD2)
#define	MSK_STA		(0x02 | MSK_RD2)
#define	MSK_TXP		(0x04 | MSK_RD2)
#define MSK_PG0     (0x00 | MSK_RD2)
#define MSK_PG1     (0x40 | MSK_RD2)

/* msr */
#define MSK_MENB	0x40

/* isr & imr */
#define IMASK		0x1f
#define	MSK_PRX		0x01
#define	MSK_PTX		0x02
#define	MSK_RXE		0x04
#define	MSK_TXE		0x08
#define	MSK_OVW		0x10
#define	MSK_CNT		0x20
#define	MSK_RDC		0x40
#define	MSK_RST		0x80

/* dcr */
#define	MSK_WTS		0x01
#define	MSK_BOS		0x02
#define	MSK_LAS		0x04
#define	MSK_BMS		0x08
#define	MSK_ARM		0x10
#define	MSK_FT00	0x00
#define	MSK_FT01	0x20
#define	MSK_FT10	0x40
#define	MSK_FT11	0x60

/* rcr */
#define	MSK_SEP		0x01
#define	MSK_AR		0x02
#define	MSK_AB		0x04
#define	MSK_AM		0x08
#define	MSK_PRO		0x10
#define	MSK_MON		0x20

/* tcr */
#define	MSK_CRC		0x01
#define	MSK_LB01	0x06
#define	MSK_ATD		0x08
#define	MSK_OFST	0x10

/* rsr */
#define	SMK_PRX		0x01
#define	SMK_CRC		0x02
#define	SMK_FAE		0x04
#define	SMK_FO		0x08
#define	SMK_MPA		0x10
#define	SMK_PHY		0x20
#define	SMK_DIS		0x40
#define	SMK_DEF		0x80

/* tsr */
#define	SMK_PTX		0x01
#define	SMK_DFR		0x02
#define	SMK_COL		0x04
#define	SMK_ABT		0x08
#define	SMK_CRS		0x10
#define	SMK_FU		0x20
#define	SMK_CDH		0x40
#define	SMK_OWC		0x80
