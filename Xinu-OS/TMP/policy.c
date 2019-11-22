/* policy.c = srpolicy*/

#include <conf.h>
#include <kernel.h>
#include <paging.h>
#include <proc.h>

fr_map_t g_frame_table[NFRAMES];

extern int page_replace_policy;
extern struct fr_queue_node* fr_queue_head;
extern struct fr_queue_node* fr_queue_now;
extern struct fr_queue_node* fr_queue_end;
extern struct fr_queue_node* fr_queue_trailing;

/*-------------------------------------------------------------------------
 * srpolicy - set page replace policy 
 *-------------------------------------------------------------------------
 */
SYSCALL srpolicy(int policy)
{
 /* sanity check ! */
#ifdef PG_DEBUG
  kprintf("PID:%d srpolicy policy:%d\n",currpid,policy);
#endif
  debug=1;
  page_replace_policy=policy;
  return OK;

}

SYSCALL init_policy() {



 if(page_replace_policy==SC)
  {
    fr_queue_end=(struct fr_queue_node*)getmem(sizeof(struct fr_queue_node));
    fr_queue_end->frame_num=-1;
    fr_queue_end->age=255;
    fr_queue_end->pid=currpid;
    fr_queue_end->next=fr_queue_end->prev=fr_queue_end;
    fr_queue_now=fr_queue_end;
  }

 if(page_replace_policy== AGING)
  {
    fr_queue_end=(struct fr_queue_node*)getmem(sizeof(struct fr_queue_node));
    fr_queue_end->frame_num=-1;
    fr_queue_end->age=255;
    fr_queue_end->pid=currpid;
    fr_queue_end->next=fr_queue_end->prev=fr_queue_end;
    fr_queue_now=fr_queue_end;
  }

}

SYSCALL replacement_policy_insert(int frame_num) {
//  kprintf("\n In replacement_policy_insert \n");
 //if the replace policy is SC, insert the frame to the cirular queue
  if(page_replace_policy==SC)
  {
    //the first page fault, init the head already existed
    if(fr_queue_end->frame_num == -1)
    {
      fr_queue_end->frame_num=frame_num;
      fr_queue_end->next=fr_queue_end->prev=fr_queue_end;
      fr_queue_end->age=255;
      fr_queue_end->pid=currpid;
      fr_queue_now=fr_queue_end;
      //fr_queue_trailing=fr_queue_now;
    }
    else
    {
      struct fr_queue_node* new_node=(struct fr_queue_node*)getmem(sizeof(struct fr_queue_node));
      new_node->frame_num=frame_num;
      new_node->age=255;
      new_node->pid=currpid;
      //inserting at the end of a circular queue
      fr_queue_end->next->prev=new_node;
      new_node->next=fr_queue_end->next;
      new_node->prev=fr_queue_end;
      fr_queue_end->next=new_node;
      fr_queue_end=new_node;
   }

 }

 if(page_replace_policy==AGING)
  {
    //the first page fault, init the head already existed
    if(fr_queue_end->frame_num == -1)
    {
      fr_queue_end->frame_num=frame_num;
      fr_queue_end->next=fr_queue_end->prev=fr_queue_end;
      fr_queue_end->age=255;
      fr_queue_end->pid=currpid;
      fr_queue_now=fr_queue_end;
    }
    else
    {
      struct fr_queue_node* new_node=(struct fr_queue_node*)getmem(sizeof(struct fr_queue_node));
      new_node->frame_num=frame_num;
      new_node->age=255;
      new_node->pid=currpid;
      //inserting at the end of a circular queue
      fr_queue_end->next->prev=new_node;
      new_node->next=fr_queue_end->next;
      new_node->prev=fr_queue_end;
      fr_queue_end->next=new_node;
      fr_queue_end=new_node;
   }

 }

}

