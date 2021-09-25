#include <algorithm>
#include <deque>
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

std::vector<int> DressModels(std::unordered_map<int, std::deque<int>>& dress_models, const std::unordered_map<int, int>& dress_count) {
    std::vector<int> changed;
    for (auto& [dress, models]: dress_models) {
        auto it = dress_count.find(dress);
        if (it == dress_count.end()) {
            for (const int& model: models) {
                changed.push_back(model);
            }
            models.clear();
            continue;
        }

        int count = it->second;
        while (models.size() > count) {
            changed.push_back(models.front());
            models.pop_front();
        }
    }

    int i = 0;
    for (const auto& [dress, count]: dress_count) {
        std::deque<int>& models = dress_models[dress];
        while (models.size() < count) {
            models.push_back(changed[i++]);
        }
    }
    return changed;
}

int ResultA1(const std::vector<int>& models, const std::vector<std::vector<int>>& shows) {
    int M = models.size();
    int N = shows.size();

    if (models.size() == 1) {
        int changes = 0;
        int prev = models.front();
        for (int n = 0; n < N; n++) {
            int next = shows[n].front();
            if (next != prev) {
                changes++;
            }
            prev = next;
        }
        return std::max(changes - 1, 0);
    }

    std::unordered_map<int, std::deque<int>> dress_models;
    for (int m = 0; m < M; m++) {
        int dress = models[m];
        dress_models[dress].push_back(m);
    }

    int result = 0;
    std::vector<bool> changed(M, false);
    for (int n = 0; n < N; n++) {
        std::unordered_map<int, int> dress_count;
        for (int m = 0; m < M; m++) {
            int dress = shows[n][m];
            dress_count[dress]++;
        }

        std::vector<int> changes = DressModels(dress_models, dress_count);
        for (const int& model: changes) {
            if (changed[model]) {
                result++;
            }
            changed[model] = true;
        }
    }
    return result;
}

void SolveA1(const Arguments& args) {
    std::ifstream input(args.input_file);
    std::ofstream output(args.output_file);

    int tasks = 0;
    input >> tasks;
    for (int t = 1; t <= tasks; t++) {
        int N = 0;
        input >> N;
        int M = 0;
        input >> M;
        std::vector<int> models;
        for (int m = 0; m < M; m++) {
            int dress = 0;
            input >> dress;
            models.push_back(dress);
        }
        std::vector<std::vector<int>> shows;
        for (int n = 0; n < N; n++) {
            int dress;
            std::vector<int> show;
            for (int m = 0; m < M; m++) {
                int dress = 0;
                input >> dress;
                show.push_back(dress);
            }
            shows.push_back(show);
        }
        output << "Case #" << t << ": " << ResultA1(models, shows) << std::endl;
    }
}

void ColorGraph(const std::vector<std::vector<int>>& graph, const std::pair<int, int>& block, int start, std::vector<int>& colors, int color) {
    std::unordered_set<int> visited({start});
    std::queue<int> queue({start});
    while (!queue.empty()) {
        int src = queue.front();
        queue.pop();
        colors[src] = color;
        for (const int& dst: graph[src]) {
            if (visited.find(dst) != visited.end()) {
                continue;
            }
            if (block.first == std::min(src, dst) && block.second == std::max(src, dst)) {
                continue;
            }
            queue.push(dst);
            visited.insert(dst);
        }
    }
}

bool CanBlock(const std::vector<std::vector<int>>& graph, const std::pair<int, int>& block, const std::unordered_map<int, std::vector<int>>& freq_towns) {
    std::vector<int> colors(graph.size(), 0);
    ColorGraph(graph, block, block.first, colors, 1);
    ColorGraph(graph, block, block.second, colors, 2);
    for (const auto& [freq, towns]: freq_towns) {
        if (towns.size() < 2) {
            continue;
        }
        int color = 0;
        for (const int& town: towns) {
            if (color == 0) {
                color = colors[town];
                continue;
            }
            if (color != colors[town]) {
                return false;
            }
        }
    }
    return true;
}

