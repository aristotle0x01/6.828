#ifndef JOS_KERN_E1000_H
#define JOS_KERN_E1000_H

#include <kern/pcireg.h>
#include <kern/pci.h>

// definition ref: https://pdos.csail.mit.edu/6.828/2018/labs/lab6/e1000_hw.h

#define E1000_VENDOR_ID             0x8086

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
#define E1000_RA       0x05400  /* Receive Address - RW Array */
#define E1000_ICR      0x000C0  /* Interrupt Cause Read - R/clr */
#define E1000_IMS      0x000D0  /* Interrupt Mask Set - RW */
#define E1000_IMC      0x000D8  /* Interrupt Mask Clear - WO */
#define E1000_RCTL     0x00100  /* RX Control - RW */

/* Transmit Control */
#define E1000_TCTL_EN     0x00000002    /* enable tx */
#define E1000_TCTL_PSP    0x00000008    /* pad short packets */
#define E1000_TCTL_CT     0x00000ff0    /* collision threshold */
#define E1000_TCTL_COLD   0x003ff000    /* collision distance */

/* Transmit Descriptor bit definitions */
#define E1000_TXD_CMD_EOP    0x01000000 /* End of Packet */
#define E1000_TXD_CMD_RS     0x08000000 /* Report Status */
#define E1000_TXD_CMD_RPS    0x10000000 /* Report Packet Sent */
#define E1000_TXD_CMD_DEXT   0x20000000 /* Descriptor extension (0 = legacy) */
#define E1000_TXD_CMD_IDE    0x80000000 /* Enable Tidv register */
#define E1000_TXD_STAT_DD    0x00000001 /* Descriptor Done */

/* Interrupt Mask Set */
#define E1000_IMS_RXT0      0x00000080      	/* rx timer intr */
#define E1000_IMS_RXO       0x00000040       	/* rx overrun */
#define E1000_IMS_RXDMT0    0x00000010    		/* rx desc min. threshold */
#define E1000_IMS_RXSEQ     0x00000008     		/* rx sequence error */
#define E1000_IMS_LSC       0x00000004       	/* Link Status Change */

#define E1000_RDTR     		0x02820       	/* RX Delay Timer - RW */
#define E1000_IMC_RXT0      0x00000080      /* rx timer intr */

/* Receive Control */
#define E1000_RCTL_RST            0x00000001    /* Software reset */
#define E1000_RCTL_EN             0x00000002    /* enable */
#define E1000_RCTL_BAM            0x00008000    /* broadcast enable */
/* these buffer sizes are valid if E1000_RCTL_BSEX is 0 */
#define E1000_RCTL_SZ_2048        0x00000000    /* rx buffer size 2048 */
/* these buffer sizes are valid if E1000_RCTL_BSEX is 1 */
#define E1000_RCTL_BSEX           0x02000000    /* Buffer size extension */
#define E1000_RCTL_SECRC          0x04000000    /* Strip Ethernet CRC */
/* Receive Address */
#define E1000_RAL     E1000_RA 			/* Receive Address LOW */
#define E1000_RAH     (E1000_RAL+4)  	/* Receive Address HIGH */
#define E1000_RAH_AV  0x80000000        /* Receive descriptor valid */
/* Receive Descriptor bit definitions */
#define E1000_RXD_STAT_DD       0x01    /* Descriptor Done */
#define E1000_RXD_STAT_EOP      0x02    /* End of Packet */

/* ethernet protocol */
#define MAX_ETHERNET_PACKET_LEN  1518   /* max ethernet packet length in bytes */

#define e1000_reg(pos) (*((volatile uint32_t *)(e1000_bar0 + pos)))

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

struct rx_desc
{
	uint64_t addr;
	uint16_t length;
	uint16_t chksum;
	uint8_t status;
	uint8_t errors;
	uint16_t special;
};

int pci_nic_attach(struct pci_func *pcif);

void tx_init(void);
void rx_init(void);

void tx_demo(void);
int32_t tx_send(const char *packet, size_t len);
int32_t rx_recv(physaddr_t packet, size_t len);

void nic_intr();
#endif  // SOL >= 6