#include <kern/e1000.h>
#include <kern/pmap.h>
#include <inc/string.h>
#include <inc/error.h>
#include <inc/env.h>
#include <inc/ns.h>
#include <inc/trap.h>
#include <kern/picirq.h>
#include <kern/env.h>

// LAB 6: Your driver code here

extern volatile uint8_t *e1000_bar0;

#define MAX_TX_DESCRIPTOR 64 /* max num of tx descritor in ring buffer */
struct tx_desc tx_desc_list[MAX_TX_DESCRIPTOR] __attribute__((__aligned__(16)));
uint8_t tx_buffer_array[MAX_TX_DESCRIPTOR][MAX_ETHERNET_PACKET_LEN];
volatile uint32_t *e1000_tdt; // tx ring tail

struct rx_desc rx_desc_list[MAX_RX_DESCRIPTOR] __attribute__((__aligned__(16)));
/**
 * Challenge! Modify your E1000 driver to be "zero copy."
 * mapping the kernel-allocated structures into user space 
 * thus make MAX_RX_BUF_BYTES == PGSIZE to facilitate calculation
 * 
 * 1) net/input ask for user access to this area of memory
 * 2) pass *e1000_rdt to net/input to calc memory mapping
 * 3) after usage net/input update *e1000_rdt
 * note: potential pitfall in blocking nic receiving if *e1000_rdt
 *       stayed long in user program
 */
__attribute__((__aligned__(PGSIZE)))
uint8_t rx_buffer_array[MAX_RX_DESCRIPTOR][MAX_RX_BUF_BYTES];
volatile uint32_t *e1000_rdt; // rx ring tail

int
pci_nic_attach(struct pci_func *pcif)
{
	pci_func_enable(pcif);

	e1000_bar0 = mmio_map_region(pcif->reg_base[0], pcif->reg_size[0]);
	cprintf("  mem mapping phy:0x%08x->va:0x%08x %d bytes\n", pcif->reg_base[0], e1000_bar0, pcif->reg_size[0]);

	cprintf("  device status register: 0x%08x\n", e1000_reg(E1000_STATUS));

	// nic initialization
	tx_init();
	// tx_demo();
	rx_init();

    assert(pcif->irq_line == IRQ_NIC);
    irq_setmask_8259A(irq_mask_8259A & ~(1<<IRQ_NIC));

	return 1;
}

void
tx_init(void) {
    assert(sizeof(struct tx_desc) == 16);
    assert(sizeof(tx_desc_list)%128 == 0 && sizeof(tx_desc_list) == 16*MAX_TX_DESCRIPTOR);

    e1000_reg(E1000_TDBAL) = PADDR((void *)tx_desc_list);
    e1000_reg(E1000_TDBAH) = 0;
    e1000_reg(E1000_TDLEN) = sizeof(tx_desc_list);
    e1000_reg(E1000_TDH) = 0;
    e1000_reg(E1000_TDT) = 0;
    e1000_tdt = (uint32_t *)(e1000_bar0+E1000_TDT);

    // Table 13-76. TCTL Register Bit Description
    uint32_t ct = E1000_TCTL_CT&(0x10<<4);
    uint32_t cold = E1000_TCTL_COLD&(0x40<<12);
    e1000_reg(E1000_TCTL) = E1000_TCTL_EN|E1000_TCTL_PSP|ct|cold;

    // Table 13-77. TIPG Register Bit Description: IEEE 802.3 standard
    uint32_t ipgt = 0x03ff&10;
    uint32_t ipgr2 = 0x3ff00000&(0x110<<20);
    uint32_t ipgr1 = 0x0ffc00&(0x100<<10);
    e1000_reg(E1000_TIPG) = ipgt|ipgr1|ipgr2;

    // init tx descriptor array
    memset(tx_desc_list, 0, sizeof(tx_desc_list));
    for (int i=0; i<MAX_TX_DESCRIPTOR; i++) {
        tx_desc_list[i].status |= E1000_TXD_STAT_DD;
        tx_desc_list[i].addr = PADDR(tx_buffer_array[i]);  
    }
}

void
tx_demo(void) {
    // send some packets from inside kernel
    int n = MAX_TX_DESCRIPTOR*2;
    char *text = "hello";
    for (int i=0; i<n; i++) {
        uint32_t ti = *e1000_tdt;
        if (tx_desc_list[ti].length > 0 && !(tx_desc_list[ti].status&E1000_TXD_STAT_DD)) {
            cprintf("tx queue is full when %d!\n", i);
            return;
        }

        strcpy((char *)tx_buffer_array[ti], text);
        tx_desc_list[ti].length = 6;
        tx_desc_list[ti].status = 0;
        *e1000_tdt = (ti + 1)%MAX_TX_DESCRIPTOR;
    }
}

int32_t
tx_send(const char *packet, size_t len) {
    if (packet == NULL || len == 0) return 0;
    if (len > MAX_ETHERNET_PACKET_LEN) return -E_INVAL;

    uint32_t ti = *e1000_tdt;
    if (!(tx_desc_list[ti].status&E1000_TXD_STAT_DD)) {
        return -E_TX_QUEUE_FULL;
    }

    memcpy(tx_buffer_array[ti], packet, len);
    tx_desc_list[ti].length = len;
    tx_desc_list[ti].status &= ~E1000_TXD_STAT_DD;
    tx_desc_list[ti].cmd |= (uint8_t)((E1000_TXD_CMD_EOP|E1000_TXD_CMD_RS)>>24);
    *e1000_tdt = (ti + 1)%MAX_TX_DESCRIPTOR;

    return 0;
}

