#include <kern/e1000.h>
#include <kern/pmap.h>
#include <inc/string.h>

// LAB 6: Your driver code here
#define MAX_TX_DESCRIPTOR 64 /* max num of tx descritor in ring buffer */

__attribute__((__aligned__(16)))
struct tx_desc tx_desc_list[MAX_TX_DESCRIPTOR];

uint8_t tx_buffer_array[MAX_TX_DESCRIPTOR][MAX_ETHERNET_PACKET_LEN];

extern volatile uint8_t *bar0;
volatile uint32_t *tx_tdt;

void
tx_init(void) {
    assert(sizeof(struct tx_desc) == 16);
    assert(sizeof(tx_desc_list)%128 == 0 && sizeof(tx_desc_list) == 16*MAX_TX_DESCRIPTOR);

    *(uint32_t *)(bar0+E1000_TDBAL) = PADDR((void *)tx_desc_list);
    *(uint32_t *)(bar0+E1000_TDBAH) = 0;
    *(uint32_t *)(bar0+E1000_TDLEN) = sizeof(tx_desc_list);
    *(uint32_t *)(bar0+E1000_TDH) = 0;
    *(uint32_t *)(bar0+E1000_TDT) = 0;
    tx_tdt = (uint32_t *)(bar0+E1000_TDT);

    // Table 13-76. TCTL Register Bit Description
    uint32_t ct = E1000_TCTL_CT&(0x10<<4);
    uint32_t cold = E1000_TCTL_COLD&(0x40<<12);
    *(uint32_t *)(bar0+E1000_TCTL) = E1000_TCTL_EN|E1000_TCTL_PSP|ct|cold;

    // Table 13-77. TIPG Register Bit Description: IEEE 802.3 standard
    uint32_t ipgt = 0x03ff&10;
    uint32_t ipgr2 = 0x3ff00000&(0x110<<20);
    uint32_t ipgr1 = 0x0ffc00&(0x100<<10);
    *(uint32_t *)(bar0+E1000_TIPG) = ipgt|ipgr1|ipgr2;

    // init tx descriptor array
    for (int i=0; i<MAX_TX_DESCRIPTOR; i++) {
        tx_desc_list[i].cso = 0;
        tx_desc_list[i].css = 0;
        tx_desc_list[i].special = 0;
        tx_desc_list[i].status = 0;

        tx_desc_list[i].addr = PADDR(tx_buffer_array[i]);
        tx_desc_list[i].length = 0;
        
        tx_desc_list[i].cmd = (uint8_t)((E1000_TXD_CMD_IDE|E1000_TXD_CMD_EOP|E1000_TXD_CMD_RS)>>24);
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