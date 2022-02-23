//
// CPU Schedule Simulator Homework
// Student Number : B617065
// Name : 신성우
//

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <limits.h>
#include <string.h>
#include <stdbool.h>

#define SEED 10
#define S_IDLE 0
#define S_READY 1
#define S_BLOCKED 2
#define S_RUNNING 3
#define S_TERMINATE 4

int NPROC, NIOREQ, QUANTUM;

struct ioDoneEvent {
	int procid;
	int doneTime;
	int len;
	struct ioDoneEvent *prev;
	struct ioDoneEvent *next;
} ioDoneEventQueue, *ioDoneEvent;

struct process {
	int id;
	int len;
	int targetServiceTime;
	int serviceTime;
	int startTime;
	int endTime;
	char state;
	int priority;
	int saveReg0, saveReg1;
	struct process *prev;
	struct process *next;
} *procTable;

struct process idleProc;
struct process readyQueue;
struct process blockedQueue;
struct process *runningProc;

int cpuReg0, cpuReg1;
int currentTime = 0;
int *procIntArrTime, *procServTime, *ioReqIntArrTime, *ioServTime;

void compute() {
	cpuReg0 = cpuReg0 + runningProc->id;
	cpuReg1 = cpuReg1 + runningProc->id;
}

// initialize functions
void initProcTable() {
	int i;
	for(i=0; i < NPROC; i++) {
		procTable[i].id = i;
		procTable[i].len = 0;
		procTable[i].targetServiceTime = procServTime[i];
		procTable[i].serviceTime = 0;
		procTable[i].startTime = 0;
		procTable[i].endTime = 0;
		procTable[i].state = S_IDLE;
		procTable[i].priority = 0;
		procTable[i].saveReg0 = 0;
		procTable[i].saveReg1 = 0;
		procTable[i].prev = NULL;
		procTable[i].next = NULL;
	}
}

void	init_idle_proc(void)
{
	idleProc.id = -1;
	idleProc.len = 0;
	idleProc.targetServiceTime = INT_MAX;
	idleProc.serviceTime = 0;
	idleProc.startTime = 0;
	idleProc.endTime = 0;
	idleProc.state = S_IDLE;
	idleProc.priority = 0;
	idleProc.saveReg0 = 0;
	idleProc.saveReg1 = 0;
	idleProc.prev = NULL;
	idleProc.next = NULL;
}

void	init_io_done_event(void)
{
	int i;
	for(i=0; i < NIOREQ; i++) {
		ioDoneEvent[i].procid = -1;
		ioDoneEvent[i].doneTime = 0;
		ioDoneEvent[i].len = 0;
		ioDoneEvent[i].prev = NULL;
		ioDoneEvent[i].next = NULL;
	}
}

// global variables
struct process		*ready_queue = NULL;
struct process		*rear_ready = NULL;
struct process		*blocked_queue = NULL;
struct process		*rear_blocked = NULL;
struct ioDoneEvent	*io_queue = NULL;
struct ioDoneEvent	*rear_io = NULL;
struct ioDoneEvent	*io_done_proc = NULL;

// queue handling functions
void	enqueue_proc(struct process **queue, struct process *proc_to_add, struct process **rear)
{
	if (*queue == NULL)
	{
		*queue = proc_to_add;
		*rear = proc_to_add;
	}
	else
	{
		proc_to_add->prev = *rear;
		(*rear)->next = proc_to_add;
		*rear = proc_to_add;
	}
}

void	enqueue_io(struct ioDoneEvent **queue, struct ioDoneEvent *proc_to_add, struct ioDoneEvent **rear)
{
	if (*queue == NULL)
	{
		*queue = proc_to_add;
		*rear = proc_to_add;
	}
	else
	{
		proc_to_add->prev = *rear;
		(*rear)->next = proc_to_add;
		*rear = proc_to_add;
	}
}

void	del_proc(struct process **queue, struct process *proc_to_del, struct process **rear)
{
	if (proc_to_del->prev == NULL && proc_to_del->next == NULL)
	{
		*queue = NULL;
		*rear = NULL;
	}
	else if (proc_to_del->prev == NULL)
	{
		proc_to_del->next->prev = NULL;
		*queue = proc_to_del->next;
		proc_to_del->next = NULL;

	}
	else if (proc_to_del->next == NULL)
	{
		proc_to_del->prev->next = NULL;
		*rear = proc_to_del->prev;
		proc_to_del->prev = NULL;
	}
	else
	{
		proc_to_del->prev->next = proc_to_del->next;
		proc_to_del->next->prev = proc_to_del->prev;
		proc_to_del->prev = NULL;
		proc_to_del->next = NULL;
	}
}