void 
rx_init(void) {
    assert(sizeof(struct rx_desc) == 16);
    assert(sizeof(rx_desc_list)%128 == 0 && sizeof(rx_desc_list) == 16*MAX_RX_DESCRIPTOR);

    // init rx descriptor array
    memset(rx_desc_list, 0, sizeof(rx_desc_list));
    struct jif_pkt dumb;
    for (int i=0; i<MAX_RX_DESCRIPTOR; i++) {
        // left space for zero copy length setting, otherwise
        // it would be hard to do direct mapping in user space for nsipcbuf.pkt
        rx_desc_list[i].addr = PADDR(rx_buffer_array[i]+sizeof(dumb.jp_len));
    }

    // Receive Address Register(s) (RAL/RAH)
    // hard-code QEMU's default MAC address of 52:54:00:12:34:56
    // 13.5.2 13.5.3
    e1000_reg(E1000_RAL) = 0x12005452;
    e1000_reg(E1000_RAH) = 0x00005634;
    e1000_reg(E1000_RAH) |= E1000_RAH_AV;

    // receive descriptor list
    e1000_reg(E1000_RDBAL) = PADDR((void *)rx_desc_list);
    e1000_reg(E1000_RDBAH) = 0;
    e1000_reg(E1000_RDLEN) = sizeof(rx_desc_list);

    // Receive Descriptor Head and Tail registers
    e1000_reg(E1000_RDH) = 0;
    e1000_reg(E1000_RDT) = MAX_RX_DESCRIPTOR-1;
    e1000_rdt = (uint32_t *)(e1000_bar0+E1000_RDT);

    e1000_reg(E1000_RDTR) = 0;
    e1000_reg(E1000_IMS) = E1000_IMS_RXT0;
    
    // the Receive Control (RCTL) register
    e1000_reg(E1000_RCTL) = E1000_RCTL_EN|E1000_RCTL_SECRC|E1000_RCTL_BAM;
}

int32_t
rx_recv(physaddr_t ring_index) {
    // 3.2.6 Receive Descriptor Queue Structure
    // by: make E1000_DEBUG=TX,TXERR,RX,RXERR,RXFILTER run-net_testinput-nox
    // there would be "e1000: RCTL: 127, mac_reg[RCTL] = 0x4008002" in console output

    // above has implications, since "e1000_reg(E1000_RDT)=MAX_RX_DESCRIPTOR-1",
    // which means *e1000_rdt initially would be 127, and "beyond" makes it never
    // contains a slot with E1000_RXD_STAT_DD, so the first ri would be 0, and
    // *e1000_rdt should be updated to ri but not ri+1, so there will always a slot 
    // wasted as tail mark

    int r = -E_RX_QUEUE_EMPTY;

    uint32_t *p_rindex = KADDR(ring_index);
    *p_rindex = MAX_RX_DESCRIPTOR;

    uint32_t ri = (*e1000_rdt + 1)%MAX_RX_DESCRIPTOR;
    uint32_t old_rdt = *e1000_rdt;
    while (ri != old_rdt) {
        if (!(rx_desc_list[ri].status & E1000_RXD_STAT_DD))
            goto end;

        if (0 == rx_desc_list[ri].length) {
            *e1000_rdt = ri;
            rx_desc_list[ri].status = 0;
            ri = (ri + 1)%MAX_RX_DESCRIPTOR;
            continue;
        }

        int *plen = (int *)rx_buffer_array[ri];
        *plen = rx_desc_list[ri].length;
        *p_rindex = ri;
        r = ri;
        goto end;
    }
    
end:
    e1000_reg(E1000_IMC) = E1000_IMC_RXT0;
    e1000_reg(E1000_IMS) = E1000_IMS_RXT0;
    return r;
}

int32_t
rx_tail_update(uint32_t tail) {
    // If software statically allocates buffers, and uses memory read 
    // to check for completed descriptors, it simply has to zero the 
    // status byte in the descriptor to make it ready for reuse by 
    // hardware. This is not a hardware requirement (moving the hardware 
    // tail pointer is), but is necessary for performing an in–memory scan.
    rx_desc_list[tail].status = 0;
    // This register holds a value that is an offset from the base, 
    // and identifies the location beyond the last descriptor hardware 
    // can process. Note that tail should still point to an area in the 
    // descriptor ring (somewhere between RDBA and RDBA + RDLEN). 
    // This is because tail points to the location where software writes 
    // the first new descriptor.
    *e1000_rdt = tail;

    return 0;
}

// ref: https://github.com/xingdl2007/6.828-2017/blob/master/kern/e1000.c
void nic_intr() {
    // reading this register implicitly acknowledges any pending interrupt events
	int icr = e1000_reg(E1000_ICR);

	for (int i = 0; i < NENV; i++) {
        bool b = envs[i].env_ipc_ether_recv && envs[i].env_status == ENV_NOT_RUNNABLE;
        if (!b) continue;

        int32_t r = rx_recv(envs[i].env_ipc_ether_addr);
        envs[i].env_ipc_ether_recv = false;
        envs[i].env_ipc_ether_addr = 0;
        envs[i].env_tf.tf_regs.reg_eax = r;
        envs[i].env_status = ENV_RUNNABLE;
        return;
    }
}