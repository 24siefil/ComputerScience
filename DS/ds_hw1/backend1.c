#include <stdio.h>
#include "backend1.h"

#define NUMBER_OF_RECORDS 5

static char g_data_name[NUMBER_OF_RECORDS][3];
static char g_data_number[NUMBER_OF_RECORDS][4];
static int g_idx = 0; // index of the first empty slot

static void print_data(char * s, int n)
{
  int i;

  for (i = 0; i < n; ++i)
    putchar(s[i]);
}

static void print_name(int i)
{
  print_data(g_data_name[i], 3);
}

static void print_number(int i)
{
  print_data(g_data_number[i], 4);
}

void add(char name[3], char number[4])
{
  if (g_idx < NUMBER_OF_RECORDS) {
    g_data_name[g_idx][0]=name[0];
    g_data_name[g_idx][1]=name[1];
    g_data_name[g_idx][2]=name[2];
    g_data_number[g_idx][0]=number[0];
    g_data_number[g_idx][1]=number[1];
    g_data_number[g_idx][2]=number[2];
    g_data_number[g_idx][3]=number[3];
    print_name(g_idx);
    printf(" : ");
    print_number(g_idx);
    printf(" was successfully added!\n");
    ++g_idx;
  }
  else
    printf("Can't add.  Address book is full.\n");
}

static int search_index(char name[3])
{
  int i;

  i = 0;
  while (i < g_idx) {
    if (g_data_name[i][0] == name[0])
      if (g_data_name[i][1] == name[1])
        if (g_data_name[i][2] == name[2])
          return (i);
    ++i;
  }
  return (-1);
}

void search(char name[3])
{
  int s_result;

  s_result = search_index(name);
  if (s_result == -1)
    printf("Couldn't find the name.\n");
  else{
    print_name(s_result);
    printf(" : ");
    print_number(s_result);
    printf(" was found.\n");
  }
}

void delete(char name[3])
{
  int idx_to_del;
  int i;

  idx_to_del = search_index(name);
  i = idx_to_del;
  while (i < g_idx - 1) {
    g_data_name[i][0] = g_data_name[i + 1][0];
    g_data_name[i][1] = g_data_name[i + 1][1];
    g_data_name[i][2] = g_data_name[i + 1][2];
    ++i;
  }
  i = idx_to_del;
  while (i < g_idx - 1) {
    g_data_number[i][0] = g_data_number[i + 1][0];
    g_data_number[i][1] = g_data_number[i + 1][1];
    g_data_number[i][2] = g_data_number[i + 1][2];
    g_data_number[i][3] = g_data_number[i + 1][3];
    ++i;
  }
  --g_idx;
  printf("The name was successfully deleted.\n");
}

void print_list()
{
  int i;

  for (i = 0; i < g_idx; ++i) {
    print_name(i);
    printf(" : ");
    print_number(i);
    printf("\n");
  }
}
