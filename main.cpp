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

// NEW: Replace $var$ inside strings
std::string interpolate(const std::string& text, int lineNumber) {
    std::string result;
    for (size_t i = 0; i < text.size(); ) {
        if (text[i] == '$') {
            size_t end = text.find('$', i + 1);
            if (end == std::string::npos) {
                std::cerr << "Error (line " << lineNumber << "): Missing closing '$'.\n";
                return result;
            }

            std::string varName = text.substr(i + 1, end - i - 1);

            if (variables.count(varName)) {
                result += std::to_string(variables[varName]);
            } else {
                std::cerr << "Error (line " << lineNumber << "): Unknown variable '" << varName << "'.\n";
            }

            i = end + 1;
        } else {
            result += text[i];
            i++;
        }
    }
    return result;
}

void runLine(const std::string& lineRaw, int lineNumber) {
    std::string line = trim(lineRaw);

    debug("Line " + std::to_string(lineNumber) + ": \"" + line + "\"");

    if (line.empty() || line.rfind("//", 0) == 0) {
        return;
    }

    // SAY COMMAND
    if (line.rfind("say ", 0) == 0) {
        std::string content = trim(line.substr(4));

        if (content.empty()) {
            std::cerr << "Error (line " << lineNumber << "): Missing content for 'say'.\n";
            return;
        }

        // STRING WITH QUOTES
        if (content.front() == '"' && content.back() == '"' && content.size() >= 2) {
            std::string text = content.substr(1, content.size() - 2);

            // interpolate $var$
            text = interpolate(text, lineNumber);

            std::cout << text << std::endl;
        }
        // VARIABLE NAME
        else {
            if (variables.count(content)) {
                std::cout << variables[content] << std::endl;
            } else {
                std::cerr << "Error (line " << lineNumber << "): Variable '" << content << "' not found.\n";
            }
        }
        return;
    }

    // SET COMMAND
    if (line.rfind("set ", 0) == 0) {
        std::string rest = trim(line.substr(4));
        size_t eq = rest.find('=');

        if (eq == std::string::npos) {
            std::cerr << "Error (line " << lineNumber << "): Missing '=' in 'set'.\n";
            return;
        }

        std::string name = trim(rest.substr(0, eq));
        std::string valueStr = trim(rest.substr(eq + 1));

        if (name.empty()) {
            std::cerr << "Error (line " << lineNumber << "): Missing variable name.\n";
            return;
        }

        try {
            int value = std::stoi(valueStr);
            variables[name] = value;
        } catch (...) {
            std::cerr << "Error (line " << lineNumber << "): Invalid integer '" << valueStr << "'.\n";
        }
        return;
    }

    std::cerr << "Error (line " << lineNumber << "): Unknown command '" << line << "'\n";
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

    if (!endsWith(filename, ".texs")) {
        std::cerr << "Error: File must have .texs extension.\n";
        return 1;
    }

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

    return 0;
}

// v0.1.0 - Initial release with basic 'say' and 'set' commands, variable interpolation, and error handling.