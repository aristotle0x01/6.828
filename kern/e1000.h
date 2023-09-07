#ifndef JOS_KERN_E1000_H
#define JOS_KERN_E1000_H

#include <kern/pcireg.h>
#include <kern/pci.h>

// definition ref: https://pdos.csail.mit.edu/6.828/2018/labs/lab6/e1000_hw.h

/* PCI Device IDs */
#define E1000_DEV_ID_82540EM             0x100E

/* Register Set. (82543, 82544)
 *
 * Registers are defined to be 32 bits and  should be accessed as 32 bit values.
 * These registers are physically located on the NIC, but are mapped into the
 * host memory address space.
 *
 * RW - register is both readable and writable
 * RO - register is read only
 * WO - register is write only
 * R/clr - register is read only and is cleared when read
 * A - register array
 */
#define E1000_CTRL     0x00000  /* Device Control - RW */
#define E1000_STATUS   0x00008  /* Device Status - RO */
#define E1000_TDBAL    0x03800  /* TX Descriptor Base Address Low - RW */
#define E1000_TDBAH    0x03804  /* TX Descriptor Base Address High - RW */
#define E1000_TDLEN    0x03808  /* TX Descriptor Length - RW */
#define E1000_TDH      0x03810  /* TX Descriptor Head - RW */
#define E1000_TDT      0x03818  /* TX Descripotr Tail - RW */
#define E1000_TCTL     0x00400  /* TX Control - RW */
#define E1000_TIPG     0x00410  /* TX Inter-packet gap -RW */
#define E1000_RDBAL    0x02800  /* RX Descriptor Base Address Low - RW */
#define E1000_RDBAH    0x02804  /* RX Descriptor Base Address High - RW */
#define E1000_RDLEN    0x02808  /* RX Descriptor Length - RW */
#define E1000_RDH      0x02810  /* RX Descriptor Head - RW */
#define E1000_RDT      0x02818  /* RX Descriptor Tail - RW */
#define E1000_RFCTL    0x05008  /* Receive Filter Control*/
#define E1000_MTA      0x05200  /* Multicast Table Array - RW Array */
#define E1000_RA       0x05400  /* Receive Address - RW Array */
#define E1000_VFTA     0x05600  /* VLAN Filter Table Array - RW Array */
#define E1000_IMS      0x000D0  /* Interrupt Mask Set - RW */
#define E1000_RCTL     0x00100  /* RX Control - RW */

/* Transmit Control */
#define E1000_TCTL_RST    0x00000001    /* software reset */
#define E1000_TCTL_EN     0x00000002    /* enable tx */
#define E1000_TCTL_BCE    0x00000004    /* busy check enable */
#define E1000_TCTL_PSP    0x00000008    /* pad short packets */
#define E1000_TCTL_CT     0x00000ff0    /* collision threshold */
#define E1000_TCTL_COLD   0x003ff000    /* collision distance */
#define E1000_TCTL_SWXOFF 0x00400000    /* SW Xoff transmission */
#define E1000_TCTL_PBE    0x00800000    /* Packet Burst Enable */
#define E1000_TCTL_RTLC   0x01000000    /* Re-transmit on late collision */
#define E1000_TCTL_NRTU   0x02000000    /* No Re-transmit on underrun */
#define E1000_TCTL_MULR   0x10000000    /* Multiple request support */

/* Transmit Descriptor bit definitions */
#define E1000_TXD_DTYP_D     0x00100000 /* Data Descriptor */
#define E1000_TXD_DTYP_C     0x00000000 /* Context Descriptor */
#define E1000_TXD_POPTS_IXSM 0x01       /* Insert IP checksum */
#define E1000_TXD_POPTS_TXSM 0x02       /* Insert TCP/UDP checksum */
#define E1000_TXD_CMD_EOP    0x01000000 /* End of Packet */
#define E1000_TXD_CMD_IFCS   0x02000000 /* Insert FCS (Ethernet CRC) */
#define E1000_TXD_CMD_IC     0x04000000 /* Insert Checksum */
#define E1000_TXD_CMD_RS     0x08000000 /* Report Status */
#define E1000_TXD_CMD_RPS    0x10000000 /* Report Packet Sent */
#define E1000_TXD_CMD_DEXT   0x20000000 /* Descriptor extension (0 = legacy) */
#define E1000_TXD_CMD_VLE    0x40000000 /* Add VLAN tag */
#define E1000_TXD_CMD_IDE    0x80000000 /* Enable Tidv register */
#define E1000_TXD_STAT_DD    0x00000001 /* Descriptor Done */
#define E1000_TXD_STAT_EC    0x00000002 /* Excess Collisions */
#define E1000_TXD_STAT_LC    0x00000004 /* Late Collisions */
#define E1000_TXD_STAT_TU    0x00000008 /* Transmit underrun */
#define E1000_TXD_CMD_TCP    0x01000000 /* TCP packet */
#define E1000_TXD_CMD_IP     0x02000000 /* IP packet */
#define E1000_TXD_CMD_TSE    0x04000000 /* TCP Seg enable */
#define E1000_TXD_STAT_TC    0x00000004 /* Tx Underrun */