void	del_io(struct ioDoneEvent **queue, struct ioDoneEvent *io_to_del, struct ioDoneEvent **rear)
{
	if (io_to_del->prev == NULL && io_to_del->next == NULL)
	{
		*queue = NULL;
		*rear = NULL;
	}
	else if (io_to_del->prev == NULL)
	{
		io_to_del->next->prev = NULL;
		*queue = io_to_del->next;
		io_to_del->next = NULL;

	}
	else if (io_to_del->next == NULL)
	{
		io_to_del->prev->next = NULL;
		*rear = io_to_del->prev;
		io_to_del->prev = NULL;
	}
	else
	{
		io_to_del->prev->next = io_to_del->next;
		io_to_del->next->prev = io_to_del->prev;
		io_to_del->prev = NULL;
		io_to_del->next = NULL;
	}
}

struct ioDoneEvent	*get_io_done_proc(void)
{
	struct ioDoneEvent	*cur;

	if (io_queue == NULL)
		return (NULL);
	if (io_queue->doneTime == currentTime)
		return (io_queue);
	cur = io_queue;
	while (cur && cur->doneTime != currentTime)
		cur = cur->next;
	if (cur == NULL)
		return (NULL);
	return (cur);
}

// print result functions
void	print_ready_queue(void)
{
	struct process *cur;

	printf("ready queue: ");
	if (ready_queue == NULL)
	{
		printf("(null)\n");
		return ;
	}
	cur = ready_queue;
	while (cur)
	{
		printf("%d ", cur->id);
		cur = cur->next;
	}
	printf("\n");
}

void	print_blocked_queue(void)
{
	struct process *cur;

	printf("blocked queue: ");
	if (blocked_queue == NULL)
	{
		printf("(null)\n");
		return ;
	}
	cur = blocked_queue;
	while (cur)
	{
		printf("%d ", cur->id);
		cur = cur->next;
	}
	printf("\n");
}

void	print_io_queue(void)
{
	struct ioDoneEvent *cur;

	printf("io queue: ");
	if (io_queue == NULL)
	{
		printf("(null)\n");
		return ;
	}
	cur = io_queue;
	while (cur)
	{
		printf("%d", cur->procid);
		printf("(%d) ", cur->doneTime);
		cur = cur->next;
	}
	printf("\n");
}

void	print_proc_table()
{
	int i;
	for(i=0; i < NPROC; i++) {
		printf("id: %d / ", procTable[i].id);
		printf("targetServiceTime: %d / ", procTable[i].targetServiceTime);
		printf("serviceTime: %d / ", procTable[i].serviceTime);
		printf("startTime: %d / ", procTable[i].startTime);
		printf("endTime: %d / ", procTable[i].endTime);
		if (procTable[i].state == S_TERMINATE)
			printf("state: TERMINATE\n");
		else
			printf("state: %d\n", procTable[i].state);
	}
}

