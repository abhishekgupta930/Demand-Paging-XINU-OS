/* pfint.c - pfint */

#include <conf.h>
#include <kernel.h>
#include <paging.h>
#include <proc.h>

extern fr_map_t g_frame_table[NFRAMES];
extern int page_replace_policy;



/*----------------------------------------------------------------------n
 * pfint - paging fault ISR
 *-------------------------------------------------------------------------
 */
SYSCALL pfint()
{
  

  //kprintf("\nAbhishek 25 : In pfint");
  STATWORD ps;
  disable(ps);
  
  //read the Page Fault Linear Address
  
  unsigned long addr=read_cr2();
  unsigned long page_dir_offset = get_dir_offset(addr);
  unsigned long page_table_offset = get_table_offset(addr);
  //kprintf(" PID:%d page_fault vaddr:%x\n",currpid,addr);

  int bs_num,page_num,res,new_pt;
  
  
  if( res=bsm_lookup(currpid,addr,&bs_num,&page_num)==SYSERR)
	{
           kill(currpid);
           restore(ps);
           return (SYSERR);
	}
  
  //find the pde and pte

  pd_t* pde=(pd_t*)proctab[currpid].pdbr;
  pde+=page_dir_offset;

  // Create Page Table if does not exists
  if(pde->pd_pres==FALSE)
  {
    new_pt=create_page_table(currpid);
    //kprintf("\n Paget Table Created - frame number :  %d \n",new_pt);
    //now this pde is present
    pde->pd_pres=1;
    //update the vpn of this page
    pde->pd_base=new_pt+FRAME0;
    g_frame_table[new_pt].fr_type=FR_TBL;
  }
  
  // Move into page table
  pt_t* pte=(pt_t*)(pde->pd_base<<12);
  pte+=page_table_offset;
  
  //if a page fault occurs, the pte->pres must be 0, we dont need to check
  pte->pt_pres=1;
  int frame_num;
  if(res=get_frm(&frame_num)==SYSERR)
  {
    kill(currpid);
    restore(ps);
    return SYSERR;
  }


  //read the conten from the bs, this only take effect when this page is
  //original existed but been swapped out
  
  read_bs((FRAME0+frame_num)*NBPG,bs_num,page_num);
  
  //update the pte and the frame info since they are now vaild
  
  pte->pt_base=frame_num+FRAME0;
  pte->pt_pres=1;
  g_frame_table[frame_num].fr_status=FRM_MAPPED;
  g_frame_table[frame_num].fr_pid=currpid;
  g_frame_table[frame_num].fr_vpno=(addr>>12);
  g_frame_table[frame_num].fr_refcnt++;
   if ( g_frame_table[frame_num].fr_type==FR_UNDEFINE)
	g_frame_table[frame_num].fr_type=FR_PAGE;
  //this frame is dirty since it's not the same one in the bs
  g_frame_table[frame_num].fr_dirty=1;



 //Insert new frame into page rep-lacement datastructurei
   if ( g_frame_table[frame_num].fr_type==FR_PAGE)
  	replacement_policy_insert(frame_num);
 
  printallframes();

  write_cr3(proctab[currpid].pdbr);
  return OK;


}


