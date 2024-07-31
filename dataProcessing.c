#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_ENTRIES 1000
#define MAX_LENGTH 50

typedef struct 
{
    char string[MAX_LENGTH];
    int id;
} map_entry;

int AddString(map_entry *map, int *size, char *string) 
{
    for (int i = 0; i < *size; i++) 
    {
        if (strcmp(map[i].string, string) == 0) 
        {
            return i; 
        }
    }

    if (*size >= MAX_ENTRIES) 
    {
        printf("Error: exceeded maximum entries.\n");
        exit(EXIT_FAILURE);
    }
    strcpy(map[*size].string, string);
    return (*size)++;
}

int main() 
{
    FILE *file = fopen("doctorwho.csv", "r");
    FILE *temp_file = fopen("temp-mapped-who.csv", "w"); 
    if (file == NULL || final_file == NULL) 
    {
        printf("Error opening file.\n");
        return 1;
    }

    char line[1024];
    map_entry map[MAX_ENTRIES];
    int mapSize = 0, edgeCount = 0;
    fgets(line, sizeof(line), file);

    while (fgets(line, sizeof(line), file)) 
    {
        char *source = strtok(line, ",");
        char *target = strtok(NULL, ",");
        AddString(map, &mapSize, source);
        AddString(map, &mapSize, target);
    }

    rewind(file);
    fgets(line, sizeof(line), file); 

    while (fgets(line, sizeof(line), file)) 
    {
        char *source = strtok(line, ",");
        char *target = strtok(NULL, ",");
        char *weightStr = strtok(NULL, ","); 

        int sourceId = AddString(map, &mapSize, source);
        int targetId = AddString(map, &mapSize, target);
        int weight = atoi(weightStr);
        fprintf(temp_file, "%d %d %d\n", sourceId, targetId, weight);
        edgeCount++;
    }

    fclose(file);
    fclose(temp_file);

    FILE *finalOutputFile = fopen("mapped-who.txt", "w"); 
    if (final_file == NULL) 
    {
        printf("Error opening final output file.\n");
        return 1;
    }

    fprintf(temp_file, "Total Edges: %d, Total Unique Nodes: %d\n", edgeCount, mapSize);
    fprintf(final_file, "SourceID,TargetID,Weight\n");

    temp_file = fopen("temp-mapped-who.csv", "r");

    while ((fgets(line, sizeof(line), temp_file)) != NULL) 
    {
        fputs(line, final_file);
    }

    fclose(temp_file);
    fclose(final_file);
    remove("temp-mapped-who.csv");
    
    return 0;
}
