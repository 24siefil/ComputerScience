#include "backend.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <memory.h>

struct node *g_q_f = NULL;
struct node *g_q_r;
char        g_state[N] = {0, }; // all undiscovered
int         g_parent[N];

void  push(int i)
{
  struct node *new;

  new = (struct node *)malloc(sizeof(struct node) * 1);
  if (new == NULL)
    exit(1);
  new->index = i;
  new->next = NULL;
  if (g_q_f == NULL)
    g_q_f = new;
  else
    g_q_r->next = new;
  g_q_r = new;
}

int pop()
{
  struct node *tmp;
  int         ret;

  if (g_q_f == NULL)
    return (-1);
  tmp = g_q_f;
  ret = tmp->index;
  g_q_f = tmp->next;
  free(tmp);
  return (ret);
}

bool  process_bfs(int i, int j) // path from i to j
{
  int         cur;
  struct node *row;

  g_state[i] = 1; // discoverd
  g_parent[i] = -1; // root
  push(i);
  while (g_q_f)
  {
    cur = pop();
    row = adj_list[cur];
    while (row)
    {
      if (g_state[row->index] == 0) // undiscovered
      {
        g_state[row->index] = 1; // discovered
        push(row->index);
        g_parent[row->index] = cur;
        if (row->index == j)
          return (true);
      }
      row = row->next;
    }
    g_state[cur] = 2; // proccessed
  }
  return (false);
}

void  print_path(int i, int j) // path from i to j
{
  int path[N];
  int v;
  int idx;
  int cnt;

  v = j;
  idx = 0;
  while (v != i)
  {
    path[idx] = v;
    ++idx;
    v = g_parent[v];
  }
  path[idx] = i;
  cnt = 0;
  while (idx >= 0)
  {
    printf("          %d ", cnt);
    print_word(path[idx]);
		printf("\n");
    ++cnt;
    --idx;
  }
}

void find_path(char start[5], char goal[5])
{
  int i, j, k, l;

  i = search_index(start);
  j = search_index(goal);
  if (i < 0)
  {
    printf("Sorry. ");
    print_five_chars(start);
    printf(" is not in the dicitonary.");
  }
  else if (j < 0)
  {
    printf("Sorry. ");
    print_five_chars(goal);
    printf(" is not in the dicitonary.");
  }
  else
  {
    if (process_bfs(i, j) == true)
      print_path(i, j);
    else
    {
      printf("Sorry. There is no path from ");
      print_five_chars(start);
      printf(" to ");
      print_five_chars(goal);
      printf(".\n");
    }
    g_q_f = NULL;
    memset(g_state, 0, sizeof(char) * N);
    memset(g_parent, 0, sizeof(int) * N);
  }
}

