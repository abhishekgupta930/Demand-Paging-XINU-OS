#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <mark.h>
#include <bufpool.h>
#include <paging.h>

int write_bs(char *src, bsd_t bs_id, int page) {

  /* write one page of data from src
     to the backing store bs_id, page
     page.
  */
   //kprintf("\n10/25 Abhishek in write_bs()\n");
   char * phy_addr = BACKING_STORE_BASE + bs_id*BACKING_STORE_UNIT_SIZE + page*NBPG;
   char * tmp = (char*) phy_addr;
   //kprintf("Location getting written: 0x%08x , Value at location = %c\n",tmp,*src);
   bcopy((void*)src, phy_addr, NBPG);

}

