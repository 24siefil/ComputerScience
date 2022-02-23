#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define N 5757
#define POOL_SIZE 6*5757

char word[N][5];
char adj_mat[N][N];

void print_word();
void init_data();
void init_adj_mat();

struct node {
  int index;
  struct node * next;
};

struct node pool[POOL_SIZE];
struct node * top=pool;

void init_pool()
{
  int i;
  struct node *r=pool;
  struct node *s;

  pool[POOL_SIZE-1].next=NULL;

  for(i=1;i<POOL_SIZE;i++) {
    s=r++;
    s->next=r;
  }
}

struct node * new_node()
{
  struct node *r;

  if(top==NULL)
    return NULL;

  r=top;
  top=r->next;
  return r;
}

void free_node(struct node *r)
{
  r->next=top;
  top=r;
}

/***************** adjacency list *******************/
struct node * adj_list[N];

void init_adj_list()
{
  int i,j;
  struct node *p;

  for (i=0; i<N; i++)
    adj_list[i]=NULL;
  for (i=0; i<N; i++)
    for (j=N-1; j>=0; j--)
      if (adj_mat[i][j])
      {
	      p=new_node();
	      p->index=j;
	      p->next=adj_list[i];
	      adj_list[i]=p;
      }
  printf("Adjacency list was successfully constructed!\n");
}

void init_data()
{
  int i;
  FILE *f;

  if ((f=fopen("words.dat", "r")) == NULL)
    printf("Sorry, I can't open words.dat.\n");

  // skip the first four lines
  for (i=1; i<=4; i++) {
    while (getc(f)!='\n');
  }

  // start reading data
  for (i=0; i<N; i++) {
    fscanf(f, "%5c", word[i]);
    while (getc(f)!='\n'); // move fd
  }

  printf("All the words were successfully read!\n");
  fclose(f);
}

int adjacent(char u[5], char v[5])
{
  int i, check=0;

  for (i=0; i<5; i++)
    if (u[i]!=v[i]) check++;
  if (check==1)
    return 1;
  else
    return 0;
}

void init_adj_mat()
{
  int i,j,l;

  for (i=0;i<N;i++)
    for (j=0;j<N;j++)
      adj_mat[i][j]=adjacent(word[i],word[j]);
  printf("Adjacency matrix was successfully constructed!\n");
}

void print_word(int k)
{
  int i;
  if(0<=k && k<N) {
    for (i=0; i<5; i++)
      putchar(word[k][i]);
  }
  else
    printf("ERROR: print_word() received a k=%d which is out of bound!\n", k);
}

int compare(char u[5], char v[5])
{
  return strncmp(u, v, 5);
}

int search_index(char key[5])
{
  int i;

  for (i=0; i<N; i++) {
    if(compare(key,word[i])==0)
      return i;
  }
  return -1;
}

/********** Written Homework 5 *****************/
void  whw5(void)
{
  // Question 1.
  printf("==================== Question 1-(a) ====================\n");
  int         idx;
  int         deg;
  struct node *cur;

  idx = search_index("hello");
  cur = adj_list[idx];
  deg = 0;
  printf("All the words adjacent to hello: ");
  while (cur)
  {
    print_word(cur->index);
    printf(" ");
    ++deg;
    cur = cur->next;
  }
  printf("\nThe degree of hello: %d\n", deg);

  printf("\n\n==================== Question 1-(b) ====================\n");
  idx = search_index("graph");
  cur = adj_list[idx];
  deg = 0;
  printf("All the words adjacent to hello: ");
  while (cur)
  {
    print_word(cur->index);
    printf(" ");
    ++deg;
    cur = cur->next;
  }
  printf("\nThe degree of graph: %d\n", deg);

  // Question 2.
  printf("\n\n==================== Question 2 ====================\n");
  int map_freq[50] = {0, };
  int map_deg[N] = {0, };
  int i;
  int freq;
  int max_deg;

  i = 0;
  while (i < N)
  {
    cur = adj_list[i];
    freq = 0;
    while (cur)
    {
      ++freq;
      cur = cur->next;
    }
    ++map_freq[freq];
    map_deg[i] = freq;
    ++i;
  }
  i = 0;
  printf("The table of distribution of degrees\n");
  while (i < 50)
  {
    if (map_freq[i] != 0)
    {
      printf("%d:\t%d\n", i, map_freq[i]);
      max_deg = i;
    }
    ++i;
  }

  // Question 3.
  printf("\n\n==================== Question 3 ====================\n");
  printf("The maximum degree: %d\n", max_deg);

  // Question 4.
  printf("\n\n==================== Question 4 ====================\n");
  i = 0;
  while (i < N)
  {
    if (map_deg[i] == max_deg)
    {
      print_word(i);
      printf(" ");
    }
    printf("\n");
    ++i;
  }

  // Question 5.
  printf("\n\n==================== Question 5 ====================\n");
  int sum;

  i = 0;
  sum = 0;
  while (i < 50)
  {
    sum += i * map_freq[i];
    ++i;
  }
  printf("The average degree: %f\n", (float)sum / N);

  // Question 6.
  printf("\n\n==================== Question 6 ====================\n");
  sum = 0;
  i = 0;
  while (i < N)
  {
    sum += map_deg[i];
    ++i;
  }
  printf("Our adjacency list has %d nodes\n", sum);
}

void init()
{
  init_data();
  init_adj_mat();
  init_pool();
  init_adj_list();
  whw5();
}

void find_path(char start[5], char goal[5])
{
  int i,j,k,l;

  i=search_index(start); // get idx of the word // if not found ret -1
  j=search_index(goal);

  if (i<0) printf("Sorry. %5s is not in the dictionary.", start);
  else if (j<0) printf("Sorry. %5s is not in the dictionary.", goal);
  else
  {
    printf("Hmm... I am trying to figure out the shortest path from ");
    print_word(i); printf(" to "); print_word(j); printf(".\n"); // print word == print ith word
    for (l=0; l<150; l++)
    {
      for (k=0; k<N; k++)
      {
	      printf("Considering about ");
	      print_word(k);
	      printf("\r"); fflush(stdout); // car ret
      }
    }
    printf("\nWell..., I don't know.  Please enlighten me ;)\n");
  }
}