void procExecSim(struct process *(*scheduler)())
{
	int		pid, qTime=0, cpuUseTime = 0, nproc=0, termProc = 0, nioreq=0;
	int		nextForkTime, nextIOReqTime;
	int		cnt_term = 0;
	char	schedule = 0, nextState = S_IDLE;
	bool	flag_run_to_ready;
	bool	flag_switch;

	init_idle_proc();
	init_io_done_event();
	runningProc = &idleProc;
	nextForkTime = procIntArrTime[nproc];
	nextIOReqTime = ioReqIntArrTime[nioreq];
	while (1)
	{
		if (cnt_term == NPROC)
			break ;
		++currentTime;
		++qTime;
		++(runningProc->serviceTime);
		printf("==================== currnet time: %d ====================\n", currentTime);
		if (runningProc->state == S_IDLE)
			printf("running process: idleProc\n");
		else
			printf("running process: %d\n", runningProc->id);
		/* MUST CALL compute() Inside While loop */
		if (runningProc->state == S_RUNNING)
		{
			compute();
			++cpuUseTime;
			runningProc->saveReg0 = cpuReg0;
			runningProc->saveReg1 = cpuReg0;
		}
		/* CASE 4 : the process job done and terminates */
		flag_run_to_ready = false;
		flag_switch = false;
		if (runningProc->serviceTime == runningProc->targetServiceTime)
		{
			printf("\e[0;32mThe process job done and terminated\n\e[0m"); // BGRN
			runningProc->state = S_TERMINATE;
			runningProc->endTime = currentTime;
			++cnt_term;
			flag_run_to_ready = false;
		}
		/* CASE 5: reqest IO operations (only when the process does not terminate) */
		if ((cpuUseTime == nextIOReqTime) && (nioreq < NIOREQ))
		{
			printf("\e[1;36mAn IO operation requested\n\e[0m"); // BCYN
			if (runningProc->state == S_RUNNING)
				runningProc->state = S_BLOCKED;
			if (qTime == QUANTUM)
				--(runningProc->priority); // cpu bound
			else
				++(runningProc->priority); // io bound
			enqueue_proc(&blocked_queue, runningProc, &rear_blocked);
			flag_run_to_ready = false;
			ioDoneEvent[nioreq].procid = runningProc->id;
			ioDoneEvent[nioreq].doneTime = currentTime + ioServTime[nioreq];
			enqueue_io(&io_queue, &(ioDoneEvent[nioreq]), &rear_io);
			++nioreq;
			if (nioreq != NIOREQ)
				nextIOReqTime += ioReqIntArrTime[nioreq];
		}
		/* CASE 2 : a new process created */
		if (currentTime == nextForkTime)
		{
			printf("\e[1;33mA new process created\n\e[0m"); // BYEL
			procTable[nproc].state = S_READY;
			procTable[nproc].startTime = currentTime;
			enqueue_proc(&ready_queue, &procTable[nproc], &rear_ready);
			++nproc;
			if (nproc != NPROC)
				nextForkTime += procIntArrTime[nproc];
			if (runningProc->state == S_RUNNING)
				flag_run_to_ready = true;
		}
		/* CASE 1 : The quantum expires */
		if (qTime == QUANTUM)
		{
			printf("\e[1;31mThe quantum expired\n\e[0m"); // BRED
			if (runningProc->state == S_RUNNING)
			{
				--(runningProc->priority); // cpu bound
				flag_run_to_ready = true;
			}
		}
		/* CASE 3 : IO Done Event */
		io_done_proc = NULL;
		while (1)
		{
			io_done_proc = get_io_done_proc();
			if (io_done_proc == NULL)
				break ;
			printf("\e[1;34mThe IO operation done\n\e[0m"); // BBLU
			del_io(&io_queue, io_done_proc, &rear_io);
			del_proc(&blocked_queue, &(procTable[io_done_proc->procid]), &rear_blocked);
			if ((procTable[io_done_proc->procid]).state == S_BLOCKED)
			{
				(procTable[io_done_proc->procid]).state = S_READY;
				enqueue_proc(&ready_queue, &(procTable[io_done_proc->procid]), &rear_ready);
			}
			if (runningProc->state == S_RUNNING)
				flag_run_to_ready = true;
		}
		if (flag_run_to_ready == true)
		{
			runningProc->state = S_READY;
			enqueue_proc(&ready_queue, runningProc, &rear_ready);
		}
		print_ready_queue();
		print_blocked_queue();
		print_io_queue();
		if (runningProc->state != S_RUNNING)
		{
			if (ready_queue == NULL)
				runningProc = &idleProc;
			else
			{
				runningProc = (*scheduler)();
				del_proc(&ready_queue, runningProc, &rear_ready);
				runningProc->state = S_RUNNING;
				cpuReg0 = runningProc->saveReg0;
				cpuReg1 = runningProc->saveReg1;
				qTime = 0;
			}
		}
		usleep(400000);
	}
	print_proc_table();
}

// Round Robin Scheduling
struct process *RRschedule()
{
	return (ready_queue);
}

struct process	*get_next_proc(char *sch_method)
{
	struct process	*cur;
	struct process	*next_proc;

	cur = ready_queue;
	next_proc = ready_queue;
	while (cur)
	{
		if (!strcmp(sch_method, "SJF") && (cur->targetServiceTime < next_proc->targetServiceTime))
			next_proc = cur;
		else if (!strcmp(sch_method, "SRTN") && \
		(cur->targetServiceTime - cur->serviceTime) < (next_proc->targetServiceTime - next_proc->serviceTime))
			next_proc = cur;
		else if (!strcmp(sch_method, "GS") && \
		((float)(cur->serviceTime) / (float)(cur->targetServiceTime)) < ((float)(next_proc->serviceTime) / (float)(next_proc->targetServiceTime)))
			next_proc = cur;
		else if (!strcmp(sch_method, "SFS") && (cur->priority > next_proc->priority))
			next_proc = cur;
		cur = cur->next;
	}
	return (next_proc);
}

