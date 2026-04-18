#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX(a, b) ((a) > (b) ? (a) : (b))

struct Node
{
    int value;
    struct Node* next;
};
typedef struct Node Node;

struct Graph
{
    Node** adjacency_list;
    Node** from_list;
    int* exists;
    size_t capacity;
    size_t size;
};
typedef struct Graph Graph;

void resize_graph(Graph* graph, size_t min_size)
{
    if (min_size < graph->capacity)
    {
        if (min_size > graph->size)
        {
            graph->size = min_size;
        }
        return;
    }

    size_t new_capacity;
    if (graph->capacity < 1)
    {
        new_capacity = 1;
    }
    else
    {
        new_capacity = graph->capacity;
    }

    // Double the capacity until it is large enough to hold min_size
    while (new_capacity < min_size)
    {
        new_capacity *= 2;
    }

    Node** new_adj_list = calloc(new_capacity, sizeof(Node*));
    Node** new_from_list = calloc(new_capacity, sizeof(Node*));
    int* new_exists_list = calloc(new_capacity, sizeof(int));
    if (!new_adj_list || !new_from_list || !new_exists_list)
    {
        free(new_from_list);
        free(new_adj_list);
        free(new_exists_list);
        printf("Failed to resize graph\n");
        return;
    }

    if (graph->adjacency_list != NULL)
    {
        memcpy(new_adj_list,
               graph->adjacency_list,
               graph->capacity * sizeof(Node*));
        free(graph->adjacency_list);
    }
    if (graph->from_list != NULL)
    {
        memcpy(new_from_list,
               graph->from_list,
               graph->capacity * sizeof(Node*));
        free(graph->from_list);
    }
    if (graph->exists != NULL)
    {
        memcpy(new_exists_list,
               graph->exists,
               // Only upto size, we have meaningful info
               graph->size * sizeof(int));
        free(graph->exists);
    }

    graph->adjacency_list = new_adj_list;
    graph->from_list = new_from_list;
    graph->exists = new_exists_list;
    graph->capacity = new_capacity;
    graph->size = min_size;
}

void update_list(Node* list, Node* node)
{
    while(list->next)
    {
        if (list->next->value > node->value)
        {
            // Insert before something bigger than node
            node->next = list->next;
            list->next = node;
            return;
        }
        list = list->next;
    }
    list->next = node;
}

int add_edge(Graph* graph, int u, int v)
{
    // TODO: handle when node already exists in either list
    Node* from_node = (Node*)malloc(sizeof(Node));
    Node* to_node = (Node*)malloc(sizeof(Node));
    if (!from_node || !to_node)
    {
        free(from_node);
        free(to_node);
        return 0;
    }

    to_node->value = v;
    to_node->next = NULL;
    from_node->value = u;
    from_node->next = NULL;


    Node* to_list = graph->adjacency_list[from_node->value];
    if (!to_list)
    {
        // to_list = to_node is wrong
        // to_list->next = to_node works since we are mutating the memory pointed by to_list
        // but to_list = to_node just means now to_list points to new memory address
        // Lesson learned
        graph->adjacency_list[from_node->value] = to_node;
    }
    else if (to_list->value > to_node->value)
    {
        // Insert at start if smallest
        // Makes it easier when priting the topological order
        // which the course platform tests wants in ascending order for deterministic result
        to_node->next = to_list;
        graph->adjacency_list[from_node->value] = to_node;
    }
    else
    {
        update_list(to_list, to_node);
    }


    Node* from_list = graph->from_list[to_node->value];
    if (!from_list)
    {
        graph->from_list[to_node->value] = from_node;
    }
    else if (from_list->value > from_node->value)
    {
        from_node->next = from_list;
        graph->from_list[to_node->value] = from_node;
    }
    else
    {
        update_list(from_list, from_node);
    }

    graph->exists[u] = 1;
    graph->exists[v] = 1;
    return 1;
}

int _remove_node(Node** list, int node)
{
    Node* current_node = *list;
    if (!current_node)
    {
        return 0;
    }

    if (current_node->value == node)
    {
        *list = current_node->next;
        free(current_node);
        return 1;
    }
    
    Node* prev_node = current_node;
    current_node = current_node->next;
    while (current_node && current_node->value != node)
    {
        prev_node = current_node;
        current_node = current_node->next;
    }

    if (!current_node)
        return 0;

    prev_node->next = current_node->next;
    
    free(current_node);
    return 1;
}

void remove_node(Graph* graph, int node)
{
    size_t size = graph->size;
    for (size_t idx = 0; idx < size; idx++)
    {
        _remove_node(&graph->adjacency_list[idx], node);
        _remove_node(&graph->from_list[idx], node);
    }
}

