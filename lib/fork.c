// implement fork from user space

#include <inc/string.h>
#include <inc/lib.h>

// PTE_COW marks copy-on-write page table entries.
// It is one of the bits explicitly allocated to user processes (PTE_AVAIL).
#define PTE_COW		0x800

extern volatile pte_t uvpt[];
extern volatile pde_t uvpd[]; 

//
// Custom page fault handler - if faulting page is copy-on-write,
// map in our own private writable copy.
//
static void
pgfault(struct UTrapframe *utf)
{
	void *addr = (void *) utf->utf_fault_va;
	uint32_t err = utf->utf_err;
	int r;

	// Check that the faulting access was (1) a write, and (2) to a
	// copy-on-write page.  If not, panic.
	// Hint:
	//   Use the read-only page table mappings at uvpt
	//   (see <inc/memlayout.h>).

	// LAB 4: Your code here.
	if (!((err & FEC_WR) && (uvpt[PGNUM(addr)] & PTE_COW))) {
		panic("[%08x] not a write err %d, or copy-on-write %08x\n", thisenv->env_id, err, addr);
	}

	// Allocate a new page, map it at a temporary location (PFTEMP),
	// copy the data from the old page to the new page, then move the new
	// page to the old page's address.
	// Hint:
	//   You should make three system calls.

	// LAB 4: Your code here.
	if ((r = sys_page_alloc(0, (void *)PFTEMP, PTE_P|PTE_U|PTE_W)) < 0){
		panic("pgfault sys_page_alloc: %e", r);
	}
	memmove(PFTEMP, ROUNDDOWN(addr, PGSIZE), PGSIZE);

	if ((r = sys_page_map(0, PFTEMP, 0, ROUNDDOWN(addr, PGSIZE), PTE_P|PTE_U|PTE_W)) < 0){
		panic("pgfault sys_page_map: %e", r);
	}
	if ((r = sys_page_unmap(0, PFTEMP)) < 0){
		panic("pgfault sys_page_unmap: %e", r);
	}
}

//
// Map our virtual page pn (address pn*PGSIZE) into the target envid
// at the same virtual address.  If the page is writable or copy-on-write,
// the new mapping must be created copy-on-write, and then our mapping must be
// marked copy-on-write as well.  (Exercise: Why do we need to mark ours
// copy-on-write again if it was already copy-on-write at the beginning of
// this function?)
//
// Returns: 0 on success, < 0 on error.
// It is also OK to panic on error.
//
static int
duppage(envid_t envid, unsigned pn)
{
	// LAB 4: Your code here.
	int perm = PTE_COW|PTE_U|PTE_P;
	void *va = (void *)(pn*PGSIZE);

	int r;
	pte_t pte = uvpt[pn];
	if ((pte & PTE_W) || (pte & PTE_COW)) {
		if ((r = sys_page_map(0, va, envid, va, perm))) {
			panic("duppage sys_page_map error %e\n", r);
		}
		if ((r = sys_page_map(0, va, 0, va, perm))) {
			panic("duppage sys_page_map error old %e\n", r);
		}
	} else {
		if ((r = sys_page_map(0, va, envid, va, pte & 0xfff))) {
			panic("duppage sys_page_map error %e\n", r);
		}
	}
	
	return 0;
}

//
// User-level fork with copy-on-write.
// Set up our page fault handler appropriately.
// Create a child.
// Copy our address space and page fault handler setup to the child.
// Then mark the child as runnable and return.
//
// Returns: child's envid to the parent, 0 to the child, < 0 on error.
// It is also OK to panic on error.
//
// Hint:
//   Use uvpd, uvpt, and duppage.
//   Remember to fix "thisenv" in the child process.
//   Neither user exception stack should ever be marked copy-on-write,
//   so you must allocate a new page for the child's user exception stack.
//
envid_t
fork(void)
{
	// LAB 4: Your code here.
	set_pgfault_handler(pgfault);

	envid_t evnid = sys_exofork();
	if (evnid < 0) {
		panic("user fork error %e\n", evnid);
	} 
	if (evnid == 0) {
		// child
		thisenv = &envs[ENVX(sys_getenvid())];
		return evnid;
	}

	void *va;
	int pn, r;
	for (int di=0; di < PDX(UTOP); di++) {
		pde_t pde_entry = uvpd[di];
		if (!(pde_entry & PTE_P)) {
			continue;
		}

		for (int ti=0; ti<NPTENTRIES; ti++) {
			pn = (di << 10) | ti;
			if (pn == PGNUM(UXSTACKTOP-PGSIZE)) {
				continue;
			}

			if (!(uvpt[pn] & PTE_P)) {
				continue;
			}

			duppage(evnid, pn);
		}
	}

	if ((r = sys_page_alloc(evnid, (void *)(UXSTACKTOP-PGSIZE), PTE_P | PTE_U | PTE_W))) {
		panic("user fork sys_page_alloc error %e\n", r);
	}
	if ((r = sys_env_set_pgfault_upcall(evnid, (void *)(thisenv->env_pgfault_upcall)))) {
		panic("user fork upcall error %e\n", r);
	}
	if ((r = sys_env_set_status(evnid, ENV_RUNNABLE))) {
		panic("user fork status error %e\n", r);
	}
	return evnid;
}

// Challenge!
int
sfork(void)
{
	panic("sfork not implemented");
	return -E_INVAL;
}
