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
	char buf[MAX_ETHERNET_PACKET_LEN];
	int r;
	while (1) {
		r = sys_recv_ether_packet(buf, MAX_ETHERNET_PACKET_LEN);
		if (r < 0) continue;

		nsipcbuf.pkt.jp_len = r;
		memcpy(nsipcbuf.pkt.jp_data, buf, r);
		ipc_send(ns_envid, NSREQ_INPUT, &nsipcbuf, PTE_U|PTE_P|PTE_W);

		// don't immediately receive another
		sleep(50);
	}
}
