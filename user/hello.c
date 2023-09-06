// hello, world
#include <inc/lib.h>

void
umain(int argc, char **argv)
{
	cprintf("hello, world\n");
	cprintf("i am environment %08x\n", thisenv->env_id);
	char *p = "send ethernet packet from hello [user space]";
	int r = sys_send_ether_packet(p, strlen(p));
	cprintf("sys_send_ether_packet: %s, result %d\n", p, r);
}
