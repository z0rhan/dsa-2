#include <functional>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <queue>

using PriorityQueue = std::priority_queue<std::tuple<int, int, int>,
                                          std::vector<std::tuple<int, int, int>>,
                                          std::greater<std::tuple<int, int, int>>>;

class Graph
{
public:
    Graph() = default;
    ~Graph() = default;

    void read_graph(std::string filename);
    void read_graph_with_to_weights(std::string filename);

    void add_edge(int from, int to);
    void add_edge_with_weight(int from, int to, int weight);

    bool exists(int from, int to);

    void print();

    size_t size() { return to_list.size(); }

    std::vector<std::vector<int>> to_list;
    std::vector<std::vector<int>> from_list;
    std::vector<std::vector<int>> to_weights;
    std::vector<std::vector<int>> from_weights;

    int total_cost = 0;
};


void Graph::read_graph(std::string filename)
{
    to_list.clear();
    std::ifstream file_handle(filename);
    std::string line_buf;
    while (std::getline(file_handle, line_buf))
    {
        std::stringstream ss(line_buf);
        int from{}, to{};
        ss >> from >> to;

        add_edge(from, to);
    }
}

void Graph::read_graph_with_to_weights(std::string filename)
{
    to_list.clear();
    to_weights.clear();
    std::ifstream file_handle(filename);
    std::string line_buf;
    while (std::getline(file_handle, line_buf))
    {
        std::stringstream ss(line_buf);
        int from{}, to{}, weight{};
        ss >> from >> to >> weight;

        add_edge_with_weight(from, to, weight);
    }
}

void Graph::add_edge(int from, int to)
{
    int max = std::max(from, to) + 1;
    if (max > (int)to_list.size())
    {
        to_list.resize(max);
        from_list.resize(max);
    }

    auto& adj_list = to_list[from];
    auto it = std::find(adj_list.begin(), adj_list.end(), to);
    if (it == adj_list.end())
    {
        adj_list.push_back(to);
    }

    auto& from_adj_list = from_list[to];
    it = std::find(from_adj_list.begin(), from_adj_list.end(), from);
    if (it == from_adj_list.end())
    {
        from_adj_list.push_back(from);
    }
}


void Graph::add_edge_with_weight(int from, int to, int weight)
{
    int max = std::max(from, to) + 1;
    if (max > (int)to_list.size())
    {
        to_list.resize(max);
        from_list.resize(max);
        to_weights.resize(max);
        from_weights.resize(max);
    }

    auto& adj_list = to_list[from];
    auto& weight_list = to_weights[from];
    auto it = std::find(adj_list.begin(), adj_list.end(), to);
    if (it == adj_list.end())
    {
        adj_list.push_back(to);
        weight_list.push_back(weight);

        total_cost += weight;
    }
    else
    {
        // Update the weight if edge already exists
        size_t index = it - adj_list.begin();
        total_cost = total_cost - weight_list[index] + weight;
        weight_list[index] = weight;
    }

    auto& from_adj_list = from_list[to];
    auto& from_weight_list = from_weights[to];
    it = std::find(from_adj_list.begin(), from_adj_list.end(), from);
    if (it == from_adj_list.end())
    {
        from_adj_list.push_back(from);
        from_weight_list.push_back(weight);
    }
    else
    {
        size_t index = it - from_adj_list.begin();
        from_weight_list[index] = weight;
    }
}

bool Graph::exists(int from, int to)
{
    auto& adj_list = to_list.at(from);
    auto it = std::find(adj_list.begin(), adj_list.end(), to);

    if (it == adj_list.end())
    {
        return false;
    }
    return true;
}

void Graph::print()
{
    for (size_t i = 0; i < to_list.size(); i++)
    {
        std::cout << i << " :";
        for (size_t j = 0; j < to_list[i].size(); j++)
        {
            std::cout << " (" << to_list[i][j] << ", " << to_weights[i][j] << ")";
        }
        std::cout << std::endl;
    }
}

void print_mst(Graph& graph)
{
    auto to_list = graph.to_list;
    std::cout << "MST(" << graph.total_cost << "):";
    for (size_t idx = 0; idx < to_list.size(); idx++)
    {
        auto& current_to_list = to_list[idx];
        std::sort(current_to_list.begin(), current_to_list.end());
        for (const auto& node : current_to_list)
        {
            std::cout << " " << idx << "-" << node;
        }
    }
    std::cout << std::endl;
}

Graph mst(Graph& graph, int& start_node)
{
    Graph result;
    PriorityQueue queue;
    std::vector<bool> visited;
    visited.resize(graph.size());
    visited[start_node] = true;

    // update priority_queue with both from start_node and to start_node
    auto current_list = graph.to_list[start_node];
    auto current_weight = graph.to_weights[start_node];
    for (size_t idx = 0; idx < current_list.size(); idx++)
    {
        queue.push({current_weight.at(idx),
                    start_node,
                    current_list.at(idx)});
    }
    current_list = graph.from_list[start_node];
    current_weight = graph.from_weights[start_node];
    for (size_t idx = 0; idx < current_list.size(); idx++)
    {
        queue.push({current_weight.at(idx),
                    current_list.at(idx),
                    start_node});
    }

    int iteration = 1;

    while (!queue.empty())
    {
        auto [w, u, v] = queue.top();
        queue.pop();

        int new_node = -1;
        if (!visited[u])
        {
            new_node = u;
        }
        else if (!visited[v])
        {
            new_node = v;
        }

        if (new_node != -1)
        {
            result.add_edge_with_weight(u, v, w);
            visited[new_node] = true;

            std::cout << "** Iteration " << iteration << " **\n";
            iteration++;
            std::cout << "Adding the edge ("
                      << u << ", "
                      << v << ", "
                      << w << ") with the new node "
                      << new_node 
                      << std::endl;

            current_list = graph.to_list[new_node];
            current_weight = graph.to_weights[new_node];
            for (size_t idx = 0; idx < current_list.size(); idx++)
            {
                if (!visited[current_list.at(idx)])
                {
                    queue.push({current_weight.at(idx),
                                new_node,
                                current_list.at(idx)});
                }
            }
            current_list = graph.from_list[new_node];
            current_weight = graph.from_weights[new_node];
            for (size_t idx = 0; idx < current_list.size(); idx++)
            {
                if (!visited[current_list.at(idx)])
                {
                    queue.push({current_weight.at(idx),
                                current_list.at(idx),
                                new_node});
                }
            }
        }
        else
        {
            std::cout << "** Iteration " << iteration << " **\n";
            iteration++;
        }

    }

    return result;
}

int main(int argc, char** argv)
{
    if (argc != 3)
    {
        std::cout << "Usage: " << argv[0] << " <input-file> <start-node>\n";
        return 1;
    }

    Graph graph;
    graph.read_graph_with_to_weights(argv[1]);

    int start_node = atoi(argv[2]);
    Graph mst_graph = mst(graph, start_node);
    print_mst(mst_graph);

    std::cout << std::endl;
    std::cout << "--> Above this is required by course platform for the test <--\n";
    std::cout << std::endl;

    std::cout << "This is what the graph looks like:\n";
    graph.print();
    std::cout << "This is what the mst of the graph looks like:\n";
    mst_graph.print();
    return 0;
}
