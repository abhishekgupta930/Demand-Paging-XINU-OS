/* vfreemem.c - vfreemem */

#include <conf.h>
#include <kernel.h>
#include <mem.h>
#include <paging.h>
#include <proc.h>

extern struct pentry proctab[];
/*------------------------------------------------------------------------
 *  vfreemem  --  free a virtual memory block, returning it to vmemlist
 *------------------------------------------------------------------------
 */
SYSCALL	vfreemem(block, size)
	struct	mblock	*block;
	unsigned size;
{


      //  kprintf("--------------------In vfreemem()-----------------\n");


  	STATWORD ps;
        struct  mblock  *p, *q;
        unsigned top;
 	struct  pentry  *pptr = &proctab[currpid];
        struct mblock *memlist = proctab[currpid].vmemlist;

        size = (unsigned)roundmb(size);
        disable(ps);
        for( p=memlist->mnext,q= &memlist;
             p != (struct mblock *) NULL && p < block ;
             q=p,p=p->mnext )
                ;
        if (((top=q->mlen+(unsigned)q)>(unsigned)block && q!= &memlist) ||
            (p!=NULL && (size+(unsigned)block) > (unsigned)p )) {

                restore(ps);
                return(SYSERR);
        }
        if ( q!= &memlist && top == (unsigned)block )
                        q->mlen += size;
        else {
                block->mlen = size;
                block->mnext = p;
                q->mnext = block;
                q = block;
        }
        if ( (unsigned)( q->mlen + (unsigned)q ) == (unsigned)p) {
                q->mlen += p->mlen;
                q->mnext = p->mnext;
        }


/*
	kprintf("-----------Savita1---------\n");
 // remove frame and page table entry
        pd_t *pde = (pd_t*) (pptr->pdbr);
        unsigned long npages = size / NBPG;
        unsigned long vpno = pptr->vhpno;
        unsigned long pd_offset = vpage2dir_offset(vpno);
        unsigned long pt_frm_id = pde->pd_base - FRAME0;
        //kprintf("\nvfreemem: removing page mappings [vpno: %d] [pd_offset: %d] [pt_frm_id: %d]", vpno, pd_offset, pt_frm_id);
        pde->pd_base = FALSE;
        pde->pd_global = FALSE;
        pde->pd_pres = FALSE;
        free_frm(pt_frm_id);

*/
        restore(ps);
        return(OK);


}
