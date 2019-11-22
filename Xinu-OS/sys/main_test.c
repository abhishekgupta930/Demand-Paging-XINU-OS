#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <stdio.h>
#include <paging.h>


#define PROC1_VADDR	0x40000000
#define PROC1_VADDR4	0x50000000
#define PROC1_VADDR5	0x60000000
#define PROC1_VPNO      0x40000
#define PROC1_VPNO4     0x50000
#define PROC1_VPNO5     0x60000
#define PROC2_VADDR     0x80000000
#define PROC2_VPNO      0x80000
#define TEST1_BS	1
#define TEST2_BS	2


void proc1_test1(char *msg, int lck) {
	char *addr;
	int i;

	get_bs(TEST1_BS, 50);

	if (xmmap(PROC1_VPNO, TEST1_BS, 50) == SYSERR) {
		kprintf("xmmap call failed\n");
		sleep(3);
		return;
	}


        kprintf("\nAbhishek 11/10 : Just before assigning value");
	addr = (char*) PROC1_VADDR;
	for (i = 0; i < 40; i++) {
		*(addr + i * NBPG) = 'A' + i;
	}

        kprintf("\nAbhishek 11/10 : Just before printing  value");
	sleep(6);

	for (i = 0; i < 40; i++) {
		kprintf("0x%08x: %c\n", addr + i * NBPG, *(addr + i * NBPG));
	}

	xmunmap(PROC1_VPNO);
	return;
}


void proc1_test4(char *msg, int lck) {


 	char *addr;
        int i;

        get_bs(TEST2_BS, 50);

        if (xmmap(PROC1_VPNO4, TEST2_BS, 50) == SYSERR) {
                kprintf("xmmap call failed\n");
                sleep(3);
                return;
        }


        kprintf("\nAbhishek 25 : Just before assigning value");
        addr = (char*) PROC1_VADDR4;
        for (i = 0; i < 20; i++) {
                *(addr + i * NBPG) = 'M' + i;
        }

        sleep(2);

        for (i = 0; i < 20; i++) {
                kprintf("0x%08x: %c\n", addr + i * NBPG, *(addr + i * NBPG));
        }

        xmunmap(PROC1_VPNO4);
        return;


}

void proc1_test5(char *msg, int lck) {


        char *addr;
        int i;
	 get_bs(TEST2_BS, 50);


        if (xmmap(PROC1_VPNO5, TEST2_BS, 50) == SYSERR) {
                kprintf("xmmap call failed\n");
                sleep(3);
                return;
        }
        addr = (char*) PROC1_VADDR5;

        sleep(2);
	//get_bs(TEST2_BS, 100);

        kprintf("\nReading from backking Stores\n");
        for (i = 0; i < 10; i++) {
                kprintf(" Reading from backing stores 0x%08x: %c\n", addr + i * NBPG, *(addr + i * NBPG));
        }

        return;


}






void proc1_test2(char *msg, int lck) {
	int *x;
	int *y;

	kprintf("ready to allocate heap space\n");
	x = vgetmem(2048);
	kprintf("heap allocated at %x\n", x);
	*x = 100;
	*(x + 1) = 200;

	kprintf("heap variable: %d %d\n", *x, *(x + 1));


	vfreemem(x, 1024);
	sleep(5);
        y = vgetmem(1024);
        kprintf("heap allocated at %x\n", x);
        //*x = 100;
        //*(x + 1) = 200;

        kprintf("heap variable: %d %d\n", *x, *(x + 1));




}

void proc1_test3(char *msg, int lck) {

	char *addr;
	int i;

	addr = (char*) 0x0;

	for (i = 0; i < 26; i++) {
		*(addr + i * NBPG) = 'B';
	}

	for (i = 0; i < 26; i++) {
		kprintf("0x%08x: %c\n", addr + i * NBPG, *(addr + i * NBPG));
	}

	return;
}


int main() {
	int pid1;
	int pid2;
	int pid3;

	srpolicy(SC);

	kprintf("\n1: shared memory\n");
	pid1 = create(proc1_test1, 2000, 20, "proc1_test1", 0, NULL);
	resume(pid1);

	sleep(10);
	
	pid2 = create(proc1_test4, 2000, 20, "proc1_test4", 0, NULL);
	resume(pid2);

	sleep(10);


	kprintf("\n2: vgetmem/vfreemem\n");
	pid1 = vcreate(proc1_test2, 2000, 100, 20, "proc1_test2", 0, NULL);
	kprintf("pid %d has private heap\n", pid1);
	resume(pid1);
	sleep(5);
}
