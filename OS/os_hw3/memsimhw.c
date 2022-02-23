//
// Virual Memory Simulator Homework
// One-level page table system with FIFO and LRU
// Two-level page table system with LRU
// Inverted page table with a hashing system
// Submission Year: 2021
// Student Name: 신성우
// Student Number: B617065
//
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>

#define PAGESIZEBITS 12			// page size = 4Kbytes
#define VIRTUALADDRBITS 32		// virtual address space size = 4Gbytes

typedef struct s_trace
{
	size_t			addr;
	char			rw;
	struct s_trace	*nxt;
}					t_trace;

typedef struct s_procEntry
{
	char *traceName;			// the memory trace name
	int pid;					// process (trace) id
	int ntraces;				// the number of memory traces
	int num2ndLevelPageTable;	// The 2nd level page created(allocated);
	int numIHTConflictAccess; 	// The number of Inverted Hash Table Conflict Accesses
	int numIHTNULLAccess;		// The number of Empty Inverted Hash Table Accesses
	int numIHTNonNULLAcess;		// The number of Non Empty Inverted Hash Table Accesses
	int numPageFault;			// The number of page faults
	int numPageHit;				// The number of page hits
	struct pageTableEntry *firstLevelPageTable;
	FILE *tracefp;
	t_trace	*traces_f;
	t_trace	*traces_r;
	int		*ptable;
	int		**fir_ptable;
}			t_procEntry;

typedef struct s_pframe
{
	int				pid;
	long long		ptable_idx;
	long long		fir_ptable_idx;
	long long		sec_ptable_idx;
	long long		vpage_nbr;
	int				hash;
	int				pframe_nbr;
	struct s_pframe	*nxt;
}					t_pframe;

t_procEntry	*g_procTable;
t_pframe	*g_rdy_q_f;
t_pframe	*g_rdy_q_r;

void	create_ptable(int numProcess)
{
	int	i;

	i = 0;
	while (i < numProcess)
	{
		g_procTable[i].ptable = (int *)malloc(sizeof(int) * 2 * (0xfffff + 1));
		if (g_procTable[i].ptable == NULL)
			exit(1);
		memset(g_procTable[i].ptable, 0, sizeof(int) * 2 * (0xfffff + 1));
		++i;
	}
}

int	get_new_pframe_nbr_fifo(void)
{
	int	new_pframe_nbr;

	new_pframe_nbr = g_rdy_q_f->pframe_nbr;
	g_rdy_q_f = g_rdy_q_f->nxt;
	return (new_pframe_nbr);
}

void	update_old_mapping(int old_pid, size_t old_ptable_idx)
{
	if (old_pid != -1)
	{
		(g_procTable[old_pid].ptable)[2 * old_ptable_idx] = 0;
		(g_procTable[old_pid].ptable)[2 * old_ptable_idx + 1] = 0;
	}
}

void	update_new_mapping(int i, size_t new_ptable_idx, int new_pframe_nbr)
{
	(g_procTable[i].ptable)[2 * new_ptable_idx] = new_pframe_nbr;
	(g_procTable[i].ptable)[2 * new_ptable_idx + 1] = 1;
}

int	get_new_pframe_nbr_lru(int hitted_pframe_nbr)
{
	int			new_pframe_nbr;
	t_pframe	*trail;
	t_pframe	*cur;

	new_pframe_nbr = g_rdy_q_f->pframe_nbr;
	if (g_rdy_q_f == g_rdy_q_r) // one and only node
		;
	else if (hitted_pframe_nbr == -1) // fault
	{
		g_rdy_q_r->nxt = g_rdy_q_f;
		g_rdy_q_r = g_rdy_q_f;
		g_rdy_q_f = g_rdy_q_f->nxt;
		g_rdy_q_r->nxt = NULL;
	}
	else // hit
	{
		trail = g_rdy_q_f;
		cur = g_rdy_q_f;
		while (cur)
		{
			if (cur->pframe_nbr == hitted_pframe_nbr)
				break ;
			trail = cur;
			cur = cur->nxt;
		}
		if (cur == g_rdy_q_f)
			g_rdy_q_f = g_rdy_q_f->nxt;
		if (cur != g_rdy_q_r)
		{
			g_rdy_q_r->nxt = cur;
			g_rdy_q_r = cur;
			trail->nxt = cur->nxt;
			g_rdy_q_r->nxt = NULL;
		}
	}
	return (new_pframe_nbr);
}

