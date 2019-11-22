/* bsm.c - manage the backing store mapping*/

#include <conf.h>
#include <kernel.h>
#include <paging.h>
#include <proc.h>

bs_map_t bsm_tab[BS_NUM];

/*-------------------------------------------------------------------------
 * init_bsm- initialize bsm_tab
 *-------------------------------------------------------------------------
 */
SYSCALL init_bsm()
{

  int i;
  for (i=0 ; i<BS_NUM; i++)
    	{
	   bsm_tab[i].bs_status = BSM_UNMAPPED;
	}        
  return OK;

} 

/*-------------------------------------------------------------------------
 * get_bsm - get a free entry from bsm_tab 
 *-------------------------------------------------------------------------
 */



SYSCALL get_bsm(int* avail)
{

	STATWORD ps;
        disable(ps);
        int i;
        *avail = NONE;
        for (i=0 ; i<BS_NUM ; i++) {
                if (bsm_tab[i].bs_status == BSM_UNMAPPED) {
                        *avail = i;
                        break;
                }
        }
        restore(ps);
        return OK;

}


/*-------------------------------------------------------------------------
 * bsm_lookup - lookup bsm_tab and find the corresponding entry
 *-------------------------------------------------------------------------
 */


SYSCALL bsm_lookup(int pid, long vaddr, int* store, int* pageth)
{

        STATWORD ps;
        disable(ps);

        int i;
        vaddr = vaddr >> 12;
        for (i = 0; i < BS_NUM; i++) {
                if (    (bsm_tab[i].bs_status == BSM_MAPPED) &&
                        (bsm_tab[i].bs_pid[pid] == BSM_MAPPED) &&
                        (bsm_tab[i].bs_vpno[pid] <= vaddr) &&
                        ((bsm_tab[i].bs_vpno[pid] + bsm_tab[i].bs_npages) > vaddr))  {
                                //kprintf("\nbsm_lookup: VADDR/4096: %d, bs_vpno: %d, bs_npages: %d", vaddr, bsm_tab[i].bs_vpno[pid], bsm_tab[i].bs_npages[pid]);
                                //kprintf("\nbsm_lookup: store %d", i);
                                *store = i;
                                *pageth = vaddr - bsm_tab[i].bs_vpno[pid];
                                //*pageth = vaddr - bsm_tab[i].bs_vpno[pid]+bsm_tab[i].bs_offset[pid];
                                return OK;
                }
        }
        restore(ps);
        return SYSERR;








}


/*-------------------------------------------------------------------------
 * bsm_map - add an mapping into bsm_tab 
 *-------------------------------------------------------------------------
 */


SYSCALL bsm_map(int pid, int vpno, int source, int npages)
{

 if  (bsm_tab[source].bs_status = BSM_UNMAPPED)
	return SYSERR;
 else 
   { 
    bsm_tab[source].bs_status = BSM_MAPPED;
    bsm_tab[source].bs_pid[pid] = BSM_MAPPED;
    bsm_tab[source].bs_vpno[pid] = V_HEAP;
    bsm_tab[source].bs_private = TRUE;
    bsm_tab[source].bs_npages = npages;
    return OK;
   }

}


/*-------------------------------------------------------------------------
 * bsm_unmap - delete an mapping from bsm_tab
 *-------------------------------------------------------------------------
 */


SYSCALL bsm_unmap(int pid, int vpno, int flag)
{


  int bs_num,page_num;
  //find this mapping
  int res=bsm_lookup(pid,vpno<<12,&bs_num,&page_num);
  if(res==SYSERR)
  {
    return SYSERR;
  }

  //save the data to the backing store
  int length=bsm_tab[bs_num].bs_pages[pid];
  int i;
  //use the info of this mapping to save mem to backing store
  for(i=0;i<length;i++)
    write_bs((vpno+i)<<12,bs_num,(page_num)+i);


  bsm_tab[bs_num].bs_vpno[pid] = NONE;
  bsm_tab[bs_num].bs_pages[pid] = NONE;

}


