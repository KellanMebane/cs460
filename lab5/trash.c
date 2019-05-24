PROC *kfork(char *filename)
{
    int i;
    char *cp, *cq;
    char *addr;
    char line[8];
    int usize1, usize;
    int *ustacktop, *usp;
    u32 BA, Btop, Busp;

    PROC *p = dequeue(&freeList);
    if (p == 0)
    {
        kprintf("kfork failed\n");
        return (PROC *)0;
    }
    p->ppid = running->pid;
    p->parent = running;
    p->parent = running;
    p->status = READY;
    p->priority = 1;

    // set kstack to resume to body
    for (i = 1; i < 29; i++) // all 28 cells = 0
        p->kstack[SSIZE - i] = 0;

    p->kstack[SSIZE - 15] = (int)goUmode; // in dec reg=address ORDER !!!
    p->ksp = &(p->kstack[SSIZE - 28]);

    // to go Umode, must set new PROC's Umode cpsr to Umode=10000
    // this was done in ts.s dring init the mode stacks ==>
    // user mode's cspr was set to IF=00, mode=10000
    // must load filename to Umode image area at 7MB+(pid-1)*1MB
    addr = (char *)(0x800000 + (p->pid - 1) * 0x100000);

    // loadelf(filename, p); // p->PROC containing pid, pgdir, etc
    if (strcmp(filename, "u1") == 0)
    {
        usize1 = (int)&_binary_u1_size;
        cp = &_binary_u1_start;
        usize = &_binary_u1_end - &_binary_u1_start;
        // printf("loading u1 %x to %x\n", cp, addr);
    }

    cq = addr;
    for (i = 0; i < usize; i++)
    {
        *cq++ = *cp++;
    }

    //p->usp = (int *)(0x80100000);
    p->usp = (int *)VA(0x100000);

    //  p->kstack[SSIZE-1] = (int)0x80000000;
    p->kstack[SSIZE - 1] = VA(0);
    // -|-----goUmode-------------------------------------------------
    //  r0 r1 r2 r3 r4 r5 r6 r7 r8 r9 r10 ufp uip upc|string       |
    //----------------------------------------------------------------
    //  14 13 12 11 10 9  8  7  6  5  4   3    2   1 |             |

    enqueue(&readyQueue, p);

    kprintf("proc %d kforked a child %d: ", running->pid, p->pid);
    printQ(readyQueue);

    return p;
}