void	oneLevelVMSim(int simType, int numProcess)
{
	bool		flag_fin;
	int			i;
	int			new_pframe_nbr;
	int			dummy;
	int			old_pid;
	size_t		va;
	size_t		new_ptable_idx;
	t_trace		*trail;
	long long	old_ptable_idx;

	if (simType == 0)
		g_rdy_q_r->nxt = g_rdy_q_f; // circular queue
	create_ptable(numProcess);
	flag_fin = false;
	while (1)
	{
		if (flag_fin == true)
			break ;
		i = 0;
		while (i < numProcess)
		{
			if (g_procTable[i].traces_f == NULL)
			{
				flag_fin = true;
				break ;
			}
			++(g_procTable[i].ntraces);
			va = (g_procTable[i].traces_f)->addr;
			trail = g_procTable[i].traces_f;
			g_procTable[i].traces_f = (g_procTable[i].traces_f)->nxt;
			free(trail);
			new_ptable_idx = (va & 0xfffff000) >> 12;
			if ((g_procTable[i].ptable)[2 * new_ptable_idx + 1] == 0)
			{
				++(g_procTable[i].numPageFault);
				old_pid = g_rdy_q_f->pid;
				old_ptable_idx = g_rdy_q_f->ptable_idx;
				g_rdy_q_f->pid = i; // new pid
				g_rdy_q_f->ptable_idx = new_ptable_idx;
				if (simType == 0)
					new_pframe_nbr = get_new_pframe_nbr_fifo();
				else
					new_pframe_nbr = get_new_pframe_nbr_lru(-1);
				update_old_mapping(old_pid, old_ptable_idx);
				update_new_mapping(i, new_ptable_idx, new_pframe_nbr);
			}
			else
			{
				++(g_procTable[i].numPageHit);
				if (simType == 1)
					dummy = get_new_pframe_nbr_lru((g_procTable[i].ptable)[2 * new_ptable_idx]);
			}
			++i;
		}
	}
	for(i = 0; i < numProcess; ++i)
	{
		printf("**** %s *****\n",g_procTable[i].traceName);
		printf("Proc %d Num of traces %d\n",i,g_procTable[i].ntraces);
		printf("Proc %d Num of Page Faults %d\n",i,g_procTable[i].numPageFault);
		printf("Proc %d Num of Page Hit %d\n",i,g_procTable[i].numPageHit);
		assert(g_procTable[i].numPageHit + g_procTable[i].numPageFault == g_procTable[i].ntraces);
	}
}

size_t	get_ptable_max_idx(int level_bits)
{
	int		i;
	size_t	bit_mask;
	size_t	ptable_max_idx;

	i = 0;
	ptable_max_idx = 0;
	bit_mask = 1;
	while (i < level_bits)
	{
		ptable_max_idx |= bit_mask;
		bit_mask <<= 1;
		++i;
	}
	return (ptable_max_idx);
}

void	create_fir_ptable(int numProcess, size_t fir_ptable_max_idx)
{
	int	i;

	i = 0;
	while (i < numProcess)
	{
		g_procTable[i].fir_ptable = (int **)malloc(sizeof(int *) * (fir_ptable_max_idx + 1));
		if (g_procTable[i].fir_ptable == NULL)
			exit(1);
		memset(g_procTable[i].fir_ptable, 0, sizeof(int *) * (fir_ptable_max_idx + 1));
		++i;
	}
}

