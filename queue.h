#include <stdio.h>
#include <stdlib.h>

#define MAX_NODES 336
#define MAX_PATHS 50

typedef struct 
{
  int path[MAX_NODES];
  int cost;
} PathCostPair;

typedef struct 
{
  PathCostPair pairs[MAX_PATHS];
  int size;
} PriorityQueue;

void initialize(PriorityQueue *pq) 
{
  pq->size = 0;
}

void swap(PathCostPair *a, PathCostPair *b) 
{
  PathCostPair temp = *a;
  *a = *b;
  *b = temp;
}

int parent(int i) 
{
  return (i - 1) / 2;
}

int leftChild(int i) 
{
  return 2 * i + 1;
}

int rightChild(int i) 
{
  return 2 * i + 2;
}

void insert(PriorityQueue *pq, PathCostPair newPair) 
{
    if (pq->size == MAX_PATHS) 
    {
      printf("Priority Queue is full!\n");
      return;
    }

    int i = pq->size;
    pq->pairs[i] = newPair;
    pq->size++;

    while (i != 0 && pq->pairs[parent(i)].cost > pq->pairs[i].cost) 
    {
      swap(&pq->pairs[i], &pq->pairs[parent(i)]);
      i = parent(i);
    }
}

void heapify(PriorityQueue *pq, int i) 
{
    int l = leftChild(i);
    int r = rightChild(i);
    int smallest = i;

    if (l < pq->size && pq->pairs[l].cost < pq->pairs[i].cost)
        smallest = l;

    if (r < pq->size && pq->pairs[r].cost < pq->pairs[smallest].cost)
        smallest = r;

    if (smallest != i) 
    {
      swap(&pq->pairs[i], &pq->pairs[smallest]);
      heapify(pq, smallest);
    }
}

PathCostPair extractMin(PriorityQueue *pq) 
{
  if (pq->size <= 0) 
  {
    printf("Priority Queue is empty!\n");
    exit(1);
  }

  if (pq->size == 1) 
  {
    pq->size--;
    return pq->pairs[0];
  }

  PathCostPair root = pq->pairs[0];
  pq->pairs[0] = pq->pairs[pq->size - 1];
  pq->size--;
  heapify(pq, 0);

  return root;
}