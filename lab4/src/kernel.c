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
along with this program.  If not, see <http://www.gnu.org/licenses/>.
********************************************************************/

/********************
#define  SSIZE 1024
#define  NPROC  9
#define  FREE   0
#define  READY  1
#define  SLEEP  2
#define  BLOCK  3
#define  ZOMBIE 4
#define  printf  kprintf
 
typedef struct proc{
  struct proc *next;
  int    *ksp;
  int    status;
  int    pid;

  int    priority;
  int    ppid;
  struct proc *parent;
  int    event;
  int    exitCode;
  int    kstack[SSIZE];	
}PROC;
***************************/
#define NPROC 9
PROC proc[NPROC], *running, *freeList, *readyQueue, *sleepStack;
TNODE *activeTimers, all_timers[20], *freeTimers;

// ***************************************************************
// FORWARD DECS
int procsize = sizeof(PROC);
int body(int pid, int ppid, int func, int priority);
int scheduler();
int init();
int kexit(int exitValue);
int ksleep(int event);
int kwait(int *status);
int kwakeup(int event);
PROC *kfork(int func, int priority);
void add_child(PROC *parent, PROC *baby);
PROC *pop_child(PROC *parent);
void timer_queue_init();

// ***************************************************************
// NOT GOOD SHIT

PROC *kfork(int func, int priority)
{
	int SR = int_off();
	int i;
	PROC *p = dequeue(&freeList); // take a node from freeList
	if (p == 0)
	{
		printf("no more PROC, kfork failed\n");
		int_on(SR);
		return 0;
	}
	p->status = READY;
	p->priority = priority;
	p->ppid = running->pid; // set child's parent id to self
	p->parent = running;	// set child's parent to self
	p->child = 0;			//no children yet

	//reset it's stack and open it's body
	for (i = 1; i < 15; i++)
		p->kstack[SSIZE - i] = 0;
	p->kstack[SSIZE - 1] = (int)func;
	p->ksp = &(p->kstack[SSIZE - 14]);

	p->sibling = 0; //doesn't know about siblings in the womb
	add_child(running, p);

	//put it on the ready list
	enqueue(&readyQueue, p);
	printf("%d kforked a child %d\n", running->pid, p->pid);
	printList("readyQueue", readyQueue);

	int_on(SR);
	return p;
}

int kexit(int exitValue)
{
	//int SR = int_off();
	if (running->pid < 2) // don't kill p1
	{
		//int_on(SR);
		return 0;
	}

	if (running->child)
	{
		while (running->child)
		{
			add_child(&proc[1], pop_child(running));
		}

		kwakeup(&proc[1]);
		kwakeup(running->parent);
	}
	else
		kwakeup(running->parent);

	running->child = 0;
	running->exitCode = exitValue;
	running->status = ZOMBIE;

	//printf("TSWITCH FROM EXIT\n");
	tswitch();
	//int_on(SR);

	return 1;
}

int ksleep(int event)
{
	int SR = int_off();
	running->event = event;
	running->status = SLEEP;
	//printf("TSWITCH FROM SLEEP\n");

	push_sleep(&sleepStack, running);

	tswitch();
	int_on(SR);
}

int kwait(int *status)
{
	//int SR = int_off();
	PROC *babies = running->child;
	PROC *temp = 0;

	if (babies == 0)
	{
		//int_on(SR);
		return -1;
	}

	while (1)
	{
		babies = running->child;

		temp = 0;
		while (babies)
		{
			if (babies->status == ZOMBIE)
			{
				if (temp == 0)
				{
					running->child = 0;
				}
				*status = babies->exitCode;
				temp->sibling = babies->sibling;
				babies->sibling = 0;
				babies->priority = 0;
				enqueue(&freeList, babies);
				//int_on(SR);
				return babies->pid;
			}
			temp = babies;
			babies = babies->sibling;
		}
		//printf("SLEEPING FROM WAIT\n");
		ksleep(running);
	}

	//int_on(SR);
}

int kwakeup(int event)
{
	int SR = int_off();
	int i = 1;
	PROC *temp = 0;

	//printf("WAKING UP\n");

	/*for (i = 1; i < 9; i++)
	{
		//printf("checking %d\n", i);
		if (proc[i].status == SLEEP && proc[i].event == event)
		{
			//printf("proc %d is now woke\n", i);
			proc[i].status = READY;
			enqueue(&readyQueue, &proc[i]);
			printList("READY ", readyQueue);
		}
	}*/

	temp = pop_sleep(&sleepStack);

	temp->status = READY;

	enqueue(&readyQueue, temp);
	//printList("READY ", readyQueue);

	int_on(SR);
}

