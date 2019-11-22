#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <paging.h>

extern struct fr_queue_node* fr_queue_head;
extern struct fr_queue_node* fr_queue_now;

pt_t* g_general_page_table[4];
int create_page_table(int pro_id);
int init_general_page_table();
int create_page_dir(int pro_id);
extern fr_map_t g_frame_table[NFRAMES];

int create_page_dir(int pro_id)
{
  int frame_num,res;
  if(res=get_frm(&frame_num)!=OK)
    return SYSERR;

  //kprintf("\nAbhishek 15  : Created Page Directory");
  int i;
  for(i=0;i<(NBPG/4);i++)
  {
    pd_t* page_dir_entery=(FRAME0+(frame_num))*NBPG+sizeof(pd_t)*i;
    //only first 4 enteries are present in the beginning
    page_dir_entery->pd_pres=0;
    page_dir_entery->pd_base=0;
    page_dir_entery->pd_write=1;
    page_dir_entery->pd_avail=0;
  }
  for(i=0;i<4;i++)
  {
    pd_t* page_dir_entery=(FRAME0+(frame_num))*NBPG+sizeof(pd_t)*i;
    //only first 4 enteries are present in the beginning
    page_dir_entery->pd_pres=1;
    page_dir_entery->pd_write=1;
    //pd_base only has a 20bit width, so we only save the top 20bit of the pointer
    page_dir_entery->pd_base=((int)g_general_page_table[i])>>12;
    //page_dir_entery->pd_base=((int)g_general_page_table[i]);
  }
  //This frame is used
  g_frame_table[frame_num].fr_status=FRM_MAPPED;
  //Used as page dir
  g_frame_table[frame_num].fr_type=FR_DIR;
  g_frame_table[frame_num].fr_pid=pro_id;
  g_frame_table[frame_num].fr_dirty=0;
  g_frame_table[frame_num].fr_refcnt=0;
  //save page dir base address for each process
  proctab[pro_id].pdbr=(FRAME0+(frame_num))*NBPG;
  //return the frame number which contain the page table
  return frame_num;
}




int create_page_table(int pro_id)
{
  int frame_num,res;
  //find one empty frame to save the new page table
  if(res=get_frm(&frame_num)!=OK)
    return SYSERR;
  //initialize all the page enteries, But the page in this table is not persent
  //yet, the page will be persent when a page fault occur
  int i;
  for(i=0;i<(NBPG/4);i++)
  {
    pt_t* page_entry=(FRAME0+(frame_num))*NBPG+sizeof(pt_t)*i;
    //when initialize, the page entery is not present, maybe this will
    //cause the page fault
    page_entry->pt_pres=0;
    page_entry->pt_base=0;
    page_entry->pt_write=1;
    page_entry->pt_acc=0;
    page_entry->pt_dirty=0;
    page_entry->pt_avail=0;
  }
  //This frame is used
  g_frame_table[frame_num].fr_status=FRM_MAPPED;
  //Used as page table
  g_frame_table[frame_num].fr_type=FR_TBL;
  g_frame_table[frame_num].fr_pid=pro_id;
  g_frame_table[frame_num].fr_dirty=0;
  g_frame_table[frame_num].fr_refcnt=0;
  //return the frame number which contain the page table
  //kprintf("\nReturning Page Number : %d",frame_num);
  return frame_num;
}


int init_general_page_table()
{
     int i;
     int frame_num;
     for(i=0;i<4;i++)
  	{
    	  //the general page table is in control of null thread
    	  frame_num=create_page_table(0);
    	  //frame_num only contains the top 20 bit of the address
    	  g_general_page_table[i]=(frame_num+FRAME0)*NBPG;
 
 	  int ii;
  
    	  for(ii=0;ii<(NBPG/4);ii++)
    	     {
      		//pt_t* page_entery=g_general_page_table[i]+ii;
      		pt_t* page_entery=(FRAME0+(frame_num))*NBPG+sizeof(pt_t)*ii;
      		page_entery->pt_pres=1;
      		page_entery->pt_global=1;
      		page_entery->pt_base=(i<<10)+ii;
      		page_entery->pt_write=1;
      		g_frame_table[i].fr_refcnt++;
    	    }
       }
      return OK;
}
