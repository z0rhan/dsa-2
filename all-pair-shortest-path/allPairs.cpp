#include <cassert>
#include <iomanip>
#include <limits>
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <fstream>
#include <iostream>
#include <sstream>

constexpr int32_t INF = std::numeric_limits<int32_t>::max();

struct Node
{
    int32_t value;
    std::unordered_map<int32_t, int32_t> adj_list;
};

using Graph =  std::unordered_map<int32_t, Node>;

using Edge = std::shared_ptr<std::pair<int32_t, int32_t>>;

using DP_WeightTable = std::vector<std::vector<int32_t>>;
using DP_EdgeTable = std::vector<std::vector<Edge>>;

Graph read_graph(const std::string& filename)
{
    Graph graph {};

    std::fstream fd(filename);
    std::string line;
    while (std::getline(fd, line))
    {
        int32_t from, to, weight;
        std::istringstream iss(line);
         if(!(iss >> from >> to >> weight))
         {
             throw std::runtime_error("Invalid input format");
         }

         auto from_it = graph.find(from);
         if (from_it == graph.end())
         {
             Node from_node;
             from_node.value = from;
             from_node.adj_list.insert( {to, weight} );

             graph.insert( {from, from_node} );
         }
         else
         {
             from_it->second.adj_list.insert_or_assign(to, weight);
         }

         auto to_it = graph.find(to);
         if (to_it == graph.end())
         {
             Node to_node;
             to_node.value = to;

             graph.insert( {to, to_node} );
         }
    }

    return graph;
}

std::pair<DP_WeightTable, DP_EdgeTable>
construct_table(Graph& graph)
{
    size_t size = graph.size();
    DP_WeightTable weight_table(size, std::vector<int32_t>(size, INF));
    DP_EdgeTable edge_table(size, std::vector<Edge>(size, nullptr));

    for (size_t idx = 0; idx < size; ++idx)
        weight_table[idx][idx] = 0;

    for (const auto& [val, node] : graph)
    {
        auto adj_list = node.adj_list;
        for (const auto& [next_node, weight] : adj_list)
        {
            weight_table[val - 1][next_node - 1] = weight;

            Edge edge = std::make_shared<std::pair<int32_t, int32_t>>(val, next_node);
            edge_table[val - 1][next_node - 1] = edge;
        }
    }

    return { weight_table, edge_table };
}

void print_table(
    DP_WeightTable& weight_table,
    DP_EdgeTable& edge_table,
    int32_t iteration
)
{
    std::cout << "Iteration " << iteration << "\n";

    for (const auto& row : weight_table)
    {
        for (const auto& weight : row)
        {
            std::cout << std::setw(5);
            if (weight == INF)
                std::cout << "inf";
            else
                std::cout << weight;
        }
        std::cout << "\n";
    }
    std::cout << "\n";

    for (const auto& row : edge_table)
    {
        for (const auto& edge : row)
        {
            std::cout << std::setw(7);
            if (edge == nullptr)
                std::cout << "null";
            else
            {
                std::string edge_str = std::to_string(edge->first) + "-" + std::to_string(edge->second);
                std::cout << edge_str;
            }
        }
        std::cout << "\n";
    }
    std::cout << "\n";
}

[[nodiscard]] int32_t check_cycle(DP_WeightTable weight_table)
{
    int32_t start_node {};
    size_t size = weight_table.size();

    for (size_t idx = 0; idx < size; ++idx)
    {
        if (weight_table[idx][idx] < 0)
        {
            start_node = idx + 1;
            break;
        }
    }

    return start_node;
}

void print_cycle(DP_EdgeTable& edge_table, int32_t start_node)
{
    std::cout << "A negative cycle detected: " << start_node;
    Edge current_edge = edge_table[start_node - 1][start_node - 1];
    int32_t current_node = current_edge->second;
    while (current_node != start_node)
    {
        std::cout << " " << current_node;
        current_edge = edge_table[current_node - 1][start_node - 1];
        current_node = current_edge->second;
    }
    std::cout << " " << start_node << std::endl;

}

void procedure(DP_WeightTable& weight_table, DP_EdgeTable& edge_table)
{
    size_t size = weight_table.size();
    assert(size == edge_table.size());

    int32_t iteration {};
    print_table(weight_table, edge_table, iteration);

    for (size_t k = 0; k < size; ++k)
    {
        for (size_t i = 0; i < size; ++i)
        {
            for (size_t j = 0; j < size; ++j)
            {
                if (weight_table[i][k] == INF ||
                    weight_table[k][j] == INF)
                    continue;

                int32_t new_dist = weight_table[i][k] + weight_table[k][j];
                int32_t& prev_dist = weight_table[i][j];
                if (new_dist < prev_dist)
                {
                    prev_dist = new_dist;

                    auto& ptr = edge_table[i][j];
                    if (ptr == nullptr)
                    {
                        ptr = edge_table[i][k];
                    }
                    else
                    {
                        ptr.reset();
                        ptr = edge_table[i][k];
                    }
                }
            }
        }
        ++iteration;
        print_table(weight_table, edge_table, iteration);

        int32_t cycle_start = check_cycle(weight_table);
        if (cycle_start != 0)
        {
            print_cycle(edge_table, cycle_start);
            return;
        }
    }
}

int main(int argc, char** argv)
{
    if (argc != 2)
        std::cout << "Usage: " << argv[0] << " <filename>\n";

    Graph graph = read_graph(argv[1]);
    auto [weight_table, edge_table] = construct_table(graph);

    procedure(weight_table, edge_table);
    return 0;
}