int read_graph_from_file(char* filename, Graph* graph)
{
    FILE* fd = fopen(filename, "r");
    if (!fd)
    {
        return 0;
    }
    char buffer[10];

    while (fgets(buffer, 10, fd))
    {
        char* startptr = buffer;
        char* endptr;

        int u = strtol(startptr, &endptr, 10);
        if (startptr == endptr)
        {
            return 0;
        }
        startptr = endptr;

        int v = strtol(startptr, &endptr, 10);
        if (startptr == endptr)
        {
            return 0;
        }

        resize_graph(graph, MAX(u, v) + 1);

        if (!add_edge(graph, u, v))
        {
            return 0;
        }
    }

    fclose(fd);
    return 1;
}

void free_graph(Graph* graph)
{
    for (size_t i = 0; i < graph->size; i++)
    {
        Node* to_list = graph->adjacency_list[i];
        Node* current = to_list;
        while (to_list)
        {
            to_list = to_list->next;
            free(current);
            current = to_list;
        }
    }

    for (size_t i = 0; i < graph->size; i++)
    {
        Node* from_list = graph->from_list[i];
        Node* current = from_list;
        while (from_list)
        {
            from_list = from_list->next;
            free(current);
            current = from_list;
        }
    }

    free(graph->adjacency_list);
    free(graph->from_list);
    free(graph->exists);

    graph->adjacency_list = NULL;
    graph->from_list = NULL;
    graph->exists = NULL;
}

size_t num_of_node(Graph* graph)
{
    int count = 0;
    for (size_t idx = 0; idx < graph->capacity; idx++)
    {
        if (graph->exists[idx])
            count++;
    }

    return count;
}

void procedure(Graph* graph, int* topological_order)
{
    size_t size = graph->size;

    int source_node_found = 0;
    do
    {
        for (size_t node = 0; node < size; node++)
        {
            source_node_found = 0;
            Node* current_list = graph->from_list[node];
            if (graph->exists[node] && !current_list)
            {
                remove_node(graph, node);

                *topological_order = node;
                topological_order++;

                source_node_found = 1;
                
                graph->exists[node] = 0;
                break;
            }
        }
    }
    while (source_node_found);
}

void print_topological_order(int* topological_order, size_t size)
{
    printf("Found a topological order:");

    for (size_t idx = 0; idx < size; idx++)
    {
        printf(" %d", topological_order[idx]);
    }
    printf("\n");
}

int exists(int* list, int target, size_t size)
{
    for (size_t idx = 0; idx < size; idx++)
    {
        if (list[idx] == target)
        {
            // since idx can be 0 too
            return idx + 1;
        }
    }
    return 0;
}

void print_cycle(Graph* graph, int start)
{
    printf("The graph is not acyclic, found a cycle:");

    int* path = (int*)malloc(graph->size * sizeof(int));
    if (!path)
    {
        printf("\n");
        return;
    }

    size_t path_size = 0;
    int current = start;

    while (1)
    {
        int cycle_start = exists(path, current, path_size);
        if (cycle_start)
        {
            printf(" %d", current);
            for (size_t idx = path_size; idx > (size_t)cycle_start; idx--)
            {
                printf(" %d", path[idx - 1]);
            }
            printf(" %d\n", current);
            free(path);
            return;
        }

        path[path_size] = current;
        path_size++;

        Node* previous = graph->from_list[current];
        if (!previous)
        {
            break;
        }

        current = previous->value;
    }

    printf("\n");
    free(path);
}

void print_result(Graph* graph, int* topological_order, size_t num_of_node)
{
    for (int idx = 0; idx < graph->size; idx++)
    {
        if (graph->from_list[idx])
        {
            print_cycle(graph, idx);
            return;
        }
    }
    print_topological_order(topological_order, num_of_node);
}

void print_graph(Graph* graph)
{
    printf("CHILDREN LIST\n");
    int count = 0;
    for (size_t i = 0; i < graph->size; i++)
    {
        Node* to_list = graph->adjacency_list[i];
        printf("%d :", count);
        while (to_list)
        {
            printf(" %d", to_list->value);
            to_list = to_list->next;
        }
        printf("\n");
        count++;
    }

    printf("PARENT LIST\n");
    count = 0;
    for (size_t i = 0; i < graph->size; i++)
    {
        Node* to_list = graph->from_list[i];
        printf("%d :", count);
        while (to_list)
        {
            printf(" %d", to_list->value);
            to_list = to_list->next;
        }
        printf("\n");
        count++;
    }
}

int main(int argc, char** argv)
{
    if (argc != 2)
    {
        printf("Usage: %s <file-name>\n", argv[0]);
        return 1;
    }

    Graph graph = {
        .adjacency_list= NULL,
        .from_list = NULL,
        .exists = NULL,
        .capacity = 0,
        .size = 0
    };
    int sucess = read_graph_from_file(argv[1], &graph);
    if (!sucess)
    {
        printf("Could not read graph from file: %s\n", argv[1]);
        free_graph(&graph);
        return 1;
    }

    size_t nodes = num_of_node(&graph);
    int* topological_order = (int*)calloc(nodes, sizeof(int));
    if (!topological_order)
    {
        free(topological_order);
        return 1;
    }

    procedure(&graph, topological_order);

    print_result(&graph, topological_order, nodes);

    free_graph(&graph);
    free(topological_order);

    return 0;
}