void	create_sec_ptable(int i, size_t new_fir_ptable_idx, size_t sec_ptable_max_idx)
{
	(g_procTable[i].fir_ptable)[new_fir_ptable_idx] = (int *)malloc(sizeof(int) * 2 * (sec_ptable_max_idx + 1));
	if ((g_procTable[i].fir_ptable)[new_fir_ptable_idx] == NULL)
		exit(1);
	memset((g_procTable[i].fir_ptable)[new_fir_ptable_idx], 0, sizeof(int) * 2 * (sec_ptable_max_idx + 1));
}

void	update_old_mapping_two_level(int old_pid, long long old_fir_ptable_idx, long long old_sec_ptable_idx)
{
	if (old_pid != -1)
	{
		(g_procTable[old_pid].fir_ptable)[old_fir_ptable_idx][2 * old_sec_ptable_idx] = 0;
		(g_procTable[old_pid].fir_ptable)[old_fir_ptable_idx][2 * old_sec_ptable_idx + 1] = 0;
	}
}

void	update_new_mapping_two_level(int i, size_t new_fir_ptable_idx, size_t new_sec_ptable_idx, int new_pframe_nbr)
{
	(g_procTable[i].fir_ptable)[new_fir_ptable_idx][2 * new_sec_ptable_idx] = new_pframe_nbr;
	(g_procTable[i].fir_ptable)[new_fir_ptable_idx][2 * new_sec_ptable_idx + 1] = 1;
}

void	twoLevelVMSim(int numProcess, int fir_bits)
{
	int			sec_bits;
	int			i;
	int			old_pid;
	int			new_pframe_nbr;
	int			dummy;
	bool		flag_fin;
	size_t		va;
	size_t		fir_ptable_max_idx;
	size_t		sec_ptable_max_idx;
	size_t		new_fir_ptable_idx;
	size_t		new_sec_ptable_idx;
	t_trace		*trail;
	long long	old_fir_ptable_idx;
	long long	old_sec_ptable_idx;

	sec_bits = 32 - 12 - fir_bits;
	fir_ptable_max_idx = get_ptable_max_idx(fir_bits);
	sec_ptable_max_idx = get_ptable_max_idx(sec_bits);
	create_fir_ptable(numProcess, fir_ptable_max_idx);
	flag_fin = false;
	while (1)
	{
		if (flag_fin == true)
			break ;
		i = 0;
		while (i < numProcess)
		{
			if (g_procTable[i].traces_f == NULL)
			{
				flag_fin = true;
				break ;
			}
			++(g_procTable[i].ntraces);
			va = (g_procTable[i].traces_f)->addr;
			trail = g_procTable[i].traces_f;
			g_procTable[i].traces_f = (g_procTable[i].traces_f)->nxt;
			free(trail);
			new_fir_ptable_idx = (va & (fir_ptable_max_idx << (sec_bits + 12))) >> (sec_bits + 12);
			new_sec_ptable_idx = (va & (sec_ptable_max_idx << 12)) >> 12;
			if ((g_procTable[i].fir_ptable)[new_fir_ptable_idx] != NULL && \
				(g_procTable[i].fir_ptable)[new_fir_ptable_idx][2 * new_sec_ptable_idx + 1] == 1)
			{
				++(g_procTable[i].numPageHit);
				dummy = get_new_pframe_nbr_lru((g_procTable[i].fir_ptable)[new_fir_ptable_idx][2 * new_sec_ptable_idx]);
			}
			else
			{
				++(g_procTable[i].numPageFault);
				old_pid = g_rdy_q_f->pid;
				old_fir_ptable_idx = g_rdy_q_f->fir_ptable_idx;
				old_sec_ptable_idx = g_rdy_q_f->sec_ptable_idx;
				g_rdy_q_f->pid = i;
				g_rdy_q_f->fir_ptable_idx = new_fir_ptable_idx;
				g_rdy_q_f->sec_ptable_idx = new_sec_ptable_idx;
				if ((g_procTable[i].fir_ptable)[new_fir_ptable_idx] == NULL)
				{
					++(g_procTable[i].num2ndLevelPageTable);
					create_sec_ptable(i, new_fir_ptable_idx, sec_ptable_max_idx);
				}
				new_pframe_nbr = get_new_pframe_nbr_lru(-1);
				update_old_mapping_two_level(old_pid, old_fir_ptable_idx, old_sec_ptable_idx);
				update_new_mapping_two_level(i, new_fir_ptable_idx, new_sec_ptable_idx, new_pframe_nbr);
			}
			++i;
		}
	}
	for(i = 0; i < numProcess; ++i)
	{
		printf("**** %s *****\n",g_procTable[i].traceName);
		printf("Proc %d Num of traces %d\n",i,g_procTable[i].ntraces);
		printf("Proc %d Num of second level page tables allocated %d\n",i,g_procTable[i].num2ndLevelPageTable);
		printf("Proc %d Num of Page Faults %d\n",i,g_procTable[i].numPageFault);
		printf("Proc %d Num of Page Hit %d\n",i,g_procTable[i].numPageHit);
		assert(g_procTable[i].numPageHit + g_procTable[i].numPageFault == g_procTable[i].ntraces);
	}
}

