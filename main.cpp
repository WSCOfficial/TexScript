#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>

std::unordered_map<std::string, int> variables;
bool DEBUG_MODE = false;

std::string trim(const std::string& s) {
    size_t start = s.find_first_not_of(" \t");
    size_t end = s.find_last_not_of(" \t");
    if (start == std::string::npos) return "";
    return s.substr(start, end - start + 1);
}

bool endsWith(const std::string& str, const std::string& suffix) {
    return str.size() >= suffix.size() &&
           str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
}

void debug(const std::string& msg) {
    if (DEBUG_MODE) {
        std::cerr << "[DEBUG] " << msg << std::endl;
    }
}

void runLine(const std::string& lineRaw, int lineNumber) {
    std::string line = trim(lineRaw);

    debug("Line " + std::to_string(lineNumber) + ": \"" + line + "\"");

    if (line.empty() || line.rfind("//", 0) == 0) {
        debug("Skipped (empty/comment)");
        return;
    }

    if (line.rfind("say ", 0) == 0) {
        std::string content = trim(line.substr(4));

        if (content.empty()) {
            std::cerr << "Error (line " << lineNumber << "): "
                      << "Missing content for 'say'.\n";
            return;
        }

        if (content.front() == '"' && content.back() == '"' && content.size() >= 2) {
            std::string text = content.substr(1, content.size() - 2);
            debug("say string: " + text);
            std::cout << text << std::endl;
        } else {
            if (variables.count(content)) {
                debug("say variable: " + content + " = " + std::to_string(variables[content]));
                std::cout << variables[content] << std::endl;
            } else {
                std::cerr << "Error (line " << lineNumber << "): "
                          << "Variable '" << content << "' not found.\n";
            }
        }
        return;
    }

    if (line.rfind("set ", 0) == 0) {
        std::string rest = trim(line.substr(4));
        size_t eq = rest.find('=');

        if (eq == std::string::npos) {
            std::cerr << "Error (line " << lineNumber << "): "
                      << "Missing '=' in 'set' statement.\n";
            return;
        }

        std::string name = trim(rest.substr(0, eq));
        std::string valueStr = trim(rest.substr(eq + 1));

        if (name.empty()) {
            std::cerr << "Error (line " << lineNumber << "): "
                      << "Missing variable name in 'set'.\n";
            return;
        }

        if (valueStr.empty()) {
            std::cerr << "Error (line " << lineNumber << "): "
                      << "Missing value in 'set'.\n";
            return;
        }

        try {
            int value = std::stoi(valueStr);
            variables[name] = value;
            debug("set " + name + " = " + std::to_string(value));
        } catch (const std::exception& e) {
            std::cerr << "Error (line " << lineNumber << "): "
                      << "Invalid integer value '" << valueStr << "' in 'set'.\n";
        }
        return;
    }

    std::cerr << "Error (line " << lineNumber << "): "
              << "Unknown command: '" << line << "'\n";
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "Usage: texscript [--debug] <file.texs>\n";
        return 1;
    }

    std::string filename;
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--debug") {
            DEBUG_MODE = true;
        } else {
            filename = arg;
        }
    }

    if (filename.empty()) {
        std::cout << "Usage: texscript [--debug] <file.texs>\n";
        return 1;
    }

    if (!endsWith(filename, ".texs")) {
        std::cerr << "Error: File must have .texs extension.\n";
        return 1;
    }

    debug("Opening file: " + filename);

    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file '" << filename << "'.\n";
        return 1;
    }

    std::string line;
    int lineNumber = 1;
    while (std::getline(file, line)) {
        runLine(line, lineNumber);
        lineNumber++;
    }

    debug("Execution finished.");
    return 0;
}