int CutAndCheck(const std::vector<std::vector<int>>& graph, const std::vector<std::pair<int, int>>& edges,
                const std::unordered_map<int, std::vector<int>>& freq_towns) {
    int result = 0;
    for (const std::pair<int, int>& edge: edges) {
        if (CanBlock(graph, edge, freq_towns)) {
            result++;
        }
    }
    return result;
}

struct PairHash {
    std::size_t operator()(const std::pair<int,int>& v) const {
        return v.first * 17 + v.second * 31;
    }
};

using PairSet = std::unordered_set<std::pair<int, int>, PairHash>;

std::vector<int> GetPath(const std::vector<std::vector<int>>& graph, int start, int end) {
    std::unordered_set<int> visited({start});
    std::unordered_map<int, int> parent;
    std::queue<int> queue({start});
    while (!queue.empty()) {
        int src = queue.front();
        queue.pop();
        if (src == end) {
            break;
        }
        for (const int& dst: graph[src]) {
            if (visited.find(dst) != visited.end()) {
                continue;
            }
            queue.push(dst);
            visited.insert(dst);
            parent[dst] = src;
        }
    }
    int node = end;
    std::vector<int> path({node});
    do {
        node = parent[node];
        path.push_back(node);
    } while (node != start);
    return path;
}

int ConnectAndCheck(const std::vector<std::vector<int>>& graph, const std::vector<std::pair<int, int>>& edges,
                    const std::unordered_map<int, std::vector<int>>& freq_towns) {
    PairSet connected;
    for (const auto& [freq, towns]: freq_towns) {
        if (towns.size() < 2) {
            continue;
        }
        std::vector<int> visited(graph.size(), false);
        for (int i = 0; i < towns.size() - 1; i++) {
            if (visited[towns[i]] && visited[towns[i + 1]]) {
                continue;
            }
            std::vector<int> path = GetPath(graph, towns[i], towns[i + 1]);
            for (int j = 0; j < path.size() - 1; j++) {
                int src = path[j];
                int dst = path[j + 1];
                connected.insert({std::min(src, dst), std::max(src, dst)});
                visited[src] = true;
                visited[dst] = true;
            }
        }
    }
    int result = 0;
    for (const std::pair<int, int>& edge: edges) {
        if (connected.find(edge) != connected.end()) {
            continue;
        }
        result++;
    }
    return result;
}

int ResultA2(const std::vector<std::vector<int>>& graph, const std::vector<std::pair<int, int>>& edges, const std::vector<int>& freqs) {
    std::unordered_map<int, std::vector<int>> freq_towns;
    bool can_inform = false;
    for (int t = 0; t < freqs.size(); t++) {
        int freq = freqs[t];
        freq_towns[freq].push_back(t);
        if (freq_towns[freq].size() > 1) {
            can_inform = true;
        }
    }
    if (!can_inform) {
        return graph.size() - 1;
    }
    return ConnectAndCheck(graph, edges, freq_towns);
}

void SolveA2(const Arguments& args) {
    std::ifstream input(args.input_file);
    std::ofstream output(args.output_file);

    int tasks = 0;
    input >> tasks;
    for (int t = 1; t <= tasks; t++) {
        int N = 0;
        input >> N;
        std::vector<std::pair<int, int>> edges;
        std::vector<std::vector<int>> graph(N);
        for (int n = 0; n < N - 1; n++) {
            int src, dst;
            input >> src >> dst;
            src--; dst--;
            graph[src].push_back(dst);
            graph[dst].push_back(src);
            edges.push_back({std::min(src, dst), std::max(src, dst)});
        }
        std::vector<int> freqs;
        for (int n = 0; n < N; n++) {
            int freq;
            input >> freq;
            freqs.push_back(freq);
        }
        output << "Case #" << t << ": " << ResultA2(graph, edges, freqs) << std::endl;
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

    } catch(const std::exception& ex) {
        std::cerr << "Exception: " << ex.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}