/* Interrupt Mask Set */
#define E1000_IMS_RXT0      0x00000080      	/* rx timer intr */
#define E1000_IMS_RXO       0x00000040       	/* rx overrun */
#define E1000_IMS_RXDMT0    0x00000010    		/* rx desc min. threshold */
#define E1000_IMS_RXSEQ     0x00000008     		/* rx sequence error */
#define E1000_IMS_LSC       0x00000004       	/* Link Status Change */

/* Receive Control */
#define E1000_RCTL_RST            0x00000001    /* Software reset */
#define E1000_RCTL_EN             0x00000002    /* enable */
#define E1000_RCTL_SBP            0x00000004    /* store bad packet */
#define E1000_RCTL_UPE            0x00000008    /* unicast promiscuous enable */
#define E1000_RCTL_MPE            0x00000010    /* multicast promiscuous enab */
#define E1000_RCTL_LPE            0x00000020    /* long packet enable */
#define E1000_RCTL_LBM_NO         0x00000000    /* no loopback mode */
#define E1000_RCTL_LBM_MAC        0x00000040    /* MAC loopback mode */
#define E1000_RCTL_LBM_SLP        0x00000080    /* serial link loopback mode */
#define E1000_RCTL_LBM_TCVR       0x000000C0    /* tcvr loopback mode */
#define E1000_RCTL_DTYP_MASK      0x00000C00    /* Descriptor type mask */
#define E1000_RCTL_DTYP_PS        0x00000400    /* Packet Split descriptor */
#define E1000_RCTL_RDMTS_HALF     0x00000000    /* rx desc min threshold size */
#define E1000_RCTL_RDMTS_QUAT     0x00000100    /* rx desc min threshold size */
#define E1000_RCTL_RDMTS_EIGTH    0x00000200    /* rx desc min threshold size */
#define E1000_RCTL_MO_SHIFT       12            /* multicast offset shift */
#define E1000_RCTL_MO_0           0x00000000    /* multicast offset 11:0 */
#define E1000_RCTL_MO_1           0x00001000    /* multicast offset 12:1 */
#define E1000_RCTL_MO_2           0x00002000    /* multicast offset 13:2 */
#define E1000_RCTL_MO_3           0x00003000    /* multicast offset 15:4 */
#define E1000_RCTL_MDR            0x00004000    /* multicast desc ring 0 */
#define E1000_RCTL_BAM            0x00008000    /* broadcast enable */
/* these buffer sizes are valid if E1000_RCTL_BSEX is 0 */
#define E1000_RCTL_SZ_2048        0x00000000    /* rx buffer size 2048 */
#define E1000_RCTL_SZ_1024        0x00010000    /* rx buffer size 1024 */
#define E1000_RCTL_SZ_512         0x00020000    /* rx buffer size 512 */
#define E1000_RCTL_SZ_256         0x00030000    /* rx buffer size 256 */
/* these buffer sizes are valid if E1000_RCTL_BSEX is 1 */
#define E1000_RCTL_SZ_16384       0x00010000    /* rx buffer size 16384 */
#define E1000_RCTL_SZ_8192        0x00020000    /* rx buffer size 8192 */
#define E1000_RCTL_SZ_4096        0x00030000    /* rx buffer size 4096 */
#define E1000_RCTL_VFE            0x00040000    /* vlan filter enable */
#define E1000_RCTL_CFIEN          0x00080000    /* canonical form enable */
#define E1000_RCTL_CFI            0x00100000    /* canonical form indicator */
#define E1000_RCTL_DPF            0x00400000    /* discard pause frames */
#define E1000_RCTL_PMCF           0x00800000    /* pass MAC control frames */
#define E1000_RCTL_BSEX           0x02000000    /* Buffer size extension */
#define E1000_RCTL_SECRC          0x04000000    /* Strip Ethernet CRC */
#define E1000_RCTL_FLXBUF_MASK    0x78000000    /* Flexible buffer size */
#define E1000_RCTL_FLXBUF_SHIFT   27            /* Flexible buffer shift */
/* Receive Address */
#define E1000_RAL     E1000_RA 			/* Receive Address LOW */
#define E1000_RAH     (E1000_RAL+4)  	/* Receive Address HIGH */
#define E1000_RAH_AV  0x80000000        /* Receive descriptor valid */

/* ethernet protocol */
#define MAX_ETHERNET_PACKET_LEN  1518   /* max ethernet packet length in bytes */

#define E1000_REG(pos) (*((volatile uint32_t *)(bar0 + pos)))

struct tx_desc
{
	uint64_t addr;
	uint16_t length;
	uint8_t cso;
	uint8_t cmd;
	uint8_t status;
	uint8_t css;
	uint16_t special;
};
void tx_init(void);
void tx_demo(void);
int32_t tx_send(const char *packet, size_t len);

struct rx_desc
{
	uint64_t addr;
	uint16_t length;
	uint16_t chksum;
	uint8_t status;
	uint8_t errors;
	uint16_t special;
};
void rx_init(void);
#endif  // SOL >= 6