/* vcreate.c - vcreate */
    
#include <conf.h>
#include <i386.h>
#include <kernel.h>
#include <proc.h>
#include <sem.h>
#include <mem.h>
#include <io.h>
#include <paging.h>

/*
static unsigned long esp;
*/

extern bs_map_t g_back_store_table[BS_NUM];
extern bs_map_t g_proc_bs_t[NPROC][BS_NUM];

LOCAL	newpid();
/*------------------------------------------------------------------------
 *  create  -  create a process to start running a procedure
 *------------------------------------------------------------------------
 */
SYSCALL vcreate(procaddr,ssize,hsize,priority,name,nargs,args)
	int	*procaddr;		/* procedure address		*/
	int	ssize;			/* stack size in words		*/
	int	hsize;			/* virtual heap size in pages	*/
	int	priority;		/* process priority > 0		*/
	char	*name;			/* name (for debugging)		*/
	int	nargs;			/* number of args that follow	*/
	long	args;			/* arguments (treated like an	*/
					/* array in the code)		*/
{

  //kprintf("\n-----------------In vcreate()-----------------------\n");
  //a process with virtual heap should be a original process
  int proc_id=create(procaddr,ssize,priority,name,nargs,args);
  if(proc_id==SYSERR)
    return SYSERR;
  //every heap has a correctsponding backing store
  int res,bsm_num;
  if (res=get_bsm(&bsm_num)== SYSERR)
     return SYSERR;
  
  //all heap of process begins with virtual page number 4096
  //kprintf(" \n Backing Store : %d Process ID  :%d\n",bsm_num,proc_id);

    bsm_tab[bsm_num].bs_status = BSM_MAPPED;
    bsm_tab[bsm_num].bs_pid[proc_id] = BSM_MAPPED;
    bsm_tab[bsm_num].bs_vpno[proc_id] = V_HEAP;
    bsm_tab[bsm_num].bs_private = TRUE;
    bsm_tab[bsm_num].bs_npages = hsize;

    proctab[proc_id].vhpno=V_HEAP;
    proctab[proc_id].vhpnpages=hsize;
    //struct mblock *heap_begin=(struct mblock*)((V_HEAP)<<12);
    struct mblock *heap_begin=(struct mblock*)(vpage2vaddr(V_HEAP));
  //when init, the heap is a whole chunk of free mem
  //the head of the linked list point to the fist free chunk of mem
    proctab[proc_id].vmemlist->mlen=NBPG*hsize;
    proctab[proc_id].vmemlist->mnext=heap_begin;
    return proc_id;


}

/*------------------------------------------------------------------------
 * newpid  --  obtain a new (free) process id
 *------------------------------------------------------------------------
 */
LOCAL	newpid()
{
	int	pid;			/* process id to return		*/
	int	i;

	for (i=0 ; i<NPROC ; i++) {	/* check all NPROC slots	*/
		if ( (pid=nextproc--) <= 0)
			nextproc = NPROC-1;
		if (proctab[pid].pstate == PRFREE)
			return(pid);
	}
	return(SYSERR);
}
