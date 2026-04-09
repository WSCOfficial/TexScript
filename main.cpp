#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <sstream>

std::unordered_map<std::string, int> variables;

std::string trim(const std::string& s) {
    size_t start = s.find_first_not_of(" \t");
    size_t end = s.find_last_not_of(" \t");
    if (start == std::string::npos) return "";
    return s.substr(start, end - start + 1);
}

void runLine(const std::string& lineRaw) {
    std::string line = trim(lineRaw);

    if (line.empty() || line.rfind("//", 0) == 0)
        return;

    if (line.rfind("say ", 0) == 0) {
        std::string content = line.substr(4);

        if (content.front() == '"' && content.back() == '"') {
            std::cout << content.substr(1, content.size() - 2) << std::endl;
        }
        else if (variables.count(content)) {
            std::cout << variables[content] << std::endl;
        }
        return;
    }

    if (line.rfind("set ", 0) == 0) {
        std::string rest = line.substr(4);
        size_t eq = rest.find('=');

        if (eq == std::string::npos) return;

        std::string name = trim(rest.substr(0, eq));
        std::string value = trim(rest.substr(eq + 1));

        variables[name] = std::stoi(value);
        return;
    }
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "Usage: texscript <file.texs>\n";
        return 1;
    }

    std::ifstream file(argv[1]);
    if (!file.is_open()) {
        std::cout << "Could not open file.\n";
        return 1;
    }

    std::string line;
    while (std::getline(file, line)) {
        runLine(line);
    }

    return 0;
}
