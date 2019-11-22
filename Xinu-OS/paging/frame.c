
/* frame.c - manage physical frames */
#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <paging.h>

/*-------------------------------------------------------------------------
 * init_frm - initialize frm_tab
 *-------------------------------------------------------------------------
 */
fr_map_t g_frame_table[NFRAMES];
SYSCALL init_frm()
{
  
  int i;
  for(i=0;i<NFRAMES;i++)
  {
    g_frame_table[i].fr_status=FRM_UNMAPPED;
    g_frame_table[i].fr_pid=-1;
    g_frame_table[i].fr_vpno=-1;
    g_frame_table[i].fr_refcnt=0;
    g_frame_table[i].fr_type=FR_UNDEFINE;
    g_frame_table[i].fr_dirty=0;
  }

  return OK;
}

/*-------------------------------------------------------------------------
 * get_frm - get a free frame according page replacement policy
 *-------------------------------------------------------------------------
 */
SYSCALL get_frm(int* avail)
{
  int i;
  //kprintf("\nAbhishek 25 : In get_frm");
  //find a free frame to return
  for(i=0;i<NFRAMES;i++)
  {
    if(g_frame_table[i].fr_status==FRM_UNMAPPED)
    {
      *avail=i;
      //kprintf("\nFrame Allocated for PID:%d frame number :%d\n",currpid,i);

      return OK;
    }
  }
 
// Get a frame using frame replacement policy

      *avail = replacement_policy_getframe();



 

  return OK;
}

/*-------------------------------------------------------------------------
 * free_frm - free a frame 
 *-------------------------------------------------------------------------
 */
SYSCALL free_frm(int id)
{


   STATWORD ps;
   disable(ps);


  //kprintf("\n Abhishek 25 : free_frm");
////////////////////////////////////////////////////////////////////

// If deleting Page Directory

 if (g_frame_table[id].fr_type == FR_DIR) {
                        //kprintf("\nfree_frm: freeing FR_DIR at FRAME %d", id);
                        int x;
                        pd_t *pde = frm_addr(id);
                        for (x = 0; x < NBPG/4; x++) {
                                if ((pde->pd_pres == TRUE) && (pde->pd_global != TRUE)) {
                                        free_frm((pde->pd_base - FRAME0));
                                }
                        }
             }



// If deleting Page Table
if (g_frame_table[id].fr_type == FR_TBL){
                       // kprintf("\nfree_frm: freeing FR_TABLE at FRAME %d", id);
 			int x;
                        int frm_id;
                        pt_t *pte;
                        //pt_t *pte = frm_addr(id);
                        for (x = 0; x < NBPG/4; x++) {
				pte = frm_addr(id) + sizeof(pt_t)*x;
                                if (pte->pt_pres == TRUE) {
                                        frm_id = pte->pt_base - FRAME0;
                                        free_frm(frm_id);

					//Remove from Queue as per page replacement policy

                                }


			   }
			}

//If Deleting normal page
if (g_frame_table[id].fr_type == FR_PAGE) {
                       /// kprintf("\nfree_frm: freeing FR_PAGE at FRAME %d", id);

   //If it is a frame we need to find correesponding pagetable and set its present bit to false where this page entry was made 
   //kprintf("\nfree_frm: freeing FR_PAGE at FRAME %d", id);
                        unsigned char * frm_addr = frm_addr(id);

			//Get Virtual Address : First obtain virtual page number and then translate
                        unsigned long vaddr = vpage2vaddr(g_frame_table[id].fr_vpno);
                        int store, pageth;
                        int pid = g_frame_table[id].fr_pid;

                        pd_t *pde = proctab[pid].pdbr;
			unsigned long page_dir_offset = get_dir_offset(vaddr);
                        pde+=page_dir_offset;
 
  			unsigned long page_table_offset = get_table_offset(vaddr);
  			pt_t* pte=(pt_t*)(pde->pd_base<<12);
  			pte+=page_table_offset;

                        if (bsm_lookup(g_frame_table[id].fr_pid, vaddr, &store, &pageth) == SYSERR) {
                                //kprintf("\nfree_frm: bsm_lookup failed for VADDR %x, PID %d", vaddr, frm_tab[id].fr_pid);
                                restore(ps);
                                return SYSERR;
                        }
                        //kprintf("\nfree_frm: write_bs(%x, %d, %d) FRM_ID: %d", frm_addr, store, pageth, id);
                        write_bs(frm_addr, store, pageth);
                        pte->pt_pres = FALSE;
                        pte->pt_write = FALSE;
                        pte->pt_user = FALSE;
                        pte->pt_pwt = FALSE;
                        pte->pt_pcd = FALSE;
                        pte->pt_acc = FALSE;
                        pte->pt_mbz = FALSE;
                        pte->pt_dirty = FALSE;
                        pte->pt_global = FALSE;
                        pte->pt_avail = FALSE;
                        pte->pt_base = FALSE;

		  
 		g_frame_table[id].fr_status = FRM_UNMAPPED;
                g_frame_table[id].fr_pid = NONE;
                g_frame_table[id].fr_vpno = NONE;
                g_frame_table[id].fr_refcnt = 0;
                g_frame_table[id].fr_type = FR_UNDEFINE;
                g_frame_table[id].fr_dirty = FALSE;
                        //kprintf("\nDelete from Queue FRAME %d", id);
			replacement_policy_deleteframe(id);

               restore(ps);
               return OK;

	}

}
