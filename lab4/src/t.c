/********************************************************************
Copyright 2010-2017 K.C. Wang, <kwang@eecs.wsu.edu>
This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.o rg/licenses/>.
********************************************************************/

#include "type.h"
#include "string.c"
#include "queue.c"
#include "kbd.c"
#include "vid.c"
#include "exceptions.c"
#include "kernel.c"
#include "timer.c"

TIMER *tp[4];

int printList(char *name, PROC *p)
{
    kprintf("%s = ", name);
    while (p)
    {
        kprintf("[%d%d]->", p->pid, p->priority);
        p = p->next;
    }
    kprintf("NULL\n");
}

int printChildren(char *name, PROC *p)
{
    kprintf("%s = ", name);
    while (p)
    {
        kprintf("[%d%d]->", p->pid, p->priority);
        p = p->sibling;
    }
    kprintf("NULL\n");
}

void copy_vectors(void)
{
    extern u32 vectors_start;
    extern u32 vectors_end;
    u32 *vectors_src = &vectors_start;
    u32 *vectors_dst = (u32 *)0;

    while (vectors_src < &vectors_end)
        *vectors_dst++ = *vectors_src++;
}
int kprintf(char *fmt, ...);
void IRQ_handler()
{
    int vicstatus, sicstatus;
    int ustatus, kstatus;

    // read VIC status register to find out which interrupt
    vicstatus = VIC_STATUS; // VIC_STATUS=0x10140000=status reg
    sicstatus = SIC_STATUS;
    //kprintf("vicstatus=%x sicstatus=%x\n", vicstatus, sicstatus);
    if (vicstatus & (1 << 4))
    { // timer0,1=bit4
        timer_handler(0);
    }
    if (vicstatus & (1 << 31))
    {
        if (sicstatus & (1 << 3))
        {
            //kprintf("CPSR: %u  SPSR: %u\n", getCPSR() & 0x1F, getSPSR() & 0x1F);
            //kprintf("CPU in IRQ mode, previous Mode is xxx mode\n");
            kbd_handler();
        }
    }
}
int body();
int main()
{
    int i;
    char line[128];
    u8 kbdstatus, key, scode;

    color = WHITE;
    row = col = 0;

    fbuf_init();
    timer_init();
    kbd_init();

    /* enable KBD IRQ */
    VIC_INTENABLE |= (1 << 4); // timer0,1 at bit4
    VIC_INTENABLE |= (1 << 5); // timer2,3 at bit5
    VIC_INTENABLE |= 1 << 31;  // SIC to VIC's IRQ31
    SIC_ENSET |= 1 << 3;       // KBD int=3 on SIC

    timer_start(0);

    kprintf("Welcome to KINX in Arm\n");
    init();
    kfork((int)body, 1);

    while (1)
    {
        while (readyQueue == 0);
            //printf("fuck");
        printf("TSWITCH FROM MAIN\n");
        tswitch();
    }
}
