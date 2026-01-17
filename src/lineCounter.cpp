#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <numeric>
#include <string>
#include <unordered_set>
#include <vector>

class LineCounter
{
    std::ifstream _stream;
    std::size_t _line_count = 0;
    struct line : std::string
    {
        friend std::istream &operator>>(std::istream &is, line &l)
        {
            return std::getline(is, l), is;
        }
    };

public:
    LineCounter(const char *file_name) : _stream(file_name) {}
    LineCounter(LineCounter const &) = delete;
    LineCounter &operator=(LineCounter const &) = delete;
    std::size_t operator()()
    {
        if (!_stream.is_open())
            return 0;

        if (!_stream.good())
            return _line_count;

        return _line_count = std::distance(std::istream_iterator<line>(_stream), {});
    }
    ~LineCounter()
    {
        _stream.close();
    }
};

int main(int argc, const char *argv[])
{
    namespace fs = std::filesystem;
    std::cout << std::boolalpha;
    std::unordered_set<std::string> extensions = {".c", ".h", ".cpp", ".hpp"};
    std::string directory = argc == 1 ? "." : argv[1];
    fs::current_path(fs::canonical((fs::current_path() / fs::path(directory))));
    std::cout << "Current directory: " << fs::current_path() << '\n';
    bool filter = argc > 2;
    std::string filter_str = filter ? argv[2] : "";
    std::size_t line_count = 0, file_count = 0;

    for (const auto &entry : fs::recursive_directory_iterator("."))
    {
        const auto &path = entry.path();
        if (filter && path.parent_path().string().find(filter_str) == std::string::npos)
        {
            continue;
        }
        if (path.has_extension() && (extensions.find(path.extension()) != extensions.end()))
        {
            auto current_lines = LineCounter(path.c_str())();
            line_count += current_lines;
            file_count += 1;
            std::cout << "File " << std::setw(25) << path.filename() << " : ";
            std::cout << std::setw(4) << current_lines << " lines.\n";
        }
    }
    std::cout << "File count: " << file_count << '\n';
    std::cout << "Total lines in project: " << line_count << '\n';
}