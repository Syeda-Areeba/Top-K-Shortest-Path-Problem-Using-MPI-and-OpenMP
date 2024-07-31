
#include <stdio.h>
#include <stdlib.h>
#include "queue.h"
#include <limits.h>
#include <time.h>

#define INF 1000000
#define MAX_NODES 336
#define RAND_PAIRS 10

void remove_edge(int i, int j, int *graph[MAX_NODES]) 
{
  graph[i][j] = INF;
  graph[j][i] = INF;
}

void print_path(int path[]) 
{
  printf("|| Path ||\n");
  for (int i = 0; i < MAX_NODES; i++) 
  {
    if (path[i] != -1)
      printf("%d -> ", path[i]);
  }
}

void copy_path(int path[], int path_copy[]) 
{
  for (int i = 0; i < MAX_NODES; i++) {
    path_copy[i] = path[i];
  }
}

void save_shortest_path(int source, int sink, int predecessors[], int path[]) 
{
  int index = 0;
  for (int v = sink; v != source; v = predecessors[v]) 
  {
    path[index++] = v;
  }
  path[index] = source;

  // Reverse the path
  int temp = 0, start = 0;
  while (start < index) {
    temp = path[start];
    path[start] = path[index];
    path[index] = temp;
    start++;
    index--;
  }
}

int get_path_length(int path[]) 
{
  int len = 0;
  for (int i = 0; i < MAX_NODES; i++) 
  {
    if (path[i] != -1)
      len++;
  }
  return len;
}

int get_min_vertex(int distances[], int visited[]) 
{
  int min = INF, min_index = -1;

  for (int vertex = 0; vertex < MAX_NODES; vertex++) 
  {
    if (visited[vertex] == 0 && distances[vertex] <= min) 
    {
      min = distances[vertex];
      min_index = vertex;
    }
  }
  return min_index;
}

int dijkstra(int source, int sink, int *graph[MAX_NODES], int shortest_path[]) 
{
  int visited[MAX_NODES];
  int distance[MAX_NODES];
  int predecessors[MAX_NODES];

  for (int i = 0; i < MAX_NODES; i++) 
  {
    distance[i] = INF;
    visited[i] = 0;
    shortest_path[i] = -1;
    predecessors[i] = -1;
  }

  distance[source] = 0;

  for (int i = 0; i < MAX_NODES - 1; i++) 
  {
    int curr_min_vertex = get_min_vertex(distance, visited);
    visited[curr_min_vertex] = 1;

    for (int adj_v = 0; adj_v < MAX_NODES; adj_v++) 
    {
      //dist[curr_min_vertex] != INF, if node is unreachable, its dist is INF 
      if (visited[adj_v] != 1 && graph[curr_min_vertex][adj_v] && distance[curr_min_vertex] != INF && distance[curr_min_vertex] + graph[curr_min_vertex][adj_v] < distance[adj_v]) 
      {
        distance[adj_v] = distance[curr_min_vertex] + graph[curr_min_vertex][adj_v];
        predecessors[adj_v] = curr_min_vertex;
      }
    }
  }

  if (distance[sink] != INF) 
  {
    save_shortest_path(source, sink, predecessors, shortest_path);
  }

  return distance[sink];
}

int check_same_path(int path1[], int path2[]) 
{
  for (int i = 0; i < MAX_NODES; i++) 
  {
    if (path1[i] != path2[i])
      return 0;
  }
  return 1;
}

void extract_sub_path(int path[], int sub_path[], int curr_node) 
{
  int curr_node_idx = -1;
  for (int i = 0; i < MAX_NODES; i++) 
  {
    if (path[i] == curr_node) 
    {
      sub_path[i] = path[i];
      curr_node_idx = i;
    }

    if (curr_node_idx == -1) // means we have not reached curr node yet
      sub_path[i] = path[i];

    else if (curr_node_idx != -1 && i > curr_node_idx) // means we have reached curr node
      sub_path[i] = -1;
  }
}

void concat_path(int root_path[], int sub_path[], int concat_path[]) 
{
  int idx = 0;
  for (int i = 0; i < MAX_NODES; i++) 
  {
    if (root_path[i] != -1) 
    {
      concat_path[i] = root_path[i];
    } 
    else 
    {
      concat_path[i] = sub_path[++idx];
    }
  }
}

void copy_graph(int *graph[MAX_NODES], int *copy_graph[MAX_NODES]) 
{
  for (int i = 0; i < MAX_NODES; i++) 
  {
    for (int j = 0; j < MAX_NODES; j++) 
    {
      copy_graph[i][j] = graph[i][j];
    }
  }
}

int path_exists(int *path, int **K_paths, int K, PriorityQueue pq) 
{
  // Check in priority queue
  for (int i = 0; i < pq.size; i++) 
  {
    if (check_same_path(path, pq.pairs[i].path))
      return 1; 
  }

  // Check in K_paths matrix
  for (int i = 0; i < K; i++) 
  {
    if (check_same_path(path, K_paths[i]))
      return 1;
  }

  return 0; 
}

