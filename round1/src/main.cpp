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

enum class Hand {
    LEFT = -1,
    BOTH = 0,
    RIGHT = +1
};

int ResultA1(const std::string& str) {
    Hand hand = Hand::BOTH;
    Hand wrong = Hand::BOTH;
    Hand right = Hand::BOTH;
    int switches = 0;
    for (const char& ch: str) {
        switch (ch) {
            case 'X': wrong = Hand::RIGHT; right = Hand::LEFT; break;
            case 'O': wrong = Hand::LEFT; right = Hand::RIGHT; break;
            default: continue; // hand = Hand::BOTH;
        }
        if (hand == wrong) {
            switches++;
        }
        hand = right;
    } 
    return switches;
}

void SolveA1(const Arguments& args) {
    std::ifstream input(args.input_file);
    std::ofstream output(args.output_file);

    int tasks = 0;
    input >> tasks;
    for (int t = 1; t <= tasks; t++) {
        int length;
        input >> length;
        std::string str;
        input >> str;
        output << "Case #" << t << ": " << ResultA1(str) << std::endl;
    }
}

struct Interval {
    Hand hand = Hand::BOTH;
    int64_t prefix = 0;
    int64_t length = 0;
};

int ResultA2(const std::string& str) {
    std::vector<Interval> intervals(1);
    Hand hand = Hand::BOTH;
    int prefix = 0;
    int suffix = 0;
    for (const char& ch: str) {
        prefix++;
        switch (ch) {
            case 'X': hand = Hand::LEFT; break;
            case 'O': hand = Hand::RIGHT; break;
            default: hand = Hand::BOTH; break;
        }
        Interval& interval = intervals.back();
        if (hand == Hand::BOTH) {
            interval.length++;
            suffix++;
            continue;
        }
        if (interval.hand == Hand::BOTH) {
            interval.hand = hand;
            interval.length = 0;
        }
        if (interval.hand == hand) {
            interval.prefix = prefix;
            interval.length++;
            suffix = 1;
        } else {
            prefix = suffix;
            intervals.push_back(Interval{hand, prefix, 1});
            suffix = 1;
        }
    }
    if (intervals.size() == 1) {
        return 0;
    }

    int64_t result = 0;
    static const int64_t MOD = 1000000007;
    for (int64_t step = 1; step < intervals.size(); step++) {
        for (int64_t i = 0; i < intervals.size() - step; i++) {
            const Interval& lhs = intervals[i];
            const Interval& rhs = intervals[i + step];
            result += step * lhs.prefix * rhs.length;
            result %= MOD;
        }
    }
    return result;
}

void SolveA2(const Arguments& args) {
    std::ifstream input(args.input_file);
    std::ofstream output(args.output_file);

    int tasks = 0;
    input >> tasks;
    for (int t = 1; t <= tasks; t++) {
        int length;
        input >> length;
        std::string str;
        input >> str;
        output << "Case #" << t << ": " << ResultA2(str) << std::endl;
    }
}

std::string ResultB(int rows, int cols, int tl_br, int tr_bl) {
    std::ostringstream oss;
    int min_length = rows + cols - 1;
    if (tl_br < min_length || tr_bl < min_length) {
        oss << "Impossible";
        return oss.str();
    }
    
    oss << "Possible" << std::endl;
    int max_value = std::max(tl_br, tr_bl);
    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {
            if (c == 0) {
                if (r == 0) {
                    oss << tl_br - min_length + 1;
                } else if (r == rows - 1) {
                    oss << tr_bl - min_length + 1;
                } else {
                    oss << max_value;
                }
                continue;
            }
            oss << " ";
            if (r == 0 || r == rows - 1 || c == cols - 1) {
                oss << 1;
            } else {
                oss << max_value;
            }
            if (c == cols - 1 && r != rows - 1) {
                oss << std::endl;
            }
        }
    }
    return oss.str();
}

void SolveB(const Arguments& args) {
    std::ifstream input(args.input_file);
    std::ofstream output(args.output_file);

    int tasks = 0;
    input >> tasks;
    for (int t = 1; t <= tasks; t++) {
        int rows, cols;
        input >> rows >> cols;
        int tl_br, tr_bl;
        input >> tl_br >> tr_bl;
        output << "Case #" << t << ": " << ResultB(rows, cols, tl_br, tr_bl) << std::endl;
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

        // SolveA1(args);
        SolveA2(args);
        // SolveB(args);

    } catch(const std::exception& ex) {
        std::cerr << "Exception: " << ex.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}