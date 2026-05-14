#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <stdint.h>

using List = std::vector<std::string>;

void procedure(
    List& list,
    int32_t left,
    int32_t right,
    int32_t depth
)
{
    if (left < right)
    {
        size_t pivot_idx = (left + right) / 2;
        std::string pivot = list.at(pivot_idx);

        std::cout << "Sorting subarray "
                  << left
                  << "..."
                  << right
                  << " with depth "
                  << depth
                  << " and pivot "
                  << pivot
                  << "\n";

        std::swap(list[left], list[pivot_idx]);

        size_t l = left + 1;
        size_t pivot1 = left;
        size_t pivot2 = right;
        
        while (l <= pivot2)
        {
            std::string current = list[l];
            if (current[depth] < pivot[depth])
            {
                std::swap(list[l], list[pivot1]);
                pivot1++;
                l++;
            }
            else if (current[depth] > pivot[depth])
            {
                std::swap(list[l], list[pivot2]);
                pivot2--;
            }
            else
            {
                l++;
            }
        }

        procedure(list, left, pivot1 - 1, depth);
        
        if (pivot.length() > depth)
        {
            procedure(list, pivot1, pivot2, depth + 1);
        }

        procedure(list, pivot2 + 1, right, depth);
    }

    if (left >= right)
        std::cout << "Immediate return from subarray "
                  << left
                  << "..."
                  << right
                  << " with depth "
                  << depth
                  << "\n";
}

List read_file(const std::string& filename)
{
    List result;

    std::fstream fd(filename);
    std::string line;

    while (std::getline(fd, line))
    {
        result.push_back(line);
    }

    return result;
}

void print_list(List& list, std::string_view prefix)
{
    std::cout << prefix << ":";
    for (const auto& item : list)
    {
        std::cout << " " << item;
    }

    std::cout << std::endl;
}

int main(int argc, char** argv)
{
    if (argc != 2)
        std::cout << "Usage: "
                  << argv[0]
                  << " <filename>\n";

    List list = read_file(argv[1]);

    print_list(list, "Original");
    procedure(list, 0, list.size() - 1, 0);
    print_list(list, "Sorted");

    return 0;
}
