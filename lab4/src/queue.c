
// queue.c file
extern PROC *freeList;
// WRITE YOUR OWN functions:

int enqueue(PROC **queue, PROC *p)
{
	//int SR = int_off(); // IRQ interrupts off, return CPSR
	PROC *current = *queue;

	//if queue is empty
	if (current == 0)
	{
		*queue = p;
		p->next = 0;
		return;
	}

	//if new process is higher priority
	if (current->priority < p->priority)
	{
		p->next = *queue;
		*queue = p;
		return;
	}

	//find a spot with lesser or equal priority
	while ((current->next != 0) && (current->next->priority >= p->priority))
		current = current->next;

	p->next = current->next;
	current->next = p;

	//int_on(SR); //  restore CPSR
}

PROC *dequeue(PROC **queue)
{
	//int SR = int_off(); // IRQ interrupts off, return CPSR
	PROC *front = *queue;
	*queue = front->next;
	front->next = 0;
	//int_on(SR); //  restore CPSR
	return front;
}

void push_sleep(PROC **stack, PROC *p)
{
	p->next = *stack;
	*stack = p;
}

PROC *pop_sleep(PROC **stack)
{
	PROC *temp = *stack;

	*stack = temp->next;

	temp->next = 0;
	temp->event = 0;
	temp->status = READY;

	return temp;
}

void push_timer(TNODE **tqq, TNODE *tp)
{
	tp->tqe = *tqq;
	*tqq = tp;
}

TNODE *pop_timer(TNODE **tqq)
{
	TNODE *temp = *tqq;

	*tqq = temp->tqe;

	return temp;
}