typedef struct s_inv_htable
{
	int					pid;
	size_t				vpage_nbr;
	int					pframe_nbr;
	struct s_inv_htable	*nxt;
}						t_inv_htable;

t_inv_htable	**g_inv_htable;

void	create_inv_htable(int nFrame)
{
	g_inv_htable = (t_inv_htable **)malloc(sizeof(t_inv_htable *) * nFrame);
	if (g_inv_htable == NULL)
		exit(1);
	memset(g_inv_htable, 0, sizeof(t_inv_htable *) * nFrame);
}

int	get_new_hash(int i, size_t new_vpage_nbr, int nFrame)
{
	return ((i + new_vpage_nbr) % nFrame);
}

void	push_new_node(int new_hash, int i, size_t new_vpage_nbr, size_t new_pframe_nbr)
{
	t_inv_htable	*new_node;
	t_inv_htable	*cur;

	new_node = (t_inv_htable *)malloc(sizeof(t_inv_htable) * 1);
	if (new_node == NULL)
		exit(1);
	new_node->pid = i;
	new_node->vpage_nbr = new_vpage_nbr;
	new_node->pframe_nbr = new_pframe_nbr;
	new_node->nxt = NULL;
	if (g_inv_htable[new_hash] == NULL)
		g_inv_htable[new_hash] = new_node;
	else
	{
		new_node->nxt = g_inv_htable[new_hash];
		g_inv_htable[new_hash] = new_node;
	}
}

int	get_old_pframe_nbr(int i, size_t new_vpage_nbr, int new_hash, int new_pframe_nbr)
{
	t_inv_htable	*cur;

	if (g_inv_htable[new_hash] == NULL)
		++(g_procTable[i].numIHTNULLAccess);
	else
	{
		++(g_procTable[i].numIHTNonNULLAcess);
		cur = g_inv_htable[new_hash];
		while (cur)
		{
			++(g_procTable[i].numIHTConflictAccess);
			if (cur->pid == i && cur->vpage_nbr == new_vpage_nbr)
				return (cur->pframe_nbr);
			cur = cur->nxt;
		}
	}
	return (-1);
}

void	update_old_mapping_inv(int old_pid, int old_hash, int new_pframe_nbr)
{
	t_inv_htable	*trail;
	t_inv_htable	*cur;

	if (old_pid != -1) // init cond
	{
		trail = g_inv_htable[old_hash];
		cur = g_inv_htable[old_hash];
		while (cur)
		{
			if (cur->pframe_nbr == new_pframe_nbr)
			{
				if (trail == cur) // found in the 1st node
				{
					if (cur->nxt == NULL) // one and only one
					{
						free(cur);
						g_inv_htable[old_hash] = NULL;
					}
					else
					{
						g_inv_htable[old_hash] = cur->nxt;
						free(cur);
					}
				}
				else
				{
					trail->nxt = cur->nxt;
					free(cur);
				}
				break ;
			}
			trail = cur;
			cur = cur->nxt;
		}
	}
}

