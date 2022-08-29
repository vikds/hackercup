#include <fstream>
#include <functional>
#include <iostream>
#include <limits>
#include <map>
#include <queue>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>

#include <getopt.h>
#include <sysexits.h>
#include <unistd.h>

void Usage(const char* binary_name) {
    std::cerr << "Usage: " << binary_name << " [-io] [-i input][-o output]" << std::endl;
    std::cerr << "\t-i|--input (required): input file" << std::endl;
    std::cerr << "\t-o|--output (required): output file" << std::endl;
    std::cerr << "\t-h|--help: show this usage" << std::endl;
    std::exit(EX_USAGE);
}

struct Arguments {
    std::string input_file;
    std::string output_file;
};

Arguments ParseArguments(int argc, char* argv[]) {
    Arguments arguments;
    static struct option long_options[] = {
        { "input", required_argument, nullptr, 'i' },
        { "output", required_argument, nullptr, 'o' },
        { "help", no_argument, nullptr, 'h' },
        { nullptr, 0, nullptr, 0 }
    };
    int ch;
    const char* binary_name = argv[0];
    while ((ch = getopt_long(argc, argv, "i:o:h", long_options, nullptr )) != -1) {
        switch (ch) {
            case 'i':
                arguments.input_file = optarg;
                break;
            case 'o':
                arguments.output_file = optarg;
                break;
            case 'h':
                Usage(binary_name);
        }
    }
    bool is_valid = true;
    std::ostringstream oss;
    if (arguments.input_file.empty()) {
        oss << "\t-i|--input option is required" << std::endl;
        is_valid = false;
    }
    if (arguments.output_file.empty()) {
        oss << "\t-o|--output option is required" << std::endl;
        is_valid = false;
    }
    if (!is_valid) {
        std::cerr << "Invalid options:" << std::endl;
        std::cerr << oss.str();
        Usage(binary_name);
    }
    return arguments;
}

bool file_exist(const std::string& file_name) {
    return access(file_name.c_str(), F_OK) != -1;
}

bool delete_file(const std::string& file_name) {
    return unlink(file_name.c_str()) != -1;
}

static const std::string NO_ANSWER = "NO";
static const std::string YES_ANSWER = "YES";

std::string ResultA(const std::vector<int>& parts, int K_capacity) {
    if (parts.size() > 2 * K_capacity) {
        return NO_ANSWER;
    }
    std::unordered_map<int, int> part_count;
    for (const int& part: parts) {
        int count = ++part_count[part];
        if (count > 2) {
            return NO_ANSWER;
        }
    }
    return YES_ANSWER;
}

void SolveA(const Arguments& args) {
    std::ifstream input(args.input_file);
    std::ofstream output(args.output_file);

    int tasks;
    input >> tasks;
    int part;
    std::vector<int> parts;
    for (int t = 1; t <= tasks; t++) {
        parts.clear();
        int N_parts, K_capacity;
        input >> N_parts >> K_capacity;
        for (int p = 0; p < N_parts; p++) {
            input >> part;
            parts.push_back(part);
        }

        output << "Case #" << t << ": " << ResultA(parts, K_capacity) << std::endl;
    }
}

using Grid = std::vector<std::string>;

struct Cell {
    int r;
    int c;

    bool operator==(const Cell& rhs) const {
        return r == rhs.r && c == rhs.c;
    }

    bool operator!=(const Cell& rhs) const {
        return r != rhs.r || c != rhs.c;
    }
};

struct CellHash {
    size_t operator()(const Cell& cell) const {
        return cell.r * 17 + cell.c * 31;
    }
};

using Cells = std::unordered_set<Cell, CellHash>;

std::array<std::pair<int, int>, 4> DIRS = {{
    {-1,  0},
    { 1,  0},
    { 0, -1},
    { 0,  1}
}};

static const std::string IMPOSSIBLE_ANSWER = "Impossible";
static const std::string POSSIBLE_ANSWER = "Possible";

int CountCellFriends(const Grid& grid, int row, int col) {
    int rows = grid.size();
    int cols = grid.front().size();

    int friends = 0;
    for (const auto& dir: DIRS) {
        int r = row + dir.first;
        int c = col + dir.second;
        if (r < 0 || r >= rows || c < 0 || c >= cols) {
            continue;
        }
        Cell next{r, c};
        if (grid[next.r][next.c] == '#' || grid[next.r][next.c] == '!') {
            continue;
        }
        friends++;
    }
    return friends;
}

Cells GetLonely(Grid& grid) {
    int rows = grid.size();
    int cols = grid.front().size();

    Cells lonely;
    for (int row = 0; row < rows; row++) {
        for (int col = 0; col < cols; col++) {
            if (grid[row][col] == '#') {
                continue;
            }
            int friends = CountCellFriends(grid, row, col);
            if (friends < 2) {
                lonely.insert({row, col});
                grid[row][col] = '!';
            }
        }
    }
    return lonely;
}

bool ResultB(const Cells& trees, Grid& grid) {
    if (trees.empty()) {
        return true;
    }

    int rows = grid.size();
    int cols = grid.front().size();
    Cells lonely = GetLonely(grid);
    std::queue<Cell> queue;
    for (const Cell& cell: lonely) {
        if (trees.find(cell) != trees.end()) {
            return false;
        }
        queue.push(cell);
    }
    while (!queue.empty()) {
        Cell cell = queue.front();
        queue.pop();
        for (const auto& dir: DIRS) {
            int r = cell.r + dir.first;
            int c = cell.c + dir.second;
            if (r < 0 || r >= rows || c < 0 || c >= cols) {
                continue;
            }
            Cell next{r, c};
            if (grid[next.r][next.c] == '#' || grid[next.r][next.c] == '!') {
                continue;
            }
            int friends = CountCellFriends(grid, next.r, next.c);
            if (friends < 2) {
                queue.push(next);
                grid[next.r][next.c] = '!';
                if (trees.find(next) != trees.end()) {
                    return false;
                }
            }
        }
    }

    for (int row = 0; row < rows; row++) {
        for (int col = 0; col < cols; col++) {
            if (grid[row][col] == '.') {
                grid[row][col] = '^';
            }
            if (grid[row][col] == '!') {
                grid[row][col] = '.';
            }
        }
    }
    return true;
}