// Shortest Job First
struct process	*SJFschedule()
{
	return (get_next_proc("SJF"));
}

// Shortest Remaining Time Next
struct process	*SRTNschedule()
{
	return (get_next_proc("SRTN"));
}

// Guaranteed Scheduling
struct process	*GSschedule()
{
	return (get_next_proc("GS"));
}

// Simple Feedback Scheduling
struct process	*SFSschedule()
{
	return (get_next_proc("SFS"));
}

void printResult() {
	int i;
	long totalProcIntArrTime=0,totalProcServTime=0,totalIOReqIntArrTime=0,totalIOServTime=0;
	long totalWallTime=0, totalRegValue=0;
	for(i=0; i < NPROC; i++) {
		totalWallTime += procTable[i].endTime - procTable[i].startTime;
		/*
		printf("proc %d serviceTime %d targetServiceTime %d saveReg0 %d\n",
			i,procTable[i].serviceTime,procTable[i].targetServiceTime, procTable[i].saveReg0);
		*/
		totalRegValue += procTable[i].saveReg0+procTable[i].saveReg1;
		/* printf("reg0 %d reg1 %d totalRegValue %d\n",procTable[i].saveReg0,procTable[i].saveReg1,totalRegValue);*/
	}
	for(i = 0; i < NPROC; i++ ) {
		totalProcIntArrTime += procIntArrTime[i];
		totalProcServTime += procServTime[i];
	}
	for(i = 0; i < NIOREQ; i++ ) {
		totalIOReqIntArrTime += ioReqIntArrTime[i];
		totalIOServTime += ioServTime[i];
	}
	printf("Avg Proc Inter Arrival Time : %g \tAverage Proc Service Time : %g\n", (float) totalProcIntArrTime/NPROC, (float) totalProcServTime/NPROC);
	printf("Avg IOReq Inter Arrival Time : %g \tAverage IOReq Service Time : %g\n", (float) totalIOReqIntArrTime/NIOREQ, (float) totalIOServTime/NIOREQ);
	printf("%d Process processed with %d IO requests\n", NPROC,NIOREQ);
	printf("Average Wall Clock Service Time : %g \tAverage Two Register Sum Value %g\n", (float) totalWallTime/NPROC, (float) totalRegValue/NPROC);
}