void invertedPageVMSim(int numProcess, int nFrame)
{
	bool		flag_fin;
	int			i;
	int			new_pframe_nbr;
	int			dummy;
	int			old_pid;
	int			old_hash;
	int			new_hash;
	int			old_pframe_nbr;
	long long	old_vpage_nbr;
	t_trace		*trail;
	size_t		va;
	size_t		new_vpage_nbr;

	create_inv_htable(nFrame);
	flag_fin = false;
	while (1)
	{
		if (flag_fin == true)
			break ;
		i = 0;
		while (i < numProcess)
		{
			if (g_procTable[i].traces_f == NULL)
			{
				flag_fin = true;
				break ;
			}
			++(g_procTable[i].ntraces);
			va = (g_procTable[i].traces_f)->addr;
			trail = g_procTable[i].traces_f;
			g_procTable[i].traces_f = (g_procTable[i].traces_f)->nxt;
			free(trail);
			new_vpage_nbr = (va & 0xfffff000) >> 12;
			new_hash = get_new_hash(i, new_vpage_nbr, nFrame);
			old_pid = g_rdy_q_f->pid; // init: -1
			old_vpage_nbr = g_rdy_q_f->vpage_nbr;
			old_hash = g_rdy_q_f->hash;
			old_pframe_nbr = get_old_pframe_nbr(i, new_vpage_nbr, new_hash, new_pframe_nbr); // check old mapping
			if (old_pframe_nbr == -1) // not found
			{
				++(g_procTable[i].numPageFault);
				g_rdy_q_f->pid = i;
				g_rdy_q_f->vpage_nbr = new_vpage_nbr;
				g_rdy_q_f->hash = new_hash;
				new_pframe_nbr = get_new_pframe_nbr_lru(-1);
				update_old_mapping_inv(old_pid, old_hash, new_pframe_nbr);
				push_new_node(new_hash, i, new_vpage_nbr, new_pframe_nbr);
			}
			else
			{
				++(g_procTable[i].numPageHit);
				dummy = get_new_pframe_nbr_lru(old_pframe_nbr);
			}
			++i;
		}
	}
	for(i = 0; i < numProcess; ++i)
	{
		printf("**** %s *****\n",g_procTable[i].traceName);
		printf("Proc %d Num of traces %d\n",i,g_procTable[i].ntraces);
		printf("Proc %d Num of Inverted Hash Table Access Conflicts %d\n",i,g_procTable[i].numIHTConflictAccess);
		printf("Proc %d Num of Empty Inverted Hash Table Access %d\n",i,g_procTable[i].numIHTNULLAccess);
		printf("Proc %d Num of Non-Empty Inverted Hash Table Access %d\n",i,g_procTable[i].numIHTNonNULLAcess);
		printf("Proc %d Num of Page Faults %d\n",i,g_procTable[i].numPageFault);
		printf("Proc %d Num of Page Hit %d\n",i,g_procTable[i].numPageHit);
		assert(g_procTable[i].numPageHit + g_procTable[i].numPageFault == g_procTable[i].ntraces);
		assert(g_procTable[i].numIHTNULLAccess + g_procTable[i].numIHTNonNULLAcess == g_procTable[i].ntraces);
	}
}

void	init_proc_table(int numProcess)
{
	int	i;

	i = 0;
	while (i < numProcess)
	{
		g_procTable[i].pid = 0;
		g_procTable[i].ntraces = 0;
		g_procTable[i].num2ndLevelPageTable = 0;
		g_procTable[i].numIHTConflictAccess = 0;
		g_procTable[i].numIHTNULLAccess = 0;
		g_procTable[i].numIHTNonNULLAcess = 0;
		g_procTable[i].numPageFault = 0;
		g_procTable[i].numPageHit = 0;
		g_procTable[i].firstLevelPageTable = NULL;
		++i;
	}
}