void SolveB(const Arguments& args) {
    std::ifstream input(args.input_file);
    std::ofstream output(args.output_file);

    int tasks;
    input >> tasks;
    for (int t = 1; t <= tasks; t++) {
        int rows, cols;
        input >> rows >> cols;
        Grid grid;
        Cells trees;
        for (int r = 0; r < rows; r++) {
            std::string row;
            input >> row;
            grid.push_back(row);
            for (int c = 0; c < cols; c++) {
                if (row[c] == '^') {
                    trees.insert({r, c});
                }
            }
        }

        bool possible = ResultB(trees, grid);
        output << "Case #" << t << ": ";
        if (possible) {
            output << POSSIBLE_ANSWER << std::endl;
            for (const std::string& row: grid) {
                output << row << std::endl;
            }
        } else {
            output << IMPOSSIBLE_ANSWER << std::endl;
        }
    }
}

bool CodeIsPrefixOfAnother(const std::string& left, const std::string& right) {
    int min = std::min(left.size(), right.size());
    for (int i = 0; i < min; i++) {
        if (left[i] != right[i]) {
            return false;
        }
    }
    return true;
}

void Backtracking(const std::string& codeword, int N_codewords, std::string& code, int i, std::unordered_set<std::string>& result) {
    if (result.size() == N_codewords) {
        return;
    }
    if (i == code.size()) {
        if (!CodeIsPrefixOfAnother(codeword, code)) {
            result.insert(code);
        }
        return;
    }

    code[i] = '.';
    Backtracking(codeword, N_codewords, code, i + 1, result);

    code[i] = '-';
    Backtracking(codeword, N_codewords, code, i + 1, result);
}

std::unordered_set<std::string> ResultC(const std::string& codeword, int N_codewords) {
    int pow2 = 2;
    int codelength = 1;
    while (pow2 <= N_codewords) {
        codelength++;
        pow2 <<= 1;
    }

    std::string code(codelength + 1, '.');
    std::unordered_set<std::string> result;
    Backtracking(codeword, N_codewords - 1, code, 0, result);
    return result;
}

void SolveC(const Arguments& args) {
    std::ifstream input(args.input_file);
    std::ofstream output(args.output_file);

    int tasks;
    input >> tasks;
    int N_codewords;
    std::string codeword;
    for (int t = 1; t <= tasks; t++) {
        codeword.clear();
        input >> N_codewords;
        input >> codeword;

        std::unordered_set<std::string> result = ResultC(codeword, N_codewords);
        output << "Case #" << t << ": " << std::endl;
        for (const std::string& code: result) {
            output << code << std::endl;
        }
    }
}

using Flights = std::unordered_map<int, int64_t>;
using AirportFlights = std::unordered_map<int, Flights>;

int64_t ResultD(const AirportFlights& airport_flights, int src, int dst) {
    int64_t result = 0;
    auto flights_it = airport_flights.find(src);
    if (flights_it == airport_flights.end()) {
        return result;
    }
    const Flights& flights = flights_it->second;
    for (const auto& [hop, hop_capacity]: flights) {
        if (hop == dst) {
            result += 2 * hop_capacity;
            continue;
        }
        auto hop_it = airport_flights.find(hop);
        if (hop_it == airport_flights.end()) {
            continue;
        }
        const Flights& hop_flights = hop_it->second;
        auto hop_flight_it = hop_flights.find(dst);
        if (hop_flight_it == hop_flights.end()) {
            continue;
        }
        int64_t second_capacity = hop_flight_it->second;
        result += std::min(hop_capacity, second_capacity);
    }
    return result;
}

// Correct, but very inefficient:
void SolveD(const Arguments& args) {
    std::ifstream input(args.input_file);
    std::ofstream output(args.output_file);

    int N_airports;
    int M_flights;
    int Q_days;
    int airport1;
    int airport2;
    int64_t capacity;
    int src;
    int dst;

    int tasks;
    input >> tasks;
    AirportFlights airport_flights;
    for (int t = 1; t <= tasks; t++) {
        input >> N_airports;
        input >> M_flights;
        input >> Q_days;
        airport_flights.clear();
        for (int f = 0; f < M_flights; f++) {
            input >> src;
            input >> dst;
            input >> capacity;
            airport_flights[src][dst] = capacity;
            airport_flights[dst][src] = capacity;
        }
        output << "Case #" << t << ":";
        for (int q = 0; q < Q_days; q++) {
            input >> src;
            input >> dst;
            output << " " << ResultD(airport_flights, src, dst);
        }
        output << std::endl;
    }
}

int main(int argc, char* argv[]) {
    try {
        Arguments args = ParseArguments(argc, argv);

        if (!file_exist(args.input_file)) {
            std::cerr << "Input file doesn't exist: " << args.input_file << std::endl;
            return EXIT_FAILURE;
        }
        if (file_exist(args.output_file)) {
            delete_file(args.output_file);
        }

        SolveD(args);

    } catch(const std::exception& ex) {
        std::cerr << "Exception: " << ex.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}