int main(int argc, char *argv[]) {
	int i;
	int totalProcServTime = 0, ioReqAvgIntArrTime;
	int SCHEDULING_METHOD, MIN_INT_ARRTIME, MAX_INT_ARRTIME, MIN_SERVTIME, MAX_SERVTIME, MIN_IO_SERVTIME, MAX_IO_SERVTIME, MIN_IOREQ_INT_ARRTIME;

	if (argc < 12) {
		printf("%s: SCHEDULING_METHOD NPROC NIOREQ QUANTUM MIN_INT_ARRTIME MAX_INT_ARRTIME MIN_SERVTIME MAX_SERVTIME MIN_IO_SERVTIME MAX_IO_SERVTIME MIN_IOREQ_INT_ARRTIME\n",argv[0]);
		exit(1);
	}
	SCHEDULING_METHOD = atoi(argv[1]);
	NPROC = atoi(argv[2]);
	NIOREQ = atoi(argv[3]);
	QUANTUM = atoi(argv[4]);
	MIN_INT_ARRTIME = atoi(argv[5]);
	MAX_INT_ARRTIME = atoi(argv[6]);
	MIN_SERVTIME = atoi(argv[7]);
	MAX_SERVTIME = atoi(argv[8]);
	MIN_IO_SERVTIME = atoi(argv[9]);
	MAX_IO_SERVTIME = atoi(argv[10]);
	MIN_IOREQ_INT_ARRTIME = atoi(argv[11]);
	printf("SIMULATION PARAMETERS : SCHEDULING_METHOD %d NPROC %d NIOREQ %d QUANTUM %d \n", SCHEDULING_METHOD, NPROC, NIOREQ, QUANTUM);
	printf("MIN_INT_ARRTIME %d MAX_INT_ARRTIME %d MIN_SERVTIME %d MAX_SERVTIME %d\n", MIN_INT_ARRTIME, MAX_INT_ARRTIME, MIN_SERVTIME, MAX_SERVTIME);
	printf("MIN_IO_SERVTIME %d MAX_IO_SERVTIME %d MIN_IOREQ_INT_ARRTIME %d\n", MIN_IO_SERVTIME, MAX_IO_SERVTIME, MIN_IOREQ_INT_ARRTIME);

	// allocate array structures
	procTable = (struct process *) malloc(sizeof(struct process) * NPROC);
	ioDoneEvent = (struct ioDoneEvent *) malloc(sizeof(struct ioDoneEvent) * NIOREQ);
	procIntArrTime = (int *) malloc(sizeof(int) * NPROC);
	procServTime = (int *) malloc(sizeof(int) * NPROC);
	ioReqIntArrTime = (int *) malloc(sizeof(int) * NIOREQ);
	ioServTime = (int *) malloc(sizeof(int) * NIOREQ);

	// initialize queues
	readyQueue.next = readyQueue.prev = &readyQueue;
	blockedQueue.next = blockedQueue.prev = &blockedQueue;
	ioDoneEventQueue.next = ioDoneEventQueue.prev = &ioDoneEventQueue;
	ioDoneEventQueue.doneTime = INT_MAX;
	ioDoneEventQueue.procid = -1;
	ioDoneEventQueue.len  = readyQueue.len = blockedQueue.len = 0;

	srandom(SEED);
	// generate process interarrival times
	for(i = 0; i < NPROC; i++ ) {
		procIntArrTime[i] = random()%(MAX_INT_ARRTIME - MIN_INT_ARRTIME+1) + MIN_INT_ARRTIME;
	}
	// assign service time for each process
	for(i=0; i < NPROC; i++) {
		procServTime[i] = random()% (MAX_SERVTIME - MIN_SERVTIME + 1) + MIN_SERVTIME;
		totalProcServTime += procServTime[i];
	}

	ioReqAvgIntArrTime = totalProcServTime/(NIOREQ+1);
	assert(ioReqAvgIntArrTime > MIN_IOREQ_INT_ARRTIME);

	// generate io request interarrival time
	for(i = 0; i < NIOREQ; i++ ) {
		ioReqIntArrTime[i] = random()%((ioReqAvgIntArrTime - MIN_IOREQ_INT_ARRTIME)*2+1) + MIN_IOREQ_INT_ARRTIME;
	}
	// generate io request service time
	for(i = 0; i < NIOREQ; i++ ) {
		ioServTime[i] = random()%(MAX_IO_SERVTIME - MIN_IO_SERVTIME+1) + MIN_IO_SERVTIME;
	}

#ifdef DEBUG
	// printing process interarrival time and service time
	printf("Process Interarrival Time :\n");
	for(i = 0; i < NPROC; i++ ) {
		printf("%d ",procIntArrTime[i]);
	}
	printf("\n");
	printf("Process Target Service Time :\n");
	for(i = 0; i < NPROC; i++ ) {
		printf("%d ",procTable[i].targetServiceTime);
	}
	printf("\n");
#endif

	// printing io request interarrival time and io request service time
	printf("IO Req Average InterArrival Time %d\n", ioReqAvgIntArrTime);
	printf("IO Req InterArrival Time range : %d ~ %d\n",MIN_IOREQ_INT_ARRTIME,
			(ioReqAvgIntArrTime - MIN_IOREQ_INT_ARRTIME) * 2 + MIN_IOREQ_INT_ARRTIME);

#ifdef DEBUG
	printf("IO Req Interarrival Time :\n");
	for(i = 0; i < NIOREQ; i++ ) {
		printf("%d ",ioReqIntArrTime[i]);
	}
	printf("\n");
	printf("IO Req Service Time :\n");
	for(i = 0; i < NIOREQ; i++ ) {
		printf("%d ",ioServTime[i]);
	}
	printf("\n");
#endif

	struct process* (*schFunc)();
	switch(SCHEDULING_METHOD) {
		case 1 : schFunc = RRschedule; break;
		case 2 : schFunc = SJFschedule; break;
		case 3 : schFunc = SRTNschedule; break;
		case 4 : schFunc = GSschedule; break;
		case 5 : schFunc = SFSschedule; break;
		default : printf("ERROR : Unknown Scheduling Method\n"); exit(1);
	}
	initProcTable();
	procExecSim(schFunc);
	printResult();
}