// ********************************************************************
// GOOD SHIT

int init()
{
	int i, j;
	PROC *p;
	kprintf("kernel_init()\n");
	for (i = 0; i < NPROC; i++)
	{
		p = &proc[i];
		p->pid = i;
		p->status = READY;
		p->next = p + 1;
	}
	proc[NPROC - 1].next = 0; // circular proc list
	freeList = &proc[0];
	readyQueue = 0;

	printf("create P0 as initial running process\n");
	p = dequeue(&freeList);
	p->priority = 0;
	p->ppid = 0;
	p->parent = p; // P0's parent is itself
	p->child = 0;
	p->sibling = 0;
	running = p;
	kprintf("running = %d\n", running->pid);
	printList("freeList", freeList);

	timer_queue_init();
}

int body(int pid, int ppid, int func, int priority)
{
	char c;
	char line[64];
	int excuse = 0, status = 0;
	TNODE *tt = 0;
	//int pid;
	//printf("pid: %d   ppid: %d   func: %x   priority: %d\n", pid, ppid, func, priority);
	kprintf("proc %d resume to body()\n", running->pid);
	kprintf("pid: %d   ppid: %d ", running->pid, running->ppid);
	while (1)
	{
		pid = running->pid;
		if (pid == 0)
			color = BLUE;
		if (pid == 1)
			color = WHITE;
		if (pid == 2)
			color = GREEN;
		if (pid == 3)
			color = CYAN;
		if (pid == 4)
			color = YELLOW;
		if (pid == 5)
			color = WHITE;
		if (pid == 6)
			color = GREEN;
		if (pid == 7)
			color = WHITE;
		if (pid == 8)
			color = CYAN;

		printList("readyQueue", readyQueue);
		printList("freeList", freeList);
		kprintf("proc %d running, parent = %d  ", running->pid, running->ppid);
		printChildren("children: ", running->child);
		kprintf("input a char [s|f|q|w|t] : ");
		c = kgetc();
		printf("%c\n", c);

		//printf("POST GET C : %d\n", c);

		switch (c)
		{
		case 's':
			//printf("TSWITCH FROM BODY\n");
			tswitch();
			break;
		case 'f':
			kfork((int)body, 1);
			break;
		case 'q':
			printf("what's your excuse?: ");
			kgets(line);
			excuse = atoi(line);
			printf("%s\n", line);
			kexit(excuse);
			break;
		case 'w':
			status = 0;
			pid = 0;
			pid = kwait(&status);
			//printf("killed child: %d  exit: %d\n", pid, status);
			break;
		case 't':
			tt = pop_timer(&freeTimers);
			kgets(line);
			tt->seconds = atoi(line);
			tt->this_proc = running;
			push_timer(&activeTimers, tt);
			ksleep(running);
			break;
		}

		c = 0;
	}
}

int scheduler()
{
	//kprintf("proc %d in scheduler ", running->pid);
	if (running->status == READY)
		enqueue(&readyQueue, running);
	running = dequeue(&readyQueue);
	//kprintf("next running = %d\n", running->pid);
}

void add_child(PROC *parent, PROC *baby)
{
	int SR = int_off();
	if (parent == 0)
	{
		int_on(SR);
		return;
	}

	if (parent->child == 0) // empty list
	{
		parent->child = baby;
		int_on(SR);
		return;
	}

	baby->parent = parent;
	baby->ppid = parent->pid;
	baby->sibling = parent->child; // silbing is front
	parent->child = baby;		   // point at baby

	int_on(SR);
}

PROC *pop_child(PROC *parent)
{
	int SR = int_off();
	PROC *front = parent->child;

	if (front == 0)
	{
		int_on(SR);
		return 0;
	}

	parent->child = front->sibling;
	front->sibling = 0;

	int_on(SR);

	return front;
}

void timer_queue_init() {
	int i = 0;

	for (i = 0; i < 20; i++)
	{
		all_timers[i].seconds = -1; //no time
		all_timers[i].this_proc = -1; //no proc
		all_timers[i].tqe = 0; //no friends

		push_timer(&freeTimers, &all_timers[i]); //throw into free list
	}
}