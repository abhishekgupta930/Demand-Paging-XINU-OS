/* paging.h */

typedef unsigned int	 bsd_t;

/* Structure for a page directory entry */

typedef struct {

  unsigned int pd_pres	: 1;		/* page table present?		*/
  unsigned int pd_write : 1;		/* page is writable?		*/
  unsigned int pd_user	: 1;		/* is use level protection?	*/
  unsigned int pd_pwt	: 1;		/* write through cachine for pt?*/
  unsigned int pd_pcd	: 1;		/* cache disable for this pt?	*/
  unsigned int pd_acc	: 1;		/* page table was accessed?	*/
  unsigned int pd_mbz	: 1;		/* must be zero			*/
  unsigned int pd_fmb	: 1;		/* four MB pages?		*/
  unsigned int pd_global: 1;		/* global (ignored)		*/
  unsigned int pd_avail : 3;		/* for programmer's use		*/
  unsigned int pd_base	: 20;		/* location of page table?	*/
} pd_t;

/* Structure for a page table entry */

typedef struct {

  unsigned int pt_pres	: 1;		/* page is present?		*/
  unsigned int pt_write : 1;		/* page is writable?		*/
  unsigned int pt_user	: 1;		/* is use level protection?	*/
  unsigned int pt_pwt	: 1;		/* write through for this page? */
  unsigned int pt_pcd	: 1;		/* cache disable for this page? */
  unsigned int pt_acc	: 1;		/* page was accessed?		*/
  unsigned int pt_dirty : 1;		/* page was written?		*/
  unsigned int pt_mbz	: 1;		/* must be zero			*/
  unsigned int pt_global: 1;		/* should be zero in 586	*/
  unsigned int pt_avail : 3;		/* for programmer's use		*/
  unsigned int pt_base	: 20;		/* location of page?		*/
} pt_t;

typedef struct{
  unsigned int pg_offset : 12;		/* page offset			*/
  unsigned int pt_offset : 10;		/* page table offset		*/
  unsigned int pd_offset : 10;		/* page directory offset	*/
} virt_addr_t;

typedef struct{
  int bs_status;			/* MAPPED or UNMAPPED		*/
  int bs_pid[NPROC];				/* process id using this slot   */
  int bs_vpno[NPROC];				/* starting virtual page number */
  
  int bs_pages[NPROC];				/* starting virtual page number */
  int bs_npages;			/* number of pages in the store */
  int bs_sem;				/* semaphore mechanism ?	*/
  int bs_count;
  int bs_private;
} bs_map_t;

typedef struct{
  int fr_status;			/* MAPPED or UNMAPPED		*/
  int fr_pid;				/* process id using this frame  */
  int fr_vpno;				/* corresponding virtual page no*/
  int fr_refcnt;			/* reference count		*/
  int fr_type;				/* FR_DIR, FR_TBL, FR_PAGE	*/
  int fr_dirty;
}fr_map_t;

extern bs_map_t bsm_tab[];
extern fr_map_t frm_tab[];
/* Prototypes for required API calls */
SYSCALL xmmap(int, bsd_t, int);
SYSCALL xunmap(int);

/* given calls for dealing with backing store */
extern int debug;
int get_bs(bsd_t, unsigned int);
SYSCALL release_bs(bsd_t);
SYSCALL read_bs(char *, bsd_t, int);
SYSCALL write_bs(char *, bsd_t, int);

#define NBPG		4096	/* number of bytes per page	*/
#define FRAME0		1024	/* zero-th frame		*/

#define BSM_UNMAPPED	0
#define BSM_MAPPED	1

#define FRM_UNMAPPED	0
#define FRM_MAPPED	1
#define V_HEAP          4096   
#define NBPG            4096    /* number of bytes per page     */
#define FRAME0          1024    /* zero-th frame                */
#define NFRAMES         20    /* number of frames             */

#define BSM_UNMAPPED    0
#define BSM_MAPPED      1

#define FR_PAGE         0
#define FR_TBL          1
#define FR_DIR          2
#define FR_UNDEFINE     -1





#define FR_PAGE		0
#define FR_TBL		1
#define FR_DIR		2

#define SC 3
#define AGING 4

#define BACKING_STORE_BASE	0x00800000
#define BACKING_STORE_UNIT_SIZE 0x00100000


// Data Structure and functions for page replacemnet 
struct fr_queue_node
{
  int frame_num;
  //max age is 255, but we can't use char
  unsigned int age;
  int pid;
  struct fr_queue_node* next;
  struct fr_queue_node* prev;
};

struct fr_queue_node* fr_queue_head;
struct fr_queue_node* fr_queue_now;
struct fr_queue_node* fr_queue_current;
struct fr_queue_node* fr_queue_end;
struct fr_queue_node* fr_queue_tail;
struct fr_queue_node* fr_queue_trailing;


// Data Structure and functions for page replacemnet
#define V_HEAP 4096
#define BS_NUM 8
#define BACKING_STORE_BASE      0x00800000
#define BACKING_STORE_UNIT_SIZE 0x00100000
#define get_dir_offset(x)	x>>22		
#define get_table_offset(x)	x>>12&0x000003ff
#define vpage2vaddr(x)		x<<12
#define vpage2dir_offset(x)	x>>10
#define frm_addr(x)        (unsigned long) ((x + FRAME0) * NBPG)

#define NONE 8

#define VIRTUAL_MEMORY_BASE 4096
/*functions for page management*/
int create_page_table(int);
int init_general_page_table();
int create_page_dir(int);
/*page fault handler*/
SYSCALL pfint();

