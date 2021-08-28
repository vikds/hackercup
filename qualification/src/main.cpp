#include <fstream>
#include <functional>
#include <iostream>
#include <limits>
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

struct VoweltyStats {
    std::unordered_map<char, int> char_count;
    char max_char = ' ';
    int total_count = 0;
    int max_count = 0;

    void CountChar(char ch) {
        int count = ++char_count[ch];
        if (count > max_count) {
            max_count = count;
            max_char = ch;
        }
        total_count++;
    }

    int MinorCount() const {
        return total_count - max_count;
    }
};

int ResultA1(const std::string& str) {
    static const std::unordered_set<char> VOWELS = {'A', 'E', 'I', 'O', 'U'};
    std::unordered_map<char, int> char_count;
    VoweltyStats consonants;
    VoweltyStats vowels;
    for (const char& ch: str) {
        char_count[ch]++;
        if (VOWELS.find(ch) != VOWELS.end()) {
            vowels.CountChar(ch);
        } else {
            consonants.CountChar(ch);
        }
    }

    if (char_count.size() <= 1) {
        return 0;
    }
    if (vowels.char_count.size() == 0 || consonants.char_count.size() == 0) {
        return str.size();
    }
    size_t map_to_vovwel = vowels.MinorCount() * 2 + consonants.total_count;
    size_t map_to_consonant = consonants.MinorCount() * 2 + vowels.total_count;
    return std::min(map_to_vovwel, map_to_consonant);
}

void SolveA1(const Arguments& args) {
    std::ifstream input(args.input_file);
    std::ofstream output(args.output_file);

    int tasks = 0;
    input >> tasks;
    for (int t = 1; t <= tasks; t++) {
        std::string str;
        input >> str;
        output << "Case #" << t << ": " << ResultA1(str) << std::endl;
    }
}

using Matrix = std::vector<std::vector<int>>;
static const int ALPHA_NUM = 26;

struct Node {
    Node()
      : ch_count(ALPHA_NUM, 0)
    {}

    std::vector<int> ch_count;
    int time = 0;
    
    bool operator==(const Node& rhs) const {
        return ch_count == rhs.ch_count;
    }

    bool operator!=(const Node& rhs) const {
        return ch_count != rhs.ch_count;
    }

    bool IsConsistent() const {
        int count = 0;
        for (const int& val : ch_count) {
            if (val > 0) {
                count++;
            }
        }
        return count <= 1;
    }

    void Print() const {
        for (const int& val : ch_count) {
            std::cout << val;
        }
        std::cout << std::endl;
    }
};

struct NodeHash {
    size_t operator()(const Node& node) const {
        size_t result = 0;
        for (int i = 0; i < node.ch_count.size(); i++) {
            int count = node.ch_count[i];
            if (count == 0) {
                continue;
            }
            result ^= std::hash<int>{}(i * 7) + std::hash<int>{}(count * 7919);
        }
        return result;
    }
};

struct NodeCmp {
    bool operator()(const Node& lhs, const Node& rhs) const {
        return lhs.time > rhs.time; // std::greater<Node>()
    }
};

int ResultA2Slow(const std::string& str, const Matrix& matrix) {
    Node root;
    for (const char& ch: str) {
        int i = ch - 'A';
        root.ch_count[i]++;
    }

    std::unordered_set<Node, NodeHash> visited({root});
    std::priority_queue<Node, std::vector<Node>, NodeCmp> min_heap;
    min_heap.push(root);
    
    while (!min_heap.empty()) {
        Node node = min_heap.top();
        min_heap.pop();
        
        if (node.IsConsistent()) {
            return node.time;
        }

        visited.insert(node);
        for (int src = 0; src < ALPHA_NUM; src++) {
            if (node.ch_count[src] == 0) {
                continue;
            }

            for (int dst = 0; dst < ALPHA_NUM; dst++) {
                if (src == dst) {
                    continue;
                }
                if (matrix[src][dst] == 0) {
                    continue;
                }

                Node next(node);
                int steps = next.ch_count[src];
                next.ch_count[dst] += steps;
                next.ch_count[src] = 0;
                next.time += steps;

                if (visited.count(next)) {
                    continue;
                }
                min_heap.push(next);
            }
        }
    }
    return -1;
}

static const int MAX_VALUE = std::numeric_limits<int>::max();

Matrix FloydWarshall(const Matrix& matrix)  {
    int N = matrix.size();
    Matrix dist(N, std::vector<int>(N, MAX_VALUE));

    for(int src = 0; src < N; src++) {
        for(int dst = 0; dst < N; dst++) {
            if (matrix[src][dst] != 0) {
                dist[src][dst] = matrix[src][dst];
            }
        }
    }

    for(int ch = 0; ch < N; ch++) {
        dist[ch][ch] = 0;
    }

    for(int relax = 0; relax < N; relax++) {
        for(int src = 0; src < N; src++) {
            for(int dst = 0; dst < N; dst++) {
                if (dist[src][relax] < MAX_VALUE && dist[relax][dst] < MAX_VALUE) {
                    dist[src][dst] = std::min(dist[src][dst], dist[src][relax] + dist[relax][dst]);
                }
            }
        }
    }
    return dist;
}

int StepsToConvergence(const std::string& str, const Matrix& dist, char target) {
    int result = 0;
    for (const char& ch: str) {
        int i = ch - 'A';
        if (dist[i][target] == MAX_VALUE) {
            return MAX_VALUE;
        }
        result += dist[i][target];
    }
    return result;
}

int ResultA2(const std::string& str, const Matrix& matrix) {
    Matrix dist = FloydWarshall(matrix);
    int result = MAX_VALUE;
    for (int target = 0; target < matrix.size(); target++) {
        result = std::min(result, StepsToConvergence(str, dist, target));
    }
    return result == MAX_VALUE ? -1 : result;
}

void SolveA2(const Arguments& args) {
    std::ifstream input(args.input_file);
    std::ofstream output(args.output_file);

    int tasks = 0;
    input >> tasks;
    for (int t = 1; t <= tasks; t++) {
        std::string str;
        input >> str;
        int edges = 0;
        input >> edges;
        Matrix matrix(ALPHA_NUM, std::vector<int>(ALPHA_NUM, 0));
        for (int e = 0; e < edges; e++) {
            char src_ch;
            char dst_ch;
            input >> src_ch;
            input >> dst_ch;
            matrix[src_ch - 'A'][dst_ch - 'A'] = 1;
        }
        output << "Case #" << t << ": " << ResultA2(str, matrix) << std::endl;
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

        SolveA2(args);

    } catch(const std::exception& ex) {
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}