#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <utility>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>

constexpr std::string_view INVALID_PARENT = " ";

struct Node
{
    std::string value;
    int first_visit {};
    int lowest_visit {};
    std::vector<std::string> adjacency_list;
    std::vector<std::string> parent_list;
};

using Graph = std::unordered_map<std::string, Node>;
Graph graph;

void read_graph(const std::string& filename)
{
    std::ifstream fd(filename);
    std::string line;
    while (std::getline(fd, line))
    {
        std::string from, to;
        std::stringstream ss(line);
        if (!(ss >> from >> to))
        {
            throw std::runtime_error("Error in graph: " + line);
        }

        auto it = graph.find(to);
        if (it != graph.end())
        {
            it->second.parent_list.push_back(from);
        }
        Node to_node;
        to_node.value = to;
        to_node.parent_list = { from };
        graph.insert({ to, to_node });

        it = graph.find(from);
        if (it != graph.end())
        {

            it->second.adjacency_list.push_back(to);
            continue;
        }
        Node from_node;
        from_node.value = from;
        from_node.adjacency_list = { to };
        graph.insert({ from, from_node });
    }
}

void print_graph(Graph& graph)
{
    for (const auto& [value, node] : graph)
    {
        std::cout << value << " :";
        for (const auto& out : node.adjacency_list)
        {
            std::cout << " " << out;
        }
        std::cout << "\n";
    }
}

int procedure(Node& current,
              const std::string_view& parent,
              std::unordered_set<std::string>& visited,
              int& current_step,
              std::vector<std::string>& ap,
              std::vector<std::pair<std::string, std::string>>& bridges)
{
    int children {};
    current.first_visit = current_step;
    current.lowest_visit = current_step;
    visited.insert(current.value);
    current_step++;

    std::vector<std::string> outgoing(current.adjacency_list);
    outgoing.insert(outgoing.end(),
                    current.parent_list.begin(),
                    current.parent_list.end());
    for (const auto& next_node : outgoing)
    {
        auto it = graph.find(next_node);
        if (!visited.count(next_node))
        {
            children++;
            procedure(it->second, current.value, visited, current_step, ap, bridges);

            if (parent != INVALID_PARENT)
            {
                if (current.first_visit <= it->second.lowest_visit)
                {
                    if (std::find(ap.begin(), ap.end(), current.value) == ap.end())
                    {
                        ap.push_back(current.value);
                    }
                }
            }

            if (current.first_visit < it->second.lowest_visit)
            {
                std::pair<std::string, std::string> edge;
                if (current.value > next_node)
                {
                    edge = std::make_pair(next_node, current.value);
                }
                else
                {
                    edge = std::make_pair(current.value, next_node);
                }

                if (std::find(bridges.begin(),
                              bridges.end(),
                              edge) == bridges.end())
                {
                    bridges.emplace_back(std::move(edge));
                }
            }

            current.lowest_visit = std::min(it->second.lowest_visit,
                                            current.lowest_visit);
        }
        else
        {
            if (next_node == parent)
                continue;
            current.lowest_visit = std::min(it->second.first_visit,
                                            current.lowest_visit);
        }
    }

    return children;
}

void print_articulation_nodes(std::vector<std::string>& ap)
{
    if (ap.empty())
        return;
    std::cout << "Found "
              << ap.size()
              << " articulation nodes:";

    for (const auto& node : ap)
    {
        std::cout << " " << node;
    }
    std::cout << std::endl;
}

void print_bridges(std::vector<std::pair<std::string, std::string>>& bridges)
{
    if (bridges.empty())
        return;
    std::cout << "Found "
              << bridges.size()
              << " bridges:";

    for (const auto& edge : bridges)
    {
        std::cout << " " << edge.first << "-" << edge.second;
    }
    std::cout << std::endl;
}

int main(int argc, char** argv)
{
    if (argc != 2)
    {
        std::cout << "Usage: " << argv[0] << " <filename>\n";
        return 1;
    }

    read_graph(argv[1]);

    std::unordered_set<std::string> visited;
    visited.reserve(graph.size());
    int visit = 0;

    auto it = graph.begin();
    std::vector<std::string> ap;
    std::vector<std::pair<std::string, std::string>> bridges;
    int result = procedure(it->second, INVALID_PARENT, visited, visit, ap, bridges);
    if (result > 1)
    {
        if (std::find(ap.begin(), ap.end(), it->second.value) == ap.end())
            ap.push_back(it->second.value);
    }

    print_articulation_nodes(ap);
    print_bridges(bridges);

    return 0;
}
