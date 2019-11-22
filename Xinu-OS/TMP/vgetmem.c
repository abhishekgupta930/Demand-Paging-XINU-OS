/* vgetmem.c - vgetmem */

#include <conf.h>
#include <kernel.h>
#include <mem.h>
#include <proc.h>
#include <paging.h>

extern struct pentry proctab[];
/*------------------------------------------------------------------------
 * vgetmem  --  allocate virtual heap storage, returning lowest WORD address
 *------------------------------------------------------------------------
 */


WORD    *vgetmem(nbytes)
        unsigned nbytes;
{
	STATWORD ps;
	disable(ps);
	struct	mblock	*p,*q,*leftover;
  	//kprintf("--------------------In vgetmem()-----------------");
	struct mblock *memlist;
	int vgetmem_failed = TRUE;
	memlist = proctab[currpid].vmemlist;
	int length;
	length = memlist->mlen;
	//kprintf("\n Length = %d, Number of bytes = %d",length,nbytes);
	if (nbytes == 0 || memlist->mnext == (struct mblock *) NULL) {
		restore(ps);
		return((WORD *)SYSERR);
		}
	nbytes = (unsigned int) roundmb(nbytes);
	for (q=memlist,p=memlist->mnext ; p != (struct mblock *) NULL ; q=p,p=p->mnext)
		if ( q->mlen == nbytes) {
			q->mnext = p->mnext;
			//kprintf("\nvgetmem: sucess end");
			restore(ps);
			return (WORD *)p;
			}
		else if ( q->mlen > nbytes ) {
			leftover = (struct mblock *)( ((int*)p) + nbytes );
			q->mnext = leftover;
			leftover->mnext = p->mnext;
			leftover->mlen = q->mlen - nbytes;
			restore(ps);
			return (WORD *)p;
			}
	restore(ps);

	return (WORD *)SYSERR;
}



