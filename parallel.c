#include "queue.h"
#include <limits.h>
#include <mpi.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#define INF 100000
#define MAX_NODES 336
#define RAND_PAIRS 10

void remove_edge(int i, int j, int **graph) 
{
  graph[i][j] = INF;
  graph[j][i] = INF;
}

void print_path(int path[], int rank) 
{
  printf("|| Path from rank %d: ||\n", rank);
  for (int i = 0; i < MAX_NODES; i++) 
  {
    if (path[i] != -1) 
    {
      printf("%d", path[i]);

      if (i == MAX_NODES - 1 || path[i + 1] == -1)
        printf("\n");
      else
        printf(" -> ");
    } 
    else
      break;
  }
}

void copy_path(int path[], int path_copy[]) 
{
  for (int i = 0; i < MAX_NODES; i++) 
  {
    path_copy[i] = path[i];
  }
}

void save_shortest_path(int source, int sink, int predecessors[], int path[]) 
{
  int index = 0;
  for (int v = sink; v != source; v = predecessors[v]) {
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

int get_path_length(int path[]) {
  int len = 0;
  for (int i = 0; i < MAX_NODES; i++) {
    if (path[i] != -1)
      len++;
  }
  return len;
}

int get_min_vertex(int distances[], int visited[]) 
{
  int min = INF, min_index = -1;

  for (int vertex = 0; vertex < MAX_NODES; vertex++) {
    if (visited[vertex] == 0 && distances[vertex] <= min) {
      min = distances[vertex];
      min_index = vertex;
    }
  }
  return min_index;
}

int dijkstra(int source, int sink, int** graph, int shortest_path[]) 
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
      if (visited[adj_v] != 1 && graph[curr_min_vertex][adj_v] &&
        distance[curr_min_vertex] != INF &&
        distance[curr_min_vertex] + graph[curr_min_vertex][adj_v] < distance[adj_v])
        {
          distance[adj_v] = distance[curr_min_vertex] + graph[curr_min_vertex][adj_v];
          predecessors[adj_v] = curr_min_vertex;
        }
    }     
  }

  if(distance[sink] != INF)
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
    if (root_path[i] != -1) {
      concat_path[i] = root_path[i];
    } 
    else 
    {
      concat_path[i] = sub_path[++idx];
    }
  }
}

void copy_graph(int **graph, int **copy_graph) 
{
  for (int i = 0; i < MAX_NODES; i++) 
  {
    for (int j = 0; j < MAX_NODES; j++) 
    {
      copy_graph[i][j] = graph[i][j];
    }
  }
}

int path_exists(int path[], int** K_paths, int K, PriorityQueue pq) 
{
  // Check in priority queue
  for (int i = 0; i < pq.size; i++) 
  {
    if (check_same_path(path, pq.pairs[i].path))
      return 1; // Path exists, discard it
  }

  // Check in K_paths matrix
  for (int i = 0; i < K; i++) 
  {
    if (check_same_path(path, K_paths[i]))
      return 1; // Path exists, discard it
  }

  return 0; // Path does not exist
}