void	read_file(int i)
{
	size_t	addr;
	char	rw;
	t_trace	*buf;

	g_procTable[i].traces_f = NULL;
	while (fscanf(g_procTable[i].tracefp, "%zx %c", &addr, &rw) != EOF) // https://www.delftstack.com/ko/howto/c/fscanf-line-by-line-in-c/
	{
		buf = (t_trace *)malloc(sizeof(t_trace) * 1);
		if (buf == NULL)
			exit(1);
		buf->addr = addr;
		buf->rw = rw;
		buf->nxt = NULL;
		if (g_procTable[i].traces_f == NULL)
		{
			g_procTable[i].traces_f = buf;
			g_procTable[i].traces_r = buf;
		}
		else
		{
			g_procTable[i].traces_r->nxt = buf;
			g_procTable[i].traces_r = (g_procTable[i].traces_r)->nxt;
		}
	}
}

void	init_pframe_rdy_q(int nFrame)
{
	int			i;
	t_pframe	*buf;
	t_pframe	*cur;

	i = 0;
	g_rdy_q_f = NULL;
	g_rdy_q_r = NULL;
	while (i < nFrame)
	{
		buf = (t_pframe *)malloc(sizeof(t_pframe) * 1);
		if (buf == NULL)
			exit(1);
		buf->pid = -1;
		buf->ptable_idx = -1;
		buf->fir_ptable_idx = -1;
		buf->sec_ptable_idx = -1;
		buf->vpage_nbr = -1;
		buf->hash = -1;
		buf->pframe_nbr = i;
		buf->nxt = NULL;
		if (g_rdy_q_f == NULL)
		{
			g_rdy_q_f = buf;
			g_rdy_q_r = buf;
		}
		else
		{
			g_rdy_q_r->nxt = buf;
			g_rdy_q_r = g_rdy_q_r->nxt;
		}
		++i;
	}
}

int main(int argc, char *argv[])
{
	int	simType;
	int	numProcess;
	int	i;
	int	phyMemSizeBits;
	int	nFrame;

	simType = atoi(argv[1]);
	numProcess = argc - 4;
	g_procTable = (t_procEntry *)malloc(sizeof(t_procEntry) * numProcess);
	if (g_procTable == NULL)
		return (1);
	init_proc_table(numProcess);
	i = 0;
	while (i < numProcess)
	{
		g_procTable[i].traceName = argv[i + optind + 3];
		g_procTable[i].pid = i;
		printf("process %d opening %s\n", i, argv[i + optind + 3]);
		g_procTable[i].tracefp = fopen(argv[i + optind + 3], "r");
		if (g_procTable[i].tracefp == NULL)
		{
			printf("ERROR: can't open %s file; exiting...",argv[i + optind + 3]);
			exit(1);
		}
		read_file(i);
		fclose(g_procTable[i].tracefp);
		++i;
	}
	phyMemSizeBits = atoi(argv[3]);
	nFrame = (1L<<phyMemSizeBits) / (1L<<12);
	printf("Num of Frames %d Physical Memory Size %ld bytes\n", nFrame, (1L<<phyMemSizeBits));
	init_pframe_rdy_q(nFrame);
	if (simType == 0)
	{
		printf("=============================================================\n");
		printf("The One-Level Page Table with FIFO Memory Simulation Starts .....\n");
		printf("=============================================================\n");
		oneLevelVMSim(simType, numProcess);
	}
	if (simType == 1)
	{
		printf("=============================================================\n");
		printf("The One-Level Page Table with LRU Memory Simulation Starts .....\n");
		printf("=============================================================\n");
		oneLevelVMSim(simType, numProcess);
	}
	if (simType == 2)
	{
		printf("=============================================================\n");
		printf("The Two-Level Page Table Memory Simulation Starts .....\n");
		printf("=============================================================\n");
		twoLevelVMSim(numProcess, atoi(argv[2]));
	}
	if (simType == 3)
	{
		printf("=============================================================\n");
		printf("The Inverted Page Table Memory Simulation Starts .....\n");
		printf("=============================================================\n");
		invertedPageVMSim(numProcess, nFrame);
	}
	return (0);
}
