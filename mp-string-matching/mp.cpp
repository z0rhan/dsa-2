#include <fstream>
#include <iostream>
#include <vector>
#include <assert.h>
#include <stdint.h>

std::vector<int> mp_table(const std::string& pattern)
{
    size_t size = pattern.size();

    std::vector<int> table {};
    table.resize(size);

    int32_t i {}, j {};
    while (i < size)
    {
        while (i < size && pattern[i + 1] == pattern[j])
        {
            i++;
            j++;
            table[i] = j;
        }

        if (j > 0)
            j = table[j - 1];
        else
            i++;
    }

    return table;
}

void mp_match(
    const std::string& str_to_match,
    const std::string& pattern,
    std::vector<int> mp_table
)
{
    size_t str_size = str_to_match.size();
    size_t pattern_size = pattern.size();
    int32_t i{}, j{};

    while ((i - j) <= (str_size - pattern_size))
    {
        std::cout << "P at pos " << i - j << " with i = " << i << " and j = " << j << "\n";

        // Bookkeeping for tests
        std::string str_matched, pattern_matched;
        char current_str_char = str_to_match[i];
        char current_pattern_char = pattern[j];
        int32_t start_i = i;
        int32_t start_j = j;
        //

        while (j < pattern_size &&
                std::tolower(current_str_char) == std::tolower(current_pattern_char))
        {
            str_matched.push_back(current_str_char);
            pattern_matched.push_back(current_pattern_char);
            ++i;
            ++j;
            current_str_char = str_to_match[i];
            current_pattern_char = pattern[j];
        }
        assert(str_matched.empty() == pattern_matched.empty());

        if (!str_matched.empty())
        {
            std::cout << "  matched T[" << start_i << ".." << i - 1 << "] = "
                      << str_matched
                      << " = P[" << start_j << ".." << j - 1 << "] = "
                      << pattern_matched << "\n";
        }
        if (j == pattern_size)
        {
            std::cout << "  found an occurrence of P\n";
        }
        else
        {
            std::cout << "  mismatch T[" << i << "] = "
                      << current_str_char
                      << " != P[" << j << "] = "
                      << current_pattern_char << "\n";
        }

        if (j > 0)
        {
            int32_t prev_j = j;
            j = mp_table[j - 1];
            std::cout << "  updated j from " << prev_j << " to fm[" << prev_j - 1 << "] = " << j << "\n";
        }
        else
        {
            int32_t prev_i = i++;
            std::cout << "  incremented i from " << prev_i << " to " << i << "\n";
        }
    }
}

int main(int argc, char** argv)
{
    if (argc != 2)
        std::cout << "Usage: " << argv[0] << " <filenam>\n";

    std::fstream fd(argv[1]);
    std::string pattern;
    std::string str_to_match;

    std::getline(fd, pattern);
    std::getline(fd, str_to_match);
    std::cout << "P: " << pattern << "\n";

    std::vector<int> fmp = mp_table(pattern);
    std::cout << "Function fm:";
    for (const auto& num : fmp)
        std::cout << " " << num;
    std::cout << "\n";

    mp_match(str_to_match, pattern, fmp);

    return 0;
}
