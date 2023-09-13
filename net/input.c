#include "ns.h"
#include <kern/e1000.h>
#include <inc/lib.h>

extern union Nsipc nsipcbuf;

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
	
	// COW: avoid COW upon first written changing underlying physical page
	memset(&nsipcbuf, 0, sizeof(nsipcbuf));
	// must use PTE_ADDR to zero flags of PTE entry
	physaddr_t phy_addr = PTE_ADDR(uvpt[PGNUM(&nsipcbuf)]);

	int r=0;
	while (1) {
		// recv otherwise yield and wait interruption (wakeup by nic_intr)
		r = sys_recv_ether_packet(phy_addr, sizeof(nsipcbuf));
		if (r <= 0) continue;

		ipc_send(ns_envid, NSREQ_INPUT, &nsipcbuf, PTE_U|PTE_P|PTE_W);

		// don't immediately receive another
		sleep(50);
	}
}
