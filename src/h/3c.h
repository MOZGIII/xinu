/* 3c.h - 3C503 controller definitions */

#define EC1VEC  0x0d                /* vector */
#define EC1PORT 0x300               /* port */
#define EC1SMB  "0xd800"            /* shared memory base segment */

extern unsigned char inportb();

#define	ADDROM(base)	0x00+(base)
#define MSR(base)	0x00+(base)
#define	MSK_RD2		0x20
#define SM_BASE     0xd800          /* default shared mem base */

/* 3c503 Gate Array */

#define GA (EC1PORT + 0x400)        /* address of Gate Array */
#define NS EC1PORT                  /* address of NS8390 */

/* Shared memory management parameters */

#define EL2SM_START_PG  (0x20)      /* First page of TX buffer */
#define EL2SM_STOP_PG   (0x40)      /* Last page +1 of RX ring */

/* 3c503 Gate Array Registers */
#define PSTR  (GA+0)                /* Page Start must equal PSTART*/
#define PSPR  (GA+1)                /* Page stop must equal PSTOP */
#define DRQTR (GA+2)                /* DMA Timer Register */
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
#define	CMDR(base)	0x10+(base)
#define	CLDA0(base)	0x11+(base)
#define	PSTART(base)	0x11+(base)
#define	CLDA1(base)	0x12+(base)
#define	PSTOP(base)	0x12+(base)
#define	BNRY(base)	0x13+(base)
#define	TSR(base)	0x14+(base)
#define	TPSR(base)	0x14+(base)
#define	NCR(base)	0x15+(base)
#define	TBCR0(base)	0x15+(base)
#define	FIFO(base)	0x16+(base)
#define	TBCR1(base)	0x16+(base)
#define	ISR(base)	0x17+(base)
#define	CRDA0(base)	0x18+(base)
#define	RSAR0(base)	0x18+(base)
#define	CRDA1(base)	0x19+(base)
#define	RSAR1(base)	0x19+(base)
#define	RBCR0(base)	0x1a+(base)
#define	RBCR1(base)	0x1b+(base)
#define	RSR(base)	0x1c+(base)
#define	RCR(base)	0x1c+(base)
#define	CNTR0(base)	0x1d+(base)
#define	TCR(base)	0x1d+(base)
#define	CNTR1(base)	0x1e+(base)
#define	DCR(base)	0x1e+(base)
#define	CNTR2(base)	0x1f+(base)
#define	IMR(base)	0x1f+(base)

/* 8390 LAN Controller (page 1) */

#define	PAR0(base)	0x11+(base)
#define	PAR1(base)	0x12+(base)
#define	PAR2(base)	0x13+(base)
#define	PAR3(base)	0x14+(base)
#define	PAR4(base)	0x15+(base)
#define	PAR5(base)	0x16+(base)
#define	CURR(base)	0x17+(base)
#define	MAR0(base)	0x18+(base)
#define	MAR1(base)	0x19+(base)
#define	MAR2(base)	0x1a+(base)
#define	MAR3(base)	0x1b+(base)
#define	MAR4(base)	0x1c+(base)
#define	MAR5(base)	0x1d+(base)
#define	MAR6(base)	0x1e+(base)
#define	MAR7(base)	0x1f+(base)

#define	STRT_PG		6
#define	STOP_PG		32

/* msr */
#define	MSK_RESET	0x80
#define	MSK_ENASH	0x40
#define	MSK_DECOD	0x3f

/* cmsr */
#define	MSK_STP		(0x01 | MSK_RD2)
#define	MSK_STA		(0x02 | MSK_RD2)
#define	MSK_TXP		(0x04 | MSK_RD2)
#define	MSK_PG0		(0x00 | MSK_RD2)
#define	MSK_PG1		(0x40 | MSK_RD2)

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
