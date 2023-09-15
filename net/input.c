#include "ns.h"
#include <kern/e1000.h>
#include <inc/lib.h>

// map nic rx receive buffer rx_buffer_array below UTOP
#define RX_VA  (UTOP-2*PTSIZE)

void
input(envid_t ns_envid)
{
	binaryname = "ns_input";

	// LAB 6: Your code here:
	// 	- read a packet from the device driver
	//	- send it to the network server
	// Hint: When you IPC a page to the network server, it will be
	// reading from it for a while, so don't immediately receive
	// another packet in to the same physical page.
	cprintf("input rx ring mapping: 0x%08x\n", RX_VA);
	assert(!(uvpd[PDX(RX_VA)]&PTE_P));
	int r = sys_recv_mem_privilege((void *)RX_VA);
	assert(0 == r);

	const union Nsipc *b_nsipc = (union Nsipc *)RX_VA;

	uint32_t ring_index = MAX_RX_DESCRIPTOR;
	physaddr_t ri_addr = PTE_ADDR(uvpt[PGNUM(&ring_index)])|PGOFF(&ring_index);
	while (1) {
		// recv otherwise yield and wait interruption (wakeup by nic_intr)
		ring_index = MAX_RX_DESCRIPTOR;

		r = sys_recv_ether_packet(ri_addr);
		if (r < 0) continue;
		assert(r < MAX_RX_DESCRIPTOR);
		assert(r == ring_index);

		struct jif_pkt *pkt = (struct jif_pkt *)(b_nsipc + r);
		assert(pkt->jp_len <= MAX_ETHERNET_PACKET_LEN);
		
		int old_ref = pageref(pkt);
		ipc_send(ns_envid, NSREQ_INPUT, pkt, PTE_U|PTE_P);
		// don't immediately receive another
		sleep(50); // works
		// not work, why? 
		// while (pageref(pkt) > old_ref) {
		// 	sys_yield(); 
		// }
		sys_recv_tail_update(r);
	}
}
