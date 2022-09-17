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

static const int MIN_INT = std::numeric_limits<int>::min();

bool ResultA(const std::vector<int>& origin, std::vector<int>& moved, int K) {
    if (origin.size() != moved.size() || origin.empty()) {
        return false;
    }
    if (K == 0) {
        return origin == moved;
    }
    int m_idx = 0 ;
    for (int m = 0; m < moved.size(); m++) {
        if (moved[m] == origin.front()) {
            m_idx = m; 
            break;
        }
    }
    int o_idx = 0;
    int count = 0;
    int shifts = 0;
    size_t size = origin.size();
    while (count < size) {
        if (moved[m_idx] != origin[o_idx]) {
            shifts++;
            count++;
            continue;
        }
        m_idx++;
        m_idx %= moved.size();
        o_idx++;
        o_idx %= origin.size();
        count++;
    }
    return shifts == K - 1;
}

static const std::string YES_PHRASE = "YES";
static const std::string NO_PHRASE = "NO";

void SolveA(const Arguments& args) {
    std::ifstream input(args.input_file);
    std::ofstream output(args.output_file);

    int N;
    int K;

    int tasks;
    input >> tasks;
    for (int t = 1; t <= tasks; t++) {
        std::vector<int> moved;
        std::vector<int> origin;
        input >> N;
        input >> K;
        int o = 0;
        for (int i = 0; i < N; i++) {
            input >> o;
            origin.push_back(o);
        }
        int m = 0;
        for (int i = 0; i < N; i++) {
            input >> m;
            moved.push_back(m);
        }
        output << "Case #" << t << ": ";
        if (ResultA(origin, moved, K)) {
            output << YES_PHRASE;
        } else {
            output << NO_PHRASE;
        }
        output << std::endl;
    }
    return;
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

        SolveA(args);

    } catch(const std::exception& ex) {
        std::cerr << "Exception: " << ex.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}