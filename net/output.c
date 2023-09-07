#include "ns.h"

extern union Nsipc nsipcbuf;

void
output(envid_t ns_envid)
{
	binaryname = "ns_output";

	// LAB 6: Your code here:
	// 	- read a packet from the network server
	//	- send the packet to the device driver
	while (1)
	{
		envid_t whom;
		int32_t r = ipc_recv(&whom, (void *)&nsipcbuf, 0);
		if (r != NSREQ_OUTPUT) {
			cprintf("output req type error: %d\n", r);
			continue;
		}
		if (whom == 0) {
			cprintf("output ipc_recv: %e\n", r);
			continue;
		}
		assert(ns_envid == whom);
		
		int count = 0;
		while (count < 10) {
			r = sys_send_ether_packet(nsipcbuf.pkt.jp_data, nsipcbuf.pkt.jp_len);
			if (r == 0)	break;

			cprintf("output sys_send_ether_packet: %e\n", r);
			if (r == E_TX_QUEUE_FULL) count++;
			else break;
		}
		if (count == 10) cprintf("output send exceed 10 times\n", r);
	}
}
