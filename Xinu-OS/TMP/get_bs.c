#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <paging.h>
extern bs_map_t g_back_store_table[BS_NUM];

int get_bs(bsd_t bs_id, unsigned int npages)
{
	
        STATWORD ps;
        disable(ps);

 	if (bs_id >= 0 && bs_id < BS_NUM) 
	    {
                if (bsm_tab[bs_id].bs_status == BSM_UNMAPPED) 
		    {
                        bsm_tab[bs_id].bs_status = BSM_MAPPED;
                        bsm_tab[bs_id].bs_pid[currpid] = BSM_MAPPED;
                        bsm_tab[bs_id].bs_npages = npages;
                        bsm_tab[bs_id].bs_private = FALSE;
                        restore(ps);
                        return npages;
                    }
                else 
		    {
                      	if (bsm_tab[bs_id].bs_private == FALSE) 
			  {
                                bsm_tab[bs_id].bs_pid[currpid] = BSM_MAPPED;
                                restore(ps);
                                return bsm_tab[bs_id].bs_npages;
                          }
                        else 
			  {
                                restore(ps);
                                return SYSERR;
                          }
                     }
        }
        else {
                //kprintf("\nget_bs: fail : bs id out of range");
                restore(ps);
                return SYSERR;
        }
                

}
