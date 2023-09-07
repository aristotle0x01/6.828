#include <kern/e1000.h>
#include <kern/pmap.h>
#include <inc/string.h>
#include <inc/error.h>

// LAB 6: Your driver code here
extern volatile uint8_t *bar0;

#define MAX_TX_DESCRIPTOR 64 /* max num of tx descritor in ring buffer */
struct tx_desc tx_desc_list[MAX_TX_DESCRIPTOR] __attribute__((__aligned__(16)));
uint8_t tx_buffer_array[MAX_TX_DESCRIPTOR][MAX_ETHERNET_PACKET_LEN];
volatile uint32_t *tx_tdt;

#define MAX_RX_DESCRIPTOR 128 /* max num of rx descritor in ring buffer */
#define MAX_RX_BUF_BYTES 2048 /* max num of rx descritor in ring buffer */
struct rx_desc rx_desc_list[MAX_RX_DESCRIPTOR] __attribute__((__aligned__(16)));
uint8_t rx_buffer_array[MAX_RX_DESCRIPTOR][MAX_RX_BUF_BYTES];
volatile uint32_t *rx_rdt;

void
tx_init(void) {
    assert(sizeof(struct tx_desc) == 16);
    assert(sizeof(tx_desc_list)%128 == 0 && sizeof(tx_desc_list) == 16*MAX_TX_DESCRIPTOR);

    E1000_REG(E1000_TDBAL) = PADDR((void *)tx_desc_list);
    E1000_REG(E1000_TDBAH) = 0;
    E1000_REG(E1000_TDLEN) = sizeof(tx_desc_list);
    E1000_REG(E1000_TDH) = 0;
    E1000_REG(E1000_TDT) = 0;
    tx_tdt = (uint32_t *)(bar0+E1000_TDT);

    // Table 13-76. TCTL Register Bit Description
    uint32_t ct = E1000_TCTL_CT&(0x10<<4);
    uint32_t cold = E1000_TCTL_COLD&(0x40<<12);
    E1000_REG(E1000_TCTL) = E1000_TCTL_EN|E1000_TCTL_PSP|ct|cold;

    // Table 13-77. TIPG Register Bit Description: IEEE 802.3 standard
    uint32_t ipgt = 0x03ff&10;
    uint32_t ipgr2 = 0x3ff00000&(0x110<<20);
    uint32_t ipgr1 = 0x0ffc00&(0x100<<10);
    E1000_REG(E1000_TIPG) = ipgt|ipgr1|ipgr2;

    // init tx descriptor array
    for (int i=0; i<MAX_TX_DESCRIPTOR; i++) {
        tx_desc_list[i].cso = 0;
        tx_desc_list[i].css = 0;
        tx_desc_list[i].special = 0;
        tx_desc_list[i].status |= E1000_TXD_STAT_DD;
        tx_desc_list[i].length = 0;
        tx_desc_list[i].cmd = 0;
        tx_desc_list[i].addr = PADDR(tx_buffer_array[i]);  
    }
}

void
tx_demo(void) {
    // send some packets from inside kernel
    int n = MAX_TX_DESCRIPTOR*2;
    char *text = "hello";
    for (int i=0; i<n; i++) {
        uint32_t ti = *tx_tdt;
        if (tx_desc_list[ti].length > 0 && !(tx_desc_list[ti].status&E1000_TXD_STAT_DD)) {
            cprintf("tx queue is full when %d!\n", i);
            return;
        }

        strcpy((char *)tx_buffer_array[ti], text);
        tx_desc_list[ti].length = 6;
        tx_desc_list[ti].status = 0;
        *tx_tdt = (ti + 1)%MAX_TX_DESCRIPTOR;
    }
}

int32_t
tx_send(const char *packet, size_t len) {
    if (packet == NULL || len == 0) return 0;
    if (len > MAX_ETHERNET_PACKET_LEN) return -E_INVAL;

    uint32_t ti = *tx_tdt;
    if (!(tx_desc_list[ti].status&E1000_TXD_STAT_DD)) {
        return -E_TX_QUEUE_FULL;
    }

    memcpy(tx_buffer_array[ti], packet, len);
    tx_desc_list[ti].length = len;
    tx_desc_list[ti].status &= ~E1000_TXD_STAT_DD;
    tx_desc_list[ti].cmd |= (uint8_t)((E1000_TXD_CMD_EOP|E1000_TXD_CMD_RS)>>24);
    *tx_tdt = (ti + 1)%MAX_TX_DESCRIPTOR;

    return 0;
}

void 
rx_init(void) {
    assert(sizeof(struct rx_desc) == 16);
    assert(sizeof(rx_desc_list)%128 == 0 && sizeof(rx_desc_list) == 16*MAX_RX_DESCRIPTOR);

    // init rx descriptor array
    memset(rx_desc_list, 0, sizeof(rx_desc_list));
    for (int i=0; i<MAX_RX_DESCRIPTOR; i++) {
        rx_desc_list[i].addr = PADDR(rx_buffer_array[i]);
    }

    // Receive Address Register(s) (RAL/RAH)
    // hard-code QEMU's default MAC address of 52:54:00:12:34:56
    // 13.5.2 13.5.3
    E1000_REG(E1000_RAL) = 0x12005452;
    E1000_REG(E1000_RAH) = 0x00005634;
    E1000_REG(E1000_RAH) |= E1000_RAH_AV;

    // receive descriptor list
    E1000_REG(E1000_RDBAL) = PADDR((void *)rx_desc_list);
    E1000_REG(E1000_RDBAH) = 0;
    E1000_REG(E1000_RDLEN) = sizeof(rx_desc_list);

    // Receive Descriptor Head and Tail registers
    E1000_REG(E1000_RDH) = 0;
    E1000_REG(E1000_RDT) = MAX_RX_DESCRIPTOR-1;
    rx_rdt = (uint32_t *)(bar0+E1000_RDT);

    // the Receive Control (RCTL) register
    E1000_REG(E1000_RCTL) = E1000_RCTL_EN|E1000_RCTL_SECRC|E1000_RCTL_BAM;
}