SYSCALL replacement_policy_getframe() {

     
    int frame_num;
    //kprintf("\n In replacement_policy_getframe \n");
  if(page_replace_policy==SC)
  {

    
    int vpno;
    pd_t* pde;
    pt_t* pte;
    struct fr_queue_node* tmp;
    //find the correct frame and swap it
    //if (fr_queue_now == fr_queue_end->next)
    //fr_queue_trailing = fr_queue_now;
    while(1)
    {
      //find the page entery to this frame
      frame_num = fr_queue_now->frame_num;

     // kprintf("\nFrame being Processed = %d\n",frame_num);
      vpno=g_frame_table[frame_num].fr_vpno;
      pde=(pd_t*)proctab[fr_queue_now->pid].pdbr;
      pde+=(vpno>>10);
      if(pde->pd_pres==0)
        return SYSERR;
      pte=(pt_t*)(pde->pd_base<<12);
      pte+=(vpno)&0x000003ff;
      if(pte->pt_pres==0)
        return SYSERR;
      
      //if the acc bit of the correctsponding page is 0, swap this frame
      if((pte->pt_acc)==0)
        {
           
	   frame_num = fr_queue_now->frame_num;
	   tmp = fr_queue_now->next;
	   //Handiling to delete current Node
	   fr_queue_now->prev->next = fr_queue_now->next;
	   fr_queue_now->next->prev = fr_queue_now->prev; 

	   //Free Node
           free_frm(fr_queue_now->frame_num);
           freemem(fr_queue_now,sizeof(struct fr_queue_node));
           if(debug)
	          kprintf("Process ID :%d Page Replecement Policy [SC] Frame Obtained :%d\n",currpid,frame_num);

           //kprintf("\nFrame being Replaced = %d\n",frame_num);
  	   fr_queue_now = tmp;
      	   return frame_num;
       }
      //else set that acc bit to 0
      else
	{
           pte->pt_acc=0;
           //move to the next frame in the queue
      	   fr_queue_now=fr_queue_now->next;
	}
    }

}



	else if(page_replace_policy==AGING){


    kprintf("\n In replacement_policy : AGING \n");
    int min_age=255;
    struct fr_queue_node* p=NULL;
    struct fr_queue_node* q=NULL;
    int frame_num;
    int vpno;
    pd_t* pde;
    pt_t* pte;


    for(p=fr_queue_end->next;p!=fr_queue_end->next;p=p->next)
     {
      //find the page entery to this frame
      vpno=g_frame_table[p->frame_num].fr_vpno;
      pde=(pd_t*)proctab[p->pid].pdbr;
      pde+=(vpno>>10);
      if(pde->pd_pres==0)
        return SYSERR;
      pte=(pt_t*)(pde->pd_base<<12);
      pte+=(vpno)&0x000003ff;
      if(pte->pt_pres==0)
        return SYSERR;
      //half the age of every node
      p->age=p->age>>1;
      //if the acc bit is set, add 128 to the age
      if(pte->pt_acc==1)
        p->age+=128;
      //save the min age in this iteration
      if(p->age<min_age)
	{
          min_age=p->age;
        }
    }

    struct fr_queue_node* trav;
    trav = fr_queue_end->next;

    if (trav->next == trav && trav->age == min_age)
          {

             frame_num =  trav->frame_num;
	     if(debug)
                  kprintf("Process ID :%d Page Replecement Policy [AGING] Frame Obtained :%d\n",currpid,frame_num);

             free_frm(frame_num);
             freemem(trav,sizeof(struct fr_queue_node));
             fr_queue_end->frame_num=-1;
             fr_queue_end->age=255;
             fr_queue_end->next=fr_queue_end->prev=fr_queue_end;
             fr_queue_now=fr_queue_end;
             return frame_num;

          }


    if (fr_queue_end->frame_num != -1){
          do{
             if (trav->age == min_age)
                {
                   frame_num =  trav->frame_num;
	     	   if(debug)
                        kprintf("Process ID :%d Page Replecement Policy [AGING] Frame Obtained :%d\n",currpid,frame_num);
		   //kprintf("\n Aging Policy : Frame being swapped : %d",frame_num);
                   trav->prev->next = trav->next;
                   trav->next->prev = trav->prev;
           	   free_frm(frame_num);
                   freemem(trav,sizeof(struct fr_queue_node));
                    break;
                }
             trav=trav->next;

            }while(trav!= fr_queue_end->next);
	 return frame_num; 
        }
  }

  return SYSERR;

}



SYSCALL printallframes()
{

    struct fr_queue_node* trav;
	trav = fr_queue_end->next;
   if (page_replace_policy==SC || page_replace_policy==AGING){

	if (fr_queue_end->frame_num != -1)
		{
		   do{
//			kprintf("\n Frame Number : %d", trav->frame_num);
			trav=trav->next;

			}while(trav!= fr_queue_end->next);

		}
     }

}


/*-------------------------------------------------------------------------
 * grpolicy - get page replace policy 
 *-------------------------------------------------------------------------
 */
SYSCALL grpolicy()
{
  return page_replace_policy;
}


SYSCALL replacement_policy_deleteframe(int frame_num) {

  	struct fr_queue_node* trav;
  	trav = fr_queue_end->next;

  // Deleting Last node of Doubly Linked List
	if (trav->next == trav)
	  {
	     freemem(trav,sizeof(struct fr_queue_node));     
	     fr_queue_end->frame_num=-1;
    	     fr_queue_end->age=255;
    	     fr_queue_end->next=fr_queue_end->prev=fr_queue_end;
    	     fr_queue_now=fr_queue_end;
          }



	
	if (fr_queue_end->frame_num != -1){
	  do{
             if (trav->frame_num == frame_num)
		{
		   trav->prev->next = trav->next;
                   trav->next->prev = trav->prev;
           	   freemem(trav,sizeof(struct fr_queue_node));
		    break;
		}
             trav=trav->next;

            }while(trav!= fr_queue_end->next);
	}


}