int main(int argc, char *argv[]) 
{
  srand(time(NULL));
  clock_t start = clock();

  int **graph = (int **)malloc(MAX_NODES * sizeof(int *));
  for (int i = 0; i < MAX_NODES; i++) 
  {
    graph[i] = (int *)malloc(MAX_NODES * sizeof(int));
    for (int j = 0; j < MAX_NODES; j++) 
    {
      graph[i][j] = 0;
    }
  }
  
  int** org_graph = (int **)malloc(MAX_NODES * sizeof(int *));
  for (int i = 0; i < MAX_NODES; i++) 
  {
    org_graph[i] = (int *)malloc(MAX_NODES * sizeof(int));
    for (int j = 0; j < MAX_NODES; j++) 
    {
        org_graph[i][j] = 0;
    }
  }

  FILE *file = fopen("mapped-new-who.txt", "r"); 
  if (file == NULL) 
  {
    fprintf(stderr, "Error opening file.\n");
    return 1; // Exit if file opening fails
  }

  // Skip the first two lines
  char line[100];
  void* ptr = fgets(line, sizeof(line), file); // Skip the first line
  ptr = fgets(line, sizeof(line), file); // Skip the second line

  // Read data from the file and populate the graph
  int from_node, to_node, weight;
  while (fscanf(file, "%d %d %d", &from_node, &to_node, &weight) == 3) 
  {
    graph[from_node][to_node] = weight;
  }
  copy_graph(graph, org_graph);
  fclose(file);

  copy_graph(graph, org_graph);
  
  int K = 20;

  int **K_paths = (int **)malloc(K * sizeof(int *));
  for (int i = 0; i < K; i++) 
  {
    K_paths[i] = (int *)malloc(MAX_NODES * sizeof(int));
  }
  
  int *K_costs = (int *)malloc(K * sizeof(int));

  int all_sources[RAND_PAIRS];
  int all_sinks[RAND_PAIRS];

  for(int rand_pair=0; rand_pair<RAND_PAIRS; rand_pair++)
  {
    int source, sink;

    PriorityQueue pq;
    initialize(&pq);
    
    source = rand() % MAX_NODES;
    sink = rand() % MAX_NODES;

    all_sources[rand_pair] = source;
    all_sinks[rand_pair] = sink;

    int path[MAX_NODES];
    int cost = 0;

    cost = dijkstra(source, sink, graph, path);
    K_costs[0] = cost;

    // add to K shortest path and cost
    copy_path(path, K_paths[0]);

    for (int k = 1; k < K; k++) // for k=0, first path above
    {
      int curr_path[MAX_NODES];
      copy_path(K_paths[k - 1], curr_path);
      int len = get_path_length(curr_path);

      for (int i = 0; i < len - 1; i++) 
      {
        int curr_node = curr_path[i];
        int src_to_curr_cost = dijkstra(source, curr_node, org_graph, path);

        int ri[MAX_NODES];
        extract_sub_path(curr_path, ri, curr_node);

        for (int j = 0; j <= k - 1; j++) 
        {
          int jth_path[MAX_NODES];
          copy_path(K_paths[j], jth_path);
          int rj[MAX_NODES];
          extract_sub_path(jth_path, rj, curr_node);

          if (check_same_path(ri, rj)) 
          {
            int next_node = jth_path[i + 1];
            remove_edge(curr_node, next_node, graph);
          }
        }

        int si[MAX_NODES];
        int Cost = dijkstra(curr_node, sink, graph, si);

        if (Cost != INF) 
        {
          PathCostPair pair;

          concat_path(ri, si, pair.path);
          pair.cost = Cost + src_to_curr_cost;

          if (!path_exists(pair.path, K_paths, K, pq))
            insert(&pq, pair);
        }
      }

      // restore graph
      copy_graph(org_graph, graph);

      // add min shortest path to K_paths
      PathCostPair minPair;
      if (pq.size > 0) 
      {
        minPair = extractMin(&pq);
      }

      // minPair.path and minPair.cost
      copy_path(minPair.path, K_paths[k]);
      K_costs[k] = minPair.cost;
    }
  }
 
  for(int rp=0; rp<RAND_PAIRS; rp++)
  {
    printf("\n-----------------------------------------------------------");
    printf("\nSource: %d, Sink: %d\n", all_sources[rp], all_sinks[rp]);
    printf("-----------------------------------------------------------\n");
    for (int i = 0; i < K; i++) 
    {
      printf("\nk = %d\n", i + 1);
      print_path(K_paths[i]);
      printf("COST: %d\n", K_costs[i]);
    }
  }
  
  clock_t end = clock();
  double cpu_time = (double)(end - start) / CLOCKS_PER_SEC;

  printf("\nExecution time for serial code: %f\n\n", cpu_time);

  // free memory
  for (int i = 0; i < MAX_NODES; i++) 
  {
    free(graph[i]);
  }
  free(graph);

  for (int i = 0; i < MAX_NODES; i++) 
  {
    free(org_graph[i]);
  }
  free(org_graph);

  for (int i = 0; i < K; i++) 
  {
    free(K_paths[i]);
  }
  free(K_paths);

  free(K_costs);

  return 0;
}