int main(int argc, char *argv[]) 
{
  srand(time(NULL));
  int rank, num_processes;

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &num_processes);

  int **graph = (int **)malloc(MAX_NODES * sizeof(int *));
  for (int i = 0; i < MAX_NODES; i++) 
  {
    graph[i] = (int *)malloc(MAX_NODES * sizeof(int));
    for (int j = 0; j < MAX_NODES; j++) 
    {
      graph[i][j] = 0;
    }
  }

  int** org_graph;

  if(rank==0)
  {
    org_graph = (int **)malloc(MAX_NODES * sizeof(int *));
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
  }

  for(int i=0; i<MAX_NODES; i++)
  {
    MPI_Bcast(graph[i], MAX_NODES, MPI_INT, 0, MPI_COMM_WORLD);
  }

  // Initializations
  int K = 10;

  int **K_paths = (int **)malloc(K * sizeof(int *));
  for (int i = 0; i < K; i++) 
  {
    K_paths[i] = (int *)malloc(MAX_NODES * sizeof(int));
  }
  
  int *K_costs = (int *)malloc(K * sizeof(int));

  PriorityQueue all_pqs[num_processes];
  #pragma omp parallel for
  for (int i = 0; i < num_processes; i++) 
  {
    initialize(&all_pqs[i]);
  }

  int work_done = 0;
  int work_counter = 0;

  int all_sources[RAND_PAIRS];
  int all_sinks[RAND_PAIRS];

  clock_t start, end;
  double cpu_time, total_cpu_time=0;

  start = clock();

  for(int rand_pair=0; rand_pair<RAND_PAIRS; rand_pair++)
  {
    int source, sink;
    
    if(rank==0)
    {
      source = rand() % MAX_NODES;
      sink = rand() % MAX_NODES;

      all_sources[rand_pair] = source;
      all_sinks[rand_pair] = sink;
    }

    MPI_Bcast(&source, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&sink, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // initialize the local Priority Queue to store paths within each process
    PriorityQueue local_pq;
    initialize(&local_pq);

    // initialize the main Priority Queue to store paths from processes
    PriorityQueue pq;
    initialize(&pq);

    int path[MAX_NODES];
    int cost = 0;

    //reset for next random pair
    work_done = 0;
    work_counter = 0;

    if (rank == 0) // calculate first shortest path
    {
      cost = dijkstra(source, sink, graph, path);
      K_costs[0] = cost;

      // add to K shortest path and cost
      copy_path(path, K_paths[0]);
    }
    
    for (int k = 1; k < K; k++) 
    {
      int len;
      int curr_path[MAX_NODES];
      if (rank == 0) 
      {
        copy_path(K_paths[k - 1], curr_path);
        len = get_path_length(curr_path);
        omp_set_num_threads(len - 1);
      }

      MPI_Bcast(&len, 1, MPI_INT, 0, MPI_COMM_WORLD);

      for (int i = 0; i < len - 1; i++) 
      {
        // declare variables for all processes
        int ri[MAX_NODES];
        int curr_node, src_to_curr_cost;

        // initialize variables by master only
        if (rank == 0) 
        {
          curr_node = curr_path[i];
          src_to_curr_cost = dijkstra(source, curr_node, org_graph, path);
          extract_sub_path(curr_path, ri, curr_node);
        }

        // broadcast neccessary data to processes 
        MPI_Bcast(&curr_node, 1, MPI_INT, 0, MPI_COMM_WORLD);
        MPI_Bcast(&src_to_curr_cost, 1, MPI_INT, 0, MPI_COMM_WORLD);
        MPI_Bcast(&ri, MAX_NODES, MPI_INT, 0, MPI_COMM_WORLD);

        if (rank == 0) 
        {
          #pragma omp parallel for
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
        }

        int process;
        if (rank == 0) 
        {
          process = (work_counter % (num_processes - 1)) + 1;
          work_counter++;
        }

        MPI_Bcast(&process, 1, MPI_INT, 0, MPI_COMM_WORLD);
        for(int i=0; i<MAX_NODES; i++)
        {
          MPI_Bcast(graph[i], MAX_NODES, MPI_INT, 0, MPI_COMM_WORLD);
        }

        int si[MAX_NODES];
        PathCostPair pair;
        int Cost;

        if (rank == process) 
        {
          Cost = dijkstra(curr_node, sink, graph, si);
          work_done++;

          if (Cost != INF) 
          {
            concat_path(ri, si, pair.path);
            pair.cost = Cost + src_to_curr_cost;

            if (!path_exists(pair.path, K_paths, K, pq))
              insert(&local_pq, pair);
          }
        }
      }

      MPI_Gather(&local_pq, sizeof(PriorityQueue), MPI_BYTE, all_pqs, sizeof(PriorityQueue), MPI_BYTE, 0, MPI_COMM_WORLD);

      if (rank == 0) 
      {
        omp_set_num_threads(num_processes);
        // gather all min pqs for current iteration
        #pragma omp parallel for 
        for (int pair = 0; pair < num_processes; pair++) 
        {
          while (all_pqs[pair].size > 0) 
          {
            PathCostPair temp_pair = extractMin(&all_pqs[pair]);
            if (temp_pair.cost != INF && !path_exists(temp_pair.path, K_paths, K, pq)) 
            {
              insert(&pq, temp_pair);
            }
          }
        }
      }

      if (rank == 0) 
      {
         copy_graph(org_graph, graph);
        // add min shortest path to K_paths
        PathCostPair minPair;
        if (pq.size > 0) 
        {
          minPair = extractMin(&pq);
        }

        // copy minPair.path and minPair.cost
        copy_path(minPair.path, K_paths[k]);
        K_costs[k] = minPair.cost;
      }
    }
  }
  
  end = clock();
  cpu_time = ((double) (end - start)) / CLOCKS_PER_SEC;
  
  MPI_Reduce(&cpu_time, &total_cpu_time, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

  MPI_Barrier(MPI_COMM_WORLD); // for printing
  if (rank == 0) 
  {
    for(int rp=0; rp<RAND_PAIRS; rp++)
    {
      printf("\n-----------------------------------------------------------");
      printf("\nSource: %d, Sink: %d\n", all_sources[rp], all_sinks[rp]);
      printf("-----------------------------------------------------------\n");
      for (int i = 0; i < K; i++) 
      {
        printf("\nk = %d\n", i + 1);
        print_path(K_paths[i], 0);
        printf("COST: %d\n", K_costs[i]);
      }
    }
    printf("\nExecution time for parallel code: %f\n\n", (total_cpu_time/num_processes)/K);
  }

  MPI_Barrier(MPI_COMM_WORLD); // for printing
  if (rank != 0)
    printf("RANK : %d  || WORK DONE: %d\n", rank, work_done);

  // free memory
  for (int i = 0; i < MAX_NODES; i++) 
  {
    free(graph[i]);
  }
  free(graph);
  
  if(rank==0)
  {
    for (int i = 0; i < MAX_NODES; i++) 
    {
      free(org_graph[i]);
    }
    free(org_graph);
  }

  for (int i = 0; i < K; i++) 
  {
    free(K_paths[i]);
  }
  free(K_paths);

  free(K_costs);
  
  MPI_Finalize();
  return 0;
}