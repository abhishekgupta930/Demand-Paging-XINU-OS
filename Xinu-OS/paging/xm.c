/* xm.c = xmmap xmunmap */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <paging.h>



/*-------------------------------------------------------------------------
 *  * xmmap - xmmap
 *   *-------------------------------------------------------------------------
 *    */
SYSCALL xmmap(int virtpage, bsd_t store, int npages)
{

      STATWORD ps;
      disable(ps);

        //kprintf("\nxmmap: store: %d, npages: %d mapping to vpno %d for pid %d", store, npages, virtpage, currpid );
        if (    (virtpage >= VIRTUAL_MEMORY_BASE) && \
                (bsm_tab[store].bs_status == BSM_MAPPED) && \
                (bsm_tab[store].bs_pid[currpid] == BSM_MAPPED) && \
                (npages > 0) && \
                (npages <= bsm_tab[store].bs_npages)) {
                        bsm_tab[store].bs_vpno[currpid] = virtpage;
                        bsm_tab[store].bs_pages[currpid] = npages;
			 
                        //kprintf("\nxmmap: good call");
                        restore(ps);
                        return OK;
        }


        else {
                /*
                if (virtpage >= VIRTUAL_MEMORY_BASE / NBPG)
                        kprintf("\nxmmap: 1");
                if (bsm_tab[store].bs_status == BSM_MAPPED)
                        kprintf("\nxmmap: 2");
                if (bsm_tab[store].bs_pid[currpid] == BSM_MAPPED)
                        kprintf("\nxmmap: 3");
                if (npages > 0)
                        kprintf("\nxmmap: 4");
                if (npages <= bsm_tab[store].bs_npages)
                        kprintf("\nxmmap: 5");
                */
                //kprintf("\nxmmap: bad call");
                restore(ps);
                return SYSERR;
        }


}





/*-------------------------------------------------------------------------
 * xmunmap - xmunmap
 *-------------------------------------------------------------------------
 */
SYSCALL xmunmap(int virtpage)
{
   //Free all the pagetables and pages associated with those pages 

 // kprintf("PID:%d xmunmap virtpage:%x\n",currpid,virtpage);

   int i,pt_frm_id;
   //Get Base Directory
   pd_t *pde = (pd_t*) (proctab[currpid].pdbr);

   //Get Directory offset
   unsigned long page_dir_offset = virtpage >> 10;
   pde+=page_dir_offset;


   for (i = 0; i < BS_NUM; i++) {
                if ((bsm_tab[i].bs_status == BSM_MAPPED) && (bsm_tab[i].bs_pid[currpid] == BSM_MAPPED)
			 && (bsm_tab[i].bs_vpno[currpid] == virtpage)) {
                         if (pde->pd_pres == TRUE) {
                                pt_frm_id = pde->pd_base - FRAME0;
                                //kprintf("\nxmunmap: removing page table : %d",pt_frm_id);
                                free_frm(pt_frm_id);
                                //if (free_frm(pt_frm_id) == SYSERR)
                                //      kprintf("\nxmunmap: free_frm(%d) failed", pt_frm_id);
                                pde->pd_base = FALSE;
                                pde->pd_global = FALSE;
                                pde->pd_pres = FALSE;
                        }
                        bsm_tab[i].bs_vpno[currpid] = NONE;
		    }
             }


  //Unmap from BSM
  //int res=bsm_unmap(currpid,virtpage,-